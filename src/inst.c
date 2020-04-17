//=============================================================
// 
// Copyright (c) 2012 Simon Southwell. All rights reserved.
//
// Date: 25th August 2012  
//
// This file is part of the cpu8051 instruction set simulator.
//
// This file contains all the functions to implement the 8051
// instructions. It has the main 8051 state and functions to
// access this state, decoding the various adderssing modes.
//
// $Id: inst.c,v 1.3 2020/04/17 10:51:16 simon Exp $
// $Source: /home/simon/CVS/src/cpu/8051/src/inst.c,v $
//
//=============================================================

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "8051.h"

// Memory
       uint8_t  code_mem [MAXCODEMEM];
       uint8_t  ext_ram  [MAXEXTRAM];
       uint8_t  int_ram  [MAXINTRAM];


// Registers (acc, b and sp are also SFRs)
       int      acc;            // Accumulator
       uint8_t* r;              // Registers (pointer mapped over active register bank in int_ram)
       int      sp;             // Stack pointer
       int      pc;             // Current program counter value
static int      b;              // The B register

// Special function regsiters
       int      dptr;           // Data pointer register
static int      tcon;           // Timer Control
static int      tmod;
static int      tl0;            // Timers
static int      tl1;
static int      th0;
static int      th1;
static int      scon;           // Serial
static int      sbuf;
static int      pcon;           // Port control
static int      p0;             // ports
static int      p1;             // ports
static int      p2;
static int      p3;
static int      psw;
       int      ie;             // Interrupts
       int      ip;

// State not directly modelling 8051 state
       int      cycle_count;    // Cycle count
       int      break_point;    // Flag to indicate a termination/break point

// Interrupt level state
       int      int_level;
       int      last_int_level;


// -------------------------------------------------------------------------
// odd_parity8()
//
// Returns 1 if val (lower 8 bits) is odd parity, else 0
//
int odd_parity8(unsigned val) {
    
    unsigned x = val & 0xff;
    
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return (x) & 1;
}

// -------------------------------------------------------------------------
// fetch_sfr()
//
// Returns the value of the addressed SFR, unless the address is outside a
// valid SFR, in which case the internal ram byte is returned for that 
// address. Accessing invalid SFRs is 'undefined' behaviour, so extra IRAM
// should be okay.
//
int fetch_sfr (int addr) {
    switch (addr) {
    case SFR_P0   : return p0;
        break;
    case SFR_SP   : return sp;
        break;
    case SFR_DPL  : return dptr & 0xff;
        break;
    case SFR_DPH  : return (dptr >> 8) & 0xff;
        break;
    case SFR_PCON : return pcon;
        break;
    case SFR_TCON : return tcon;
        break;
    case SFR_TMOD : return tmod;
        break;
    case SFR_TL0  : return tl0;
        break;
    case SFR_TL1  : return tl1;
        break;
    case SFR_TH0  : return th0;
        break;
    case SFR_TH1  : return th1;
        break;
    case SFR_P1   : return p1;
        break;
    case SFR_SCON : return scon;
        break;
    case SFR_SBUF : return sbuf;
        break;
    case SFR_P2   : return p2;
        break;
    case SFR_IE   : return ie;
        break;
    case SFR_P3   : return p3;
        break;
    case SFR_IP   : return ip;
        break;
    case SFR_PSW  : return psw;
        break;
    case SFR_ACC  : return acc;
        break;
    case SFR_B    : return b;   
        break;
    default       : return (pSfrCallback == NULL) ? int_ram[addr] : pSfrCallback(addr, 0, MEM_CB_READ, int_ram, cycle_count);
        break;
    }
}

// -------------------------------------------------------------------------
// set_sfr()
//
// Updates the value of the addressed SFR, unless the address is outside a
// valid SFR, in which case the internal ram byte is updated for that 
// address. Accessing invalid SFRs is 'undefined' behaviour, so extra IRAM
// should be okay.
//
void set_sfr (int addr, int arg) {

    switch (addr) {

    case SFR_P0   : p0      = arg;
        break;
    case SFR_SP   : sp      = arg;
        break;
    case SFR_DPL  : dptr    = (dptr & 0xff00) | (arg & 0xff);
        break;
    case SFR_DPH  : dptr    = (dptr & 0x00ff) | ((arg & 0xff) << 8);
        break;
    case SFR_PCON : pcon    = arg;
        break;
    case SFR_TCON : tcon    = arg;
        break;
    case SFR_TMOD : tmod    = arg;
        break;
    case SFR_TL0  : tl0     = arg;
        break;
    case SFR_TL1  : tl1     = arg;
        break;
    case SFR_TH0  : th0     = arg;
        break;
    case SFR_TH1  : th1     = arg;
        break;
    case SFR_P1   : p1      = arg;
        break;
    case SFR_SCON : scon    = arg;
        break;
    case SFR_SBUF : sbuf    = arg;
        break;
    case SFR_P2   : p2      = arg;
        break;
    case SFR_IE   : ie      = arg;
        break;
    case SFR_P3   : p3      = arg;
        break;
    case SFR_IP   : ip      = arg;
        break;
    case SFR_PSW  : 
        psw = arg;
        // Update register bank on RS bits
        r = &int_ram[psw & (PSW_RS0 | PSW_RS1)];
        break;
    case SFR_ACC  : 
        acc     = arg;
        SET_PSW_P(psw, odd_parity8(acc));
        break;
    case SFR_B    : b       = arg;
        break;
    default       : (pSfrCallback == NULL) ? int_ram[addr] = arg : pSfrCallback(addr, arg, MEM_CB_WRITE, int_ram, cycle_count);
        break;
    }
}


// -------------------------------------------------------------------------
// fetch_arg()
// 
// Fetch an instructions argument value based on the address 'mode', and 
// return in *arg.
// 
static void fetch_arg(int mode, int* arg, int oparg0, int oparg1) {

    switch (mode) {

    case IMM:   *arg = oparg0;
        break;
    case IMM16: *arg = oparg1 | (oparg0 << 8);
        break;
    case DIR:   *arg = ((oparg0 & 0xff) < SFR_START) ? int_ram[oparg0 & 0xff] : fetch_sfr(oparg0 & 0xff);
        break;
    case IND0:  *arg = ((r[0] & 0xff) < SFR_START) ? int_ram[r[0] & 0xff] : 0; // Indirect does not access SFRs
        break;
    case IND1:  *arg = ((r[1] & 0xff) < SFR_START) ? int_ram[r[1] & 0xff] : 0; // Indirect does not access SFRs
        break;
    case EXT:   *arg = (pExtCallback == NULL) ? ext_ram[dptr] : pExtCallback(dptr, 0, MEM_CB_READ, ext_ram, cycle_count);
        break;
    case EXT0:  *arg = (pExtCallback == NULL) ? ext_ram[r[0]] : pExtCallback(r[0], 0, MEM_CB_READ, ext_ram, cycle_count);
        break;
    case EXT1:  *arg = (pExtCallback == NULL) ? ext_ram[r[1]] : pExtCallback(r[1], 0, MEM_CB_READ, ext_ram, cycle_count);
        break;
    case ACC:   *arg = acc;
        break;
    case BREG:  *arg = b;
        break;
    case REG0:  *arg = r[0];
        break;
    case REG1:  *arg = r[1];
        break;
    case REG2:  *arg = r[2];
        break;
    case REG3:  *arg = r[3];
        break;
    case REG4:  *arg = r[4];
        break;
    case REG5:  *arg = r[5];
        break;
    case REG6:  *arg = r[6];
        break;
    case REG7:  *arg = r[7];
        break;
    case DPTR:  *arg = dptr;
        break;
    case CC:    *arg = GET_PSW_CY(psw);
        break;
    case PC:    *arg = pc;
        break;
    case BIT:   *arg = (oparg0 >= SFR_START) ? ((fetch_sfr(oparg0 & 0xf8) >> (oparg0 & 0x7)) & 1) :
                                               ((int_ram[0x20 + (oparg0 >> 3)] >> (oparg0 & 0x7)) & 1);
        break;
    case NBIT:  *arg = ~((oparg0 >= SFR_START) ? ((fetch_sfr(oparg0 & 0xf8) >> (oparg0 & 0x7)) & 1) :
                                                ((int_ram[0x20 + (oparg0 >> 3)] >> (oparg0 & 0x7)) & 1)) & 1;
        break;
    }
}

// -------------------------------------------------------------------------
// write_arg()
//
// Write the argument "arg" to a location based on the addressing 'mode'
//
static void write_arg (int mode, int arg, int oparg0) {       

    switch (mode) {

    case DIR:  
        if ((oparg0 & 0xff) < SFR_START)
           int_ram[oparg0 & 0xff] = arg;
        else
           set_sfr(oparg0 & 0xff, arg);
        break;

    case IND0: 
        if ((r[0] & 0xff) < SFR_START)
            int_ram[r[0] & 0xff] = arg;
        break;
    case IND1: 
        if ((r[1] & 0xff) < SFR_START)
            int_ram[r[1] & 0xff] = arg;
        break;
    case EXT:  
        if (pExtCallback == NULL)
            ext_ram[dptr] = arg;
        else
            pExtCallback(dptr, arg, MEM_CB_WRITE, ext_ram, cycle_count);
        break;
    case EXT0: 
        if (pExtCallback == NULL)
            ext_ram[r[0]] = arg;
        else
            pExtCallback(r[0], arg, MEM_CB_WRITE, ext_ram, cycle_count);
        break;
    case EXT1: 
        if (pExtCallback == NULL)
            ext_ram[r[1]] = arg;
        else
            pExtCallback(r[1], arg, MEM_CB_WRITE, ext_ram, cycle_count);
        break;
    case ACC:
        acc  = arg;
        SET_PSW_P(psw, odd_parity8(acc));
        break;
    case BREG: b    = arg;
        break;
    case REG0: r[0] = arg;
        break;
    case REG1: r[1] = arg;
        break;
    case REG2: r[2] = arg;
        break;
    case REG3: r[3] = arg;
        break;
    case REG4: r[4] = arg;
        break;
    case REG5: r[5] = arg;
        break;
    case REG6: r[6] = arg;
        break;
    case REG7: r[7] = arg;
        break;
    case DPTR: dptr = arg;
        break;
    case CC:   SET_PSW_CY(psw, arg & 1);
        break;
    // Address of 0x00 to 0x7F is bit addressable RAM in locations 0x20 to 0x2F, else
    // it is a bit addressable SFR
    case BIT: 
        if (oparg0 >= SFR_START)
            set_sfr(oparg0 & 0xf8, (fetch_sfr(oparg0 & 0xf8) & ~(1 << (oparg0 & 0x7))) | ((arg & 1) << (oparg0 & 0x7)));
        else
            int_ram[BIT_RAM_START + (oparg0 >> 3) & 0xff] = (int_ram[BIT_RAM_START + (oparg0 >> 3) & 0xff] & ~(1 << (oparg0 & 0x7))) |
                                                            ((arg & 1) << (oparg0 & 0x7));
        break;
    }
}

// -------------------------------------------------------------------------
// reset_cpu()
// 
// Reset the state of the CPU as at power-up
//
void reset_cpu()
{
    int idx;

    // 8051 state
    acc  = 0;
    r    = &int_ram[0];
    b    = 0;
    dptr = 0;
    pc   = 0;
    sp   = 0;
    psw  = 0;
    ie   = 0;
    ip   = 0;
    th0  = 0;
    th1  = 0;
    tl0  = 0;
    tl1  = 0;
    tcon = 0;
    tmod = 0;
    pcon = 0;
    p0   = 0;
    p1   = 0;
    p2   = 0;
    p3   = 0;
    scon = 0;
    sbuf = 0;

    // Model state
    cycle_count    = 0;
    break_point    = 0;
    int_level      = INT_LVL_OFF;
    last_int_level = INT_LVL_OFF;

    // Clear RAM (is this necessary?)
    for (idx = 0; idx < MAXINTRAM; idx++)
        int_ram[idx] = 0;
}

// -------------------------------------------------------------------------
// ACALL 
//
// Absolute call within 2K block
//
//    ACALL page<n>
//
void ACALL (pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    // Push the address of the next instruction onto the stack (ACALL is 2 bytes)
    int_ram[++sp] = (pc  + d->decode->instr_size) & 0xff;
    int_ram[++sp] = ((pc + d->decode->instr_size) >> 8) & 0xff;

    // Update the PC
    pc = (pc & 0xf800) | ((d->opcode >> 5) << 8) | code_mem[pc+1];
}

// -------------------------------------------------------------------------
// ADD (and ADCC)
//
// ADD (ADD with carry)
//
//    ADD[C] A, #data
//    ADD[C] A, iram addr
//    ADD[C] A, @R0
//    ADD[C] A, @R1
//    ADD[C] A, Rx                      (where x is 0 to 7)
//
void ADD (pDecode_t d) {

    int with_carry;
    int old_acc;
    int arg;
    int add_val;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    // Flag if with carry
    with_carry = d->opcode & 0x10;

    // Fetch any external data
    fetch_arg(d->decode->addr_mode_op2, &arg, d->arg0, d->arg1);

    old_acc = acc;

    // Perform addition
    add_val = arg + ((with_carry) ? ((psw & (1 << PSW_CY)) ? 1 : 0) : 0);
    acc += add_val;

    // Update flags
    SET_PSW_CY(psw, acc & 0x100);
    SET_PSW_AC(psw, (((old_acc & 0xf) + (add_val & 0xf)) & 0x10) ? 1 : 0);
    SET_PSW_OV(psw, (old_acc ^ acc) & 0x80);
    SET_PSW_P(psw, odd_parity8(acc));

    // Discard carry/overflow information
    acc &= 0xff;

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// AJMP
//
// Absolute jump with 2k block
//
//    AJMP page<n>
//
void AJMP (pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    pc = (pc & 0xf800) | d->arg0 | ((d->opcode & 0xe0) << 3);
}

// -------------------------------------------------------------------------
// ANL
//
// Bitwise AND
//
//    ANL iram_addr, A
//    ANL iram_addr, #data
//    ANL A, #data
//    ANL A, iram addr
//    ANL A, @R0
//    ANL A, @R1
//    ANL A, Rx                 (where x is 0 to 7)
//    ANL C, bit
//    ANL C, ~bit
//
void ANL (pDecode_t d) {

    int op2, op1;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);

    // Only a single byte argument, fetch op2 based on d->arg0 value
    if (d->decode->instr_size != 3) 
        fetch_arg(d->decode->addr_mode_op2, &op2, d->arg0, d->arg1);

    // If both an arg0 and arg1 byte, move arg1 to arg0 temporarily
    // so fetch_arg() operates the same as for op1 fetching
    else 
        fetch_arg(d->decode->addr_mode_op2, &op2, d->arg1, d->arg0);

    op1 &= op2;

    write_arg(d->decode->addr_mode_op1, op1, d->arg0);

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// CJNE
//
// Compare and jump if not equal
//
//    CJNE A,   #data,     reladdr
//    CJNE A,   iram addr, reladdr
//    CJNE @R0, #data,     reladdr
//    CJNE @R1, #data,     reladdr
//    CJNE Rx,  #data,     reladdr      (where x is 0 to 7)
//
void CJNE (pDecode_t d) {
    int op1, op2;
    
    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);
    fetch_arg(d->decode->addr_mode_op2, &op2, d->arg0, d->arg1);

    pc += d->decode->instr_size;

    if (op1 != op2) {
        pc += (d->arg1 & 0x80) ? -1 * ((d->arg1 ^ 0xff) + 1) :( d->arg1 & 0x7f);

        // If jumping, takes an extra cycle
        cycle_count++;
    }

    SET_PSW_CY(psw, (op1 < op2) ? 1 : 0);
}

// -------------------------------------------------------------------------
// CLR
//
// Clear register
//
//    CLR C
//    CLR A
//
void CLR (pDecode_t d) {

    int offset, bit;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    // C or ACC
    if (d->decode->addr_mode_op1 != BIT) {
        write_arg(d->decode->addr_mode_op1, 0, d->arg0);
    // Bit addressable IRAM/SFR
    } else {
        // IRAM
        if (d->arg0  < SFR_START) {
            offset = BIT_RAM_START + ((d->arg0 >> 3) & 0xf);
            bit    = d->arg0 & 0x7;

            int_ram[offset] = int_ram[offset] & ~(1 << bit);
        // SFR
        } else {
            offset = d->arg0 & 0xf8;
            bit    = d->arg0 & 0x7;

            set_sfr(offset, fetch_sfr(offset) & ~(1 << bit));
        }
    }

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// CPL
//
// Complement register
//
//    CPL A
//    CPL C
//
void CPL (pDecode_t d) {

    int op1;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);

    op1 ^= 0xff;

    write_arg(d->decode->addr_mode_op1, op1, d->arg0);

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// DA
//
// Decimal adjust accumulator
//
void DA (pDecode_t d) {

    int op1, c, ac;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);

    ac = GET_PSW_AC(psw);
    c  = GET_PSW_CY(psw);

    if (ac || ((op1 & 0xf) > 9))
       acc += 6;

    c = (acc & 0x100) ? 1 : c;
    SET_PSW_CY(psw, c);

    if (c || ((acc & 0xf0) > 0x90))
        acc += 0x60;

    c = (acc & 0x100) ? 1 : c;
    SET_PSW_CY(psw, c);
    SET_PSW_P(psw, odd_parity8(acc));

    acc &= 0xff;

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// DEC
// 
// Decrement register
// 
//     DEC A
//     DEC iram addr
//     DEC @R0
//     DEC @R1
//     DEC Rx           (where x is 0 to 7)
//
void DEC (pDecode_t d) {

    int op1;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);

    op1 -= 1;

    write_arg(d->decode->addr_mode_op1, op1 & 0xff, d->arg0);

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// DIV
//  
// Divide accumulator by register B
//
//    DIV AB
//
void DIV (pDecode_t d) {
    int op1, op2, tmp;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);
    fetch_arg(d->decode->addr_mode_op2, &op2, d->arg0, d->arg1);

    SET_PSW_OV(psw, op2 == 0 ? 1 : 0);

    // Clear carry
    SET_PSW_CY(psw, 0);

    if (op2) {
        tmp = op1 % op2;
        op1 = op1 / op2;
    }

    write_arg(d->decode->addr_mode_op1, op1, d->arg0);
    write_arg(d->decode->addr_mode_op2, tmp, d->arg0);

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// DJNZ
//
// Decrement and jump if not zero
//
//    DJNZ iram addr, reladdr
//    DJNZ Rx, reladdr                  (where x is 0 to 7)
//
void DJNZ (pDecode_t d) {

    int op1;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);

    op1 = (op1 -1) & 0xff;

    write_arg(d->decode->addr_mode_op1, op1, d->arg0);

    // If this is a 3 byte instruction, move the 3rd byte to be as if byte 2, so that
    // the PC update below is common to all instructions.
    if (d->decode->instr_size == 3)
        d->arg0 = d->arg1;

    // Jump this instruction at least
    pc += d->decode->instr_size;

    // If not zero, then add the offset
    if (op1) {
        pc += (d->arg0 & 0x80) ? -1 * ((d->arg0 ^ 0xff) + 1) : ( d->arg0 & 0x7f);

        // If jumping, takes an extra cycle
        cycle_count++;
    }
}

// -------------------------------------------------------------------------
// INC
//
// Increment register
//
//    INC A
//    INC iram addr
//    INC @R0
//    INC @R1
//    INC @Rx                           (where x is 0 to 7)
//
void INC (pDecode_t d) {
    int op1;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);

    op1 += 1;

    if (d->decode->addr_mode_op1 == DPTR)
       op1 &= 0xffff;
    else
       op1 &= 0xff;

    write_arg(d->decode->addr_mode_op1, op1, d->arg0);

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// JB
//
// Jump if bit set
//
//    JB bit addr, reladdr
//
void JB (pDecode_t d) {
    int op1;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);

    pc += d->decode->instr_size;

    if (op1) {
        pc += (d->arg1 & 0x80) ? -1 * ((d->arg1 ^ 0xff) + 1) : ( d->arg1 & 0x7f);

        // If jumping, an extra cycle is used
        cycle_count++;
    }

}

// -------------------------------------------------------------------------
// JBC
//
// Jump if bit set and clear bit
//
//    JBC bit addr, reladdr
//
void JBC (pDecode_t d) {

    int op1;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);

    write_arg(d->decode->addr_mode_op1, 0, d->arg0);

    pc += d->decode->instr_size;

    if (op1) {
        pc += (d->arg1 & 0x80) ? -1 * ((d->arg1 ^ 0xff) + 1) : ( d->arg1 & 0x7f);

        // If jumping, an extra cycle is used
        cycle_count++;
    }
}

// -------------------------------------------------------------------------
// JC
//
// Jump if carry set
//
//    JC reladdr
//
void JC (pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    pc += d->decode->instr_size;

    if (GET_PSW_CY(psw)) {
        pc += (d->arg0 & 0x80) ? -1 * ((d->arg0 ^ 0xff) + 1) : (d->arg0 & 0x7f);

        // If jumping, an extra cycle is used
        cycle_count++;
    }
}

// -------------------------------------------------------------------------
// JMP
//
// Unconditional jump to DPTR+A
//
//    JMP @A+DPTR
//
void JMP (pDecode_t d) {

	// Update the cycle count
    cycle_count += d->decode->clk_cycles;

    pc = acc + dptr;
}

// -------------------------------------------------------------------------
// JNB
//
// Jump if bit not set
//
//     JNB bit addr, reladdr
//
void JNB (pDecode_t d) {

    int op1;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);

    pc += d->decode->instr_size;

    if (!op1) {
        pc += (d->arg1 & 0x80) ? -1 * ((d->arg1 ^ 0xff) + 1) : ( d->arg1 & 0x7f);

        // If jumping, an extra cycle is used
        cycle_count++;
    }
}

// -------------------------------------------------------------------------
// JNC
//
// Jump if carry not set
//
//    JNC reladdr
//
void JNC (pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    pc += d->decode->instr_size;

    if (!GET_PSW_CY(psw)) {
        pc += (d->arg0 & 0x80) ? -1 * ((d->arg0 ^ 0xff) + 1) : (d->arg0 & 0x7f);

        // If jumping, an extra cycle is used
        cycle_count++;
    }
}

// -------------------------------------------------------------------------
// JNZ
//
// Jump if accumulator not zero
//
//    JNZ reladdr
//
void JNZ (pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    pc += d->decode->instr_size;

    if (acc) {
        pc += (d->arg0 & 0x80) ? -1 * ((d->arg0 ^ 0xff) + 1) : (d->arg0 & 0x7f);

        // If jumping, an extra cycle is used
        cycle_count++;
    }
}

// -------------------------------------------------------------------------
// JZ
//
// Jump if accumulator zero
//
//    JZ reladdr
//
void JZ (pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    pc += d->decode->instr_size;

    if (!acc) {
        pc += (d->arg0 & 0x80) ? -1 * ((d->arg0 ^ 0xff) + 1) : (d->arg0 & 0x7f);

        // If jumping, an extra cycle is used
        cycle_count++;
    }
}

// -------------------------------------------------------------------------
// LCALL
//
// Long call
//
//    LCALL code addr
//
void LCALL (pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    // Push the address of the next instruction onto the stack (LCALL is 3 bytes)
    int_ram[++sp] = ( pc + d->decode->instr_size) & 0xff;
    int_ram[++sp] = ((pc + d->decode->instr_size) >> 8) & 0xff;

    pc = d->arg1 + (d->arg0 << 8);
}

// -------------------------------------------------------------------------
// LJMP
//
// Long jump
//
//    LJMP code addr
//
void LJMP (pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    pc = d->arg1 + (d->arg0 << 8);
}

// -------------------------------------------------------------------------
// MOV
//
// Move memory
//
//    MOV @R0, #data
//    MOV @R1, #data
//    MOV @R0, A
//    MOV @R1, A
//    MOV @R0, iram addr
//    MOV @R1, iram addr
//    MOV A, #data
//    MOV A, @R0
//    MOV A, @R1
//    MOV A, Rx                         (where x is 0 to 7)
//    MOV A, iram addr
//    MOV C, bit addr
//    MOV DPTR, #data16
//    MOV Rx, #data                     (where x is 0 to 7)
//    MOV Rx, A                         (where x is 0 to 7)
//    MOV Rx, iram data                 (where x is 0 to 7)
//    MOV bit addr, C
//    MOV iram addr, #data
//    MOV iram addr, @R0
//    MOV iram addr, @R1
//    MOV iram addr, Rx                 (where x is 0 to 7)
//    MOV iram addr, A
//    MOV iram addr, iram addr
//
void MOV (pDecode_t d) {

    int op2;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;


    // The 3 byte direct destination MOV instructions are awkward, as arg0
    // is the destination when source is IMM, but arg1 is the destination
    // when source is DIR.
    if (d->decode->addr_mode_op1 == DIR && d->decode->instr_size == 3) {
        // If source is direct, then fetch from arg0, and then move arg1 to arg0
        // ready for writing as per other MOV instructions
        if (d->decode->addr_mode_op2 == DIR) {
            fetch_arg(d->decode->addr_mode_op2, &op2, d->arg0, d->arg1);
            d->arg0 = d->arg1;
        // If source is IMM (in arg1), then temporarily move arg1 to
        // arg0 to make ftech look the same as all other MOV instructions
        } else 
            fetch_arg(d->decode->addr_mode_op2, &op2, d->arg1, d->arg0);
        
    } else 
        fetch_arg(d->decode->addr_mode_op2, &op2, d->arg0, d->arg1);

    write_arg(d->decode->addr_mode_op1, op2, d->arg0);
    
    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// MOVC
//
// Move code byte to accumulator
//
//    MOVC A, @A+DPTR
//    MOVC A, @A+PC
//
void MOVC(pDecode_t d) {
    int op2; 

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    pc += d->decode->instr_size;

    fetch_arg(d->decode->addr_mode_op2, &op2, d->arg0, d->arg1);

    acc = code_mem[acc+op2];
    
    SET_PSW_P(psw, odd_parity8(acc));
    
}

// -------------------------------------------------------------------------
// MOVX
//
// Move data to/from external memory
//
//    MOVX @DPTR, A
//    MOVX @R0, A
//    MOVX @R1, A
//    MOVX A, @DPTR
//    MOVX A, @R0
//    MOVX A, @R1
//
void MOVX(pDecode_t d) {
    int op2; 
    // Update the cycle count
    cycle_count += d->decode->clk_cycles;


    fetch_arg(d->decode->addr_mode_op2, &op2, d->arg0, d->arg1);
    write_arg(d->decode->addr_mode_op1, op2, d->arg0);

    pc += d->decode->instr_size;
    
}

// -------------------------------------------------------------------------
// MUL
//
// Multiply accumulator by register B
//
//    MUL AB
//
void MUL(pDecode_t d) {

    int op1, op2;
    int result;
    
    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);
    fetch_arg(d->decode->addr_mode_op2, &op2, d->arg0, d->arg1);

    result = op1 * op2;

    write_arg(d->decode->addr_mode_op1, result & 0xff, d->arg0);
    write_arg(d->decode->addr_mode_op2, (result >> 8) & 0xff, d->arg0);

    // Clear carry
    SET_PSW_CY(psw, 0);

    // Calculate overflow flag
    SET_PSW_OV(psw, result > 255 ? 1 : 0);
    
    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// NOP
//
// No operation (single cycle time elapse)
//
void NOP(pDecode_t d) {
    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// ORL
//
// Bitwise OR
//
//    ORL iram addr, A
//    ORL iram addr, #data
//    ORL A, #data
//    ORL A, iram addr
//    ORL A, @R0
//    ORL A, @R1
//    ORL A, Rx                         (where x is 0 to 7)
//    ORL C, bit addr
//    ORL C, /bit addr
//
void ORL(pDecode_t d) {

    int op2, op1;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);

    if (d->decode->instr_size != 3)
        fetch_arg(d->decode->addr_mode_op2, &op2, d->arg0, d->arg1);
    else 
        fetch_arg(d->decode->addr_mode_op2, &op2, d->arg1, d->arg0);

    op1 |= op2;

    write_arg(d->decode->addr_mode_op1, op1, d->arg0);

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// POP
//
// Pop a byte from the stack
//
//    POP iram addr
//
void POP(pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    int_ram[d->arg0] = int_ram[sp--];

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// PUSH
//
// Push a byte onto the stack
//
//    PUSH iram addr
//
void PUSH(pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    int_ram[++sp] = int_ram[d->arg0];

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// RET
//
// Return from subroutine
//
//    RET
//
void RET(pDecode_t d) {
    int addr;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    addr  = int_ram[sp--] << 8;
    addr |= int_ram[sp--];

    pc = addr;
}

// -------------------------------------------------------------------------
// RETI
//
// Return from interrupt
//
//    RETI
//
void RETI(pDecode_t d) {
    int addr;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    addr  = int_ram[sp--] << 8;
    addr |= int_ram[sp--];

    pc = addr;

    // Restore interrupt priorities 
    int_level = last_int_level;

    // The level before last must be 'no interrupt'
    last_int_level = INT_LVL_OFF;
}

// -------------------------------------------------------------------------
// RL
//
// Rotate accumulator left
//
//    RL A
//
void RL(pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    acc = ((acc & 0x7f) << 1) | ((acc & 0x80) ? 1 : 0);

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// RLC
//
// Rotate accumulator left through carry
//
//    RLC A
//
void RLC(pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    acc = ((acc & 0xff) << 1) | GET_PSW_CY(psw);
    SET_PSW_CY(psw, (acc & 0x100) ? 1 : 0);
    SET_PSW_P(psw, odd_parity8(acc));
    acc &= 0xff;
    

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// RR
//
// Rotate accumulator right
//
//    RR A
//
void RR(pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    acc = (acc >> 1) | ((acc & 1) << 7);
    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// RRC
//
// Rotate accumulator right through carry
//
//    RRC A
//
void RRC(pDecode_t d) {
    int tmp;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    tmp = acc & 1;
    acc = (acc >> 1) | (GET_PSW_CY(psw) << 7);
    SET_PSW_CY(psw, tmp);
    SET_PSW_P(psw, odd_parity8(acc));
    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// SETB
//
// Set bit
//
//    SETB C
//    SETB bit addr
//
void SETB(pDecode_t d) {

    int offset, bit;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    if (d->decode->addr_mode_op1 != BIT) {
        write_arg(d->decode->addr_mode_op1, 1, d->arg0);
    } else {
        if (d->arg0  < SFR_START) {
            offset = BIT_RAM_START + ((d->arg0 >> 3) & 0xf);
            bit    = d->arg0 & 0x7;

            int_ram[offset] = int_ram[offset] | (1 << bit);
        } else {
            offset = d->arg0 & 0xf8;
            bit    = d->arg0 & 0x7;

            set_sfr(offset, fetch_sfr(offset) | (1 << bit));
        }
    }

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// SJMP
//
// Short jump
//
//    SJMP reladdr
//
void SJMP(pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    pc += d->decode->instr_size;

    pc += (d->arg0 & 0x80) ? -1 * ((d->arg0 ^ 0xff) + 1) : (d->arg0 & 0x7f);
}

// -------------------------------------------------------------------------
// SUBB
//
//Subtract from accumulator with borrow
//
//    SUBB A, #data
//    SUBB A, iram addr
//    SUBB A, @R0
//    SUBB A, @R1
//    SUBB A, Rx                                (where x is 0 to 7)
//
void SUBB(pDecode_t d) {
    int op1, old_acc; 

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op2, &op1, d->arg0, d->arg1);

    old_acc = acc;

    acc -= GET_PSW_CY(psw) + op1;

    SET_PSW_CY(psw, (op1 > old_acc) ? 1 : 0);
    SET_PSW_AC(psw, ((op1 & 0xf) > (old_acc & 0xf)) ? 1 : 0);
    SET_PSW_OV(psw, ((old_acc ^ acc) & 0x80));
    SET_PSW_P(psw, odd_parity8(acc));

    acc &= 0xff;

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// SWAP
//
// Swap accumulator nibbles
//
//    SWAP A
//
void SWAP(pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    acc = (acc >> 4) | ((acc & 0xf) << 4);
    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// XCH
//
// Exchange bytes
//
//    XCH A, @R0
//    XCH A, @R1
//    XCH A, Rx                                 (where x is 0 to 7)
//    XCH A, iram addr
//
void XCH(pDecode_t d) {
    int op2, op1;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);
    fetch_arg(d->decode->addr_mode_op2, &op2, d->arg0, d->arg1);

    write_arg(d->decode->addr_mode_op1, op2, d->arg0);
    write_arg(d->decode->addr_mode_op2, op1, d->arg0);

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// XCHD
//
// Exchange digit
//
//    XCHD A, @R0
//    XCHD A, @R1
//
void XCHD(pDecode_t d) {
    int op2, op1;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);
    fetch_arg(d->decode->addr_mode_op2, &op2, d->arg0, d->arg1);

    write_arg(d->decode->addr_mode_op1, (op1 & 0xf0) | (op2 & 0x0f), d->arg0);
    write_arg(d->decode->addr_mode_op2, (op2 & 0xf0) | (op1 & 0x0f), d->arg0);

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// XRL
//
// Bitwise exclusive OR
//
//    XRL iram addr, A
//    XRL iram addr, #data
//    XRL A, #data
//    XRL A, iram addr
//    XRL A, @R0
//    XRL A, @R1
//    XRL A, Rx                                 (where x is 0 to 7)
//
void XRL(pDecode_t d) {

    int op2, op1;

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fetch_arg(d->decode->addr_mode_op1, &op1, d->arg0, d->arg1);

    if (d->decode->instr_size != 3) 
        fetch_arg(d->decode->addr_mode_op2, &op2, d->arg0, d->arg1);
    else 
        fetch_arg(d->decode->addr_mode_op2, &op2, d->arg1, d->arg0);

    op1 ^= op2;

    write_arg(d->decode->addr_mode_op1, op1, d->arg0);

    pc += d->decode->instr_size;
}

// -------------------------------------------------------------------------
// UNIMP
//
// Unimplemented instruction. Can be used asa break point.
//
void UNIMP(pDecode_t d) {

    // Update the cycle count
    cycle_count += d->decode->clk_cycles;

    fprintf(stderr, "WARNING: executed unimplemented instruction (0x%02x). Executed NOP\n", d->opcode);

    pc += d->decode->instr_size;

    break_point = UNIMP_BREAK;
}
