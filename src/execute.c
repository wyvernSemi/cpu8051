//=============================================================
// 
// Copyright (c) 2012 Simon Southwell. All rights reserved.
//
// Date: 25th August 2012  
//
// This file is part of the cpu8051 instruction set simulator.
//
// This file contains the main decode table for the 8051
// instructions, and implements the main executions functions
// apart from the instructions themselves. I.e. it implements
// interrupts, timers, callback registeration, external access
// to 8051 state and basic breakpoint control.
//
// $Id: execute.c,v 1.1 2013-06-25 18:40:48 simon Exp $
// $Source: /home/simon/CVS/src/cpu/8051/src/execute.c,v $
//
//=============================================================

#include <stdio.h>

#include "cpu8051.h"
#include "8051.h"
#include "read_ihx.h"

static int timer (int time, int *wakeup_time, int timer);

// Callback function pointers for interrupt sources
static pintcallback_t  pExtIntCallback0 = NULL;
static pintcallback_t  pExtIntCallback1 = NULL;
static ptimcallback_t  pTimIntCallback0 = NULL;
static ptimcallback_t  pTimIntCallback1 = NULL;
static pintcallback_t  pSerIntCallback  = NULL;

// Callback function pointers for memory accesses
       pmemcallback_t  pExtCallback     = NULL;
       pmemcallback_t  pSfrCallback     = NULL;

// Callback function pointers for regular time updates
       ptimecallback_t pTimeCallback    = NULL;

// Wakeup time state for the various callbacks connected to interrupt sources
static int wakeup_time_ext0             = -1; 
static int wakeup_time_ext1             = -1; 
static int wakeup_time_tim0             = -1; 
static int wakeup_time_tim1             = -1; 
static int wakeup_time_ser              = -1; 

// Wakeup time state for the schedule
static int wakeup_time_sched            = -1; 

// State to detect interrupt edges
static int last_ext0_int                = 0;
static int last_ext1_int                = 0;

// Control variables
static FILE*    ofp = NULL;                  // Stream for output data
static int      disable_lock_break = 0;      // Disable/enable brekpoint on lock conditions
static int      verbose = 0;                 // Verbosity level

// Main decode table for opcodes. Array of DecodeData_t structures, with each tuple consisting of
// <func ptr>, <opcode str>, <num instr bytes>, <exec clk cycles>, <addr mode 1>, <addr mode 2>

static DecodeData_t decode_table [DECODE_TABLE_SIZE] = DECODE_TABLE;

// -------------------------------------------------------------------------
// get_cycle_time()
//
// User called function that returns the current time in cycle counts
//
int get_cycle_time (void) {

    return cycle_count;
}

// -------------------------------------------------------------------------
// get_iram_byte()
//
// User called function to return the value of a byte in internal RAM, *or*
// SFRs, mapped into upper half of IRAM space
//
int get_iram_byte (int addr) {

    if (addr < SFR_START)
        return int_ram[addr & (SFR_START-1)];
    else
        return fetch_sfr(addr);
}

// -------------------------------------------------------------------------
// set_iram_byte()
//
// User called function to set the value of a byte in internal RAM, *and*
// SFRs, mapped into upper half of IRAM space
//
void set_iram_byte (int addr, int data) {

    if (addr < SFR_START)
        int_ram[addr & (SFR_START-1)] = data & 0xff;
    else
        set_sfr(addr, data & 0xff);
}

// -------------------------------------------------------------------------
// get_ext_ram_byte()
//
// User called function to return the value of a byte in external RAM
//
int get_ext_ram_byte (int addr) {

    if (pExtCallback == NULL)
        return ext_ram[addr];
    else
        return pExtCallback(addr, 0, MEM_CB_READ, ext_ram, cycle_count);
}

// -------------------------------------------------------------------------
// set_ext_ram_byte()
//
// User called function to set the value of a byte in external RAM
//
void set_ext_ram_byte (int addr, int data) {

    if (pExtCallback == NULL)
        ext_ram[addr] = data & 0xff;
    else
        pExtCallback(addr, data & 0xff, MEM_CB_WRITE, ext_ram, cycle_count);
}

// -------------------------------------------------------------------------
// set_verbosity_lvl()
//
// User called routine to set verbosity level of debug information. 
// Current only two levels supported: 0 = off, >0 = on
//
void set_verbosity_lvl (int lvl) {

    verbose = lvl;
}

// -------------------------------------------------------------------------
// set_output_stream()
//
// User called function to set the output stream of printed messages
//
void set_output_stream (FILE* file_pointer) {

    ofp = file_pointer;
}

// -------------------------------------------------------------------------
// set_disable_lock_break()
//
// User called function to disable breaking on lock conditions.
//
void set_disable_lock_break () {

    disable_lock_break = 1;
}

// -------------------------------------------------------------------------
// clr_disable_lock_break()
//
// User called function to clear the disabling of breaking on lock 
// conditions
//
void clr_disable_lock_break () {

    disable_lock_break = 0;
}

// -------------------------------------------------------------------------
// register_int_callback()
//
// Routine for registering user functions as callbacks for generating 
// external interrupts.

int register_int_callback (pintcallback_t callback_func, int callback_type) {

    switch(callback_type) {

    case INT_CALLBACK_EXT0:
        pExtIntCallback0 = callback_func;
        break;

    case INT_CALLBACK_EXT1:
        pExtIntCallback1 = callback_func;
        break;

    default:
        fprintf(ofp, "WARNING: undefined callback type (%d) in call to register_int_callback(). Callback not registered\n", callback_type);
        return 1;
        break;
    }

    return 0;
}

// -------------------------------------------------------------------------
// register_time_callback()
//
// Routine for registering user function as callback for regular time
// updates, that don't generate an interrupt (directly)
//
int register_time_callback (ptimecallback_t callback_func) {

    pTimeCallback = callback_func;

    return 0;
}

// -------------------------------------------------------------------------
// register_ext_mem_callback()
//
// Routine for registering user function as callback for accesses to
// external memory.

int register_ext_mem_callback (pmemcallback_t callback_func) {

    pExtCallback = callback_func;

    return 0;
}

// -------------------------------------------------------------------------
// register_sfr_callback()
//
// Routine for registering user function as callback for accesses to
// external memory.

int register_sfr_callback (pmemcallback_t callback_func) {

    pSfrCallback = callback_func;

    return 0;
}

// -------------------------------------------------------------------------
// timer()
//
// Timer model for 8051, shared between timer 1 and timer 0. Current cycle
// count (timer) is passed in, along with an indicator whether timer 1 
// or timer 0 is being processed.

static int timer (int time, int *wakeup_time, int timer) {

    int tmod, tcon, mode0, mode, c_t, gate, p3, th, tl, on;
    int interrupt = 0;
    int tim1_free;

    // Get TMOD data
    tmod = get_iram_byte(SFR_TMOD);
    tcon = get_iram_byte(SFR_TCON);

    mode0 = tmod & 0x3;
    tim1_free = (mode0 == 3 && timer);

    mode  = (timer ? (tmod >> 4) : tmod) & 0x3;
    c_t   = (timer ? (tmod >> 6) : (tmod >> 2)) & 0x1;
    gate  = (timer ? (tmod >> 7) : (tmod >> 3)) & 0x1;
    p3    = get_iram_byte(SFR_P3);

    // Is the timer on?
    on   = (timer ? (tcon >> 6) : (tcon >> 4)) & 0x1;

    // Get the timer value
    tl = get_iram_byte(timer ? SFR_TL1 : SFR_TL0);
    th = get_iram_byte(timer ? SFR_TH1 : SFR_TH0);

    // If timer is on and not gated off and not event counting
    if ( (on && (!(gate && !(p3&0x08)) && !(c_t && !(p3&0x20)))) || tim1_free) {
        switch (mode) {
        case TIMER_13BIT:
            if (++tl == 32) {
                tl = 0;
                if (++th == 256) {
                    th = 0;
                    interrupt = (!timer || !tim1_free);
                    set_iram_byte(SFR_TCON, tcon | ~(timer ? 0x80 : 0x20));
                } 
            }
            break;
        case TIMER_16BIT:
            if (++tl == 256) {
                tl = 0;
                if (++th == 256) {
                    th = 0;
                    interrupt = (!timer || !tim1_free);
                    set_iram_byte(SFR_TCON, tcon | ~(timer ? 0x80 : 0x20));
                } 
            }
            break;
        case TIMER_8BIT_AUTO:
            if (++tl == 256) {
                tl = th;
                interrupt = (!timer || !tim1_free);
                set_iram_byte(SFR_TCON, tcon | ~(timer ? 0x80 : 0x20));
            }
            break;
        case TIMER_SPLIT:
            if (++tl == 256) {
                tl = 0;
                interrupt = 1;
                set_iram_byte(SFR_TCON, tcon | 0x20);
            }
            break;
        }

        set_iram_byte(timer ? SFR_TL1 : SFR_TL0, tl);
        set_iram_byte(timer ? SFR_TH1 : SFR_TH0, th);

        // If timer 1 and timer 0 mode is split, then TH0 is the virtual 
        // timer 1
        if (tim1_free && (on && !(gate && !(p3&0x08)) && !(c_t && !(p3&0x20)))) {
            th = get_iram_byte(SFR_TH0);
            th++;
            if (th == 256) {
                th = 0;
                interrupt = 1;
                set_iram_byte(SFR_TCON, tcon | 0x80);
            }
            set_iram_byte(SFR_TH0, th);
        }
    }

    return interrupt;
}

// -------------------------------------------------------------------------
// interrupt()
//
// Function called on any interrupt. Sets the priority level, and saves
// the level at callup, before pusing the return address on the stack and
// setting the PC to the indicated interrupt vector address.

static void interrupt (int vector_address, int priority) {

    // Set the current priority and save the previous level
    last_int_level = int_level;
    int_level = priority;

    // Push the address of the next instruction onto the stack 
    int_ram[++sp] =  pc & 0xff;
    int_ram[++sp] = (pc >> 8) & 0xff;

    // Clear the TCON bit for EXT0 or EXT1 interrupts
    if (vector_address == EXT0_INT_VECTOR)
        set_iram_byte(SFR_TCON, get_iram_byte(SFR_TCON) & ~TCON_IE0_MSK);
    else if (vector_address == EXT1_INT_VECTOR)
        set_iram_byte(SFR_TCON, get_iram_byte(SFR_TCON) & ~TCON_IE1_MSK);

    pc = vector_address;
}

// -------------------------------------------------------------------------
// process_interrupts()
//
// Checks for the registered interrupt callbacks and invokes them for 
// interrupt status. If any interrupt is active, the interrupt vector
// is called, depending on priroity and enables.

static void process_interrupts(void) {

    int timer_int, ext_int, tcon;

    // Get TCON value
    tcon = get_iram_byte(SFR_TCON);

    // Check for interrupts (only if globally enabled and a high level interrupt isn't already active, 
    // as this can't be interrupted).
    if ((ie & IP_EA_BIT) && int_level != INT_LVL_HI) {

        // If there's an external 0 callback and not sleeping, check for interrupt...
        if (pExtIntCallback0 != NULL && (wakeup_time_ext0 <= cycle_count)) {

            // Call callback function and get interrupt status
            ext_int = pExtIntCallback0(cycle_count, &wakeup_time_ext0);

            // If indicating an interrupt...
            if (ext_int) {

                // If edge triggered and interrupt active last time, clear ext_int as if interrupt never happened
                if ((tcon & TCON_IT0_MSK) && last_ext0_int)
                    ext_int = 0;
                else {
                    // Indicating an interrupt, so update the IE0 bit in TCON
                    tcon |= TCON_IE0_MSK;
                    set_iram_byte(SFR_TCON, tcon);
                }

                // Remember ext_int set status for next time
                last_ext0_int = 1;
            } else
                // Remember ext_int clear status for next time
                last_ext0_int = 0;

            // If interrupt enabled, and priority indicates interrupt allowed, check if callback indicates an interrupt....
            if ((ie & IP_PX0_MSK) && (((ip & IP_PX0_MSK) && (int_level != INT_LVL_HI)) || (int_level == INT_LVL_OFF)) && (tcon & TCON_IE0_MSK))
                interrupt(EXT0_INT_VECTOR, (ip & IP_PX0_MSK) ? INT_LVL_HI : INT_LVL_LO);
        }
        
        // If there's a timer 0 callback and not sleeping, check for interrupt...
        if (pTimIntCallback0 != NULL && (wakeup_time_tim0 <= cycle_count)) {

            // Call the timer all the time
            timer_int = pTimIntCallback0(cycle_count, &wakeup_time_tim0, 0);

            // If interrupt enabled, and priority indicates interrupt allowed, check if callback indicates an interrupt....
            if ((ie & IP_PT0_MSK) && (((ip & IP_PT0_MSK) && (int_level != INT_LVL_HI)) || (int_level == INT_LVL_OFF)) && timer_int != 0) 
                interrupt(TIM0_INT_VECTOR, (ip & IP_PT0_MSK) ? INT_LVL_HI : INT_LVL_LO);
        } 
        
        // If there's an external 1 callback and not sleeping, check for interrupt...
        if (pExtIntCallback1 != NULL && (wakeup_time_ext1 <= cycle_count)) {

            // Call callback function and get interrupt status
            ext_int = pExtIntCallback1(cycle_count, &wakeup_time_ext1);

            // If indicating an interrupt...
            if (ext_int) {

                // If edge triggered and interrupt active last time, clear ext_int as if interrupt never happened
                if ((tcon & TCON_IT1_MSK) && last_ext1_int)
                    ext_int = 0;
                else {
                    // Indicating an interrupt, so update the IE0 bit in TCON
                    tcon |= TCON_IE1_MSK;
                    set_iram_byte(SFR_TCON, tcon);
                }

                // Remember ext_int set status for next time
                last_ext1_int = 1;
            } else
                // Remember ext_int clear status for next time
                last_ext1_int = 0;

            // If interrupt enabled, and priority indicates interrupt allowed, check if callback indicates an interrupt....
            if ((ie & IP_PX1_MSK) && (((ip & IP_PX1_MSK) && (int_level != INT_LVL_HI)) || (int_level == INT_LVL_OFF)) && (tcon & TCON_IE1_MSK)) 
                interrupt(EXT1_INT_VECTOR, (ip & IP_PX1_MSK) ? INT_LVL_HI : INT_LVL_LO);
        } 
        
        // If there's a timer 1 callback and not sleeping, check for interrupt...
        if (pTimIntCallback1 != NULL && (wakeup_time_tim1 <= cycle_count)) {

            // Call the timer all the time
            timer_int = pTimIntCallback1(cycle_count, &wakeup_time_tim1, 1);

            // If interrupt enabled, and priority indicates interrupt allowed, check if callback indicates an interrupt....
            if ((ie & IP_PT1_MSK) && (((ip & IP_PT1_MSK) && (int_level != INT_LVL_HI)) || (int_level == INT_LVL_OFF)) && timer_int != 0) 
                interrupt(TIM1_INT_VECTOR, (ip & IP_PT1_MSK) ? INT_LVL_HI : INT_LVL_LO);
        } 
        
        // If there's a serial interface callback and not sleeping, check for interrupt...
        if (pSerIntCallback != NULL && (wakeup_time_ser <= cycle_count)) {
            // If interrupt enabled, and priority indicates interrupt allowed, check if callback indicates an interrupt....
            if ((ie & IP_PS_MSK) && (((ip & IP_PS_MSK) && (int_level != INT_LVL_HI)) || (int_level == INT_LVL_OFF)) && pSerIntCallback(cycle_count, &wakeup_time_ser) != 0) 
                interrupt(SER_INT_VECTOR, (ip & IP_PS_MSK) ? INT_LVL_HI : INT_LVL_LO);
        }
    }
}

// -------------------------------------------------------------------------
// execute()
//
// Main execution function. Fetches the opcode pointed to by the PC, does
// a lookup in the main table, fetches any argument bytes follwoing the 
// opcode and then calls the function retrieved in the table lookup to
// action the intruction.

static void execute (pDecode_t d) {

    // Fetch the next instruction opcode
    d->opcode = code_mem[pc];

    // Get decode information from the master table
    d->decode = &decode_table[d->opcode];

    // Fetch any indicated argument bytes
    if (d->decode->instr_size > 1)
        d->arg0 = code_mem[pc+1];

    if (d->decode->instr_size > 2)
        d->arg1 = code_mem[pc+2];

    if (verbose) {
        fprintf(ofp, "0x%04x: %s (0x%02x): op0 = 0x%02x : op1 = 0x%02x : acc=0x%02x r0=%02x r1=%02x dptr=%04x : cycle = %d\n", 
                             pc, d->decode->instr_name, d->opcode, d->arg0, d->arg1, acc, r[0], r[1], dptr,
                             cycle_count);
    }

    // Execute the indicated instruction
    d->decode->func(d);
}

// -------------------------------------------------------------------------
// run_program()
//
// Main program loop. It will load the code to code memory from a specified 
// filename argument. It's responsible for servicing the interrupts and 
// timer callbacks (if enabled), before executing the next instruction
// indicated by the PC. Rudimentary break points are supported, with user
// configurable breaking on an address, or on a detactble 'loop forever'
// instruction. The parameters include a cycle count which specifies looping
// from a single instruction to 'FOREVER' (barring break points).

int run_program (char* filename, int cycles, int break_addr, int timer_enable) {

    Decode_t decode;
    pDecode_t d = &decode;

    // Only assign the timer callbacks if the timer is enabled by user
    if (timer_enable & 0x1)
        pTimIntCallback0 = timer;

    if (timer_enable & 0x2)
        pTimIntCallback1 = timer;

    // Make sure that the output stream points to something
    if (ofp == NULL)
        ofp = stdout;

    // Load program
    if (read_ihx(filename, code_mem))
        return 1;

    // Reset the MCU
    reset_cpu();

    // Loop executing instructions either forever (if cycles negative) or
    // until cycle_count >= cycles
    while (((cycles == FOREVER) || (cycle_count < cycles)) && (!break_point)) {

        // Check for interrupts and process
        process_interrupts();

        // If there is a scheduled time callback function registered, and wake up time 
        // has passed, call the function.
        if (pTimeCallback != NULL && wakeup_time_sched <= cycle_count)
            pTimeCallback(cycle_count, &wakeup_time_sched);

        // Execute the next instruction
        execute(d);

        // If reached the user specified breakpoint address (and break_point not set by 
        // previous instruction execution) flag to terminate the loop
        if (pc == break_addr && !break_point)
            break_point = USER_BREAK;

        // Trap on lock conditions (often used at end of program)...
        if (!disable_lock_break) {
            // label0 : SJMP label0 (0x80 0xfe)
            if (d->opcode == 0x80 && d->arg0 == 0xfe)
                break_point = LOCK_BREAK;
        }
    }

    return break_point;
}

