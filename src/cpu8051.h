//=============================================================
// 
// Copyright (c) 2012 Simon Southwell. All rights reserved.
//
// Date: 6th May 2013
//
// This file is part of the cpu8051 instruction set simulator.
//
// This is the external header for the ISS, used when linking
// the library libcpu8051.a. It defines the user API.
//
// $Id: cpu8051.h,v 1.1 2013-06-25 18:40:48 simon Exp $
// $Source: /home/simon/CVS/src/cpu/8051/src/cpu8051.h,v $
//
//=============================================================

#include <stdint.h>

#ifndef _CPU8051_H_
#define _CPU8051_H_

#define FOREVER                 0
#define ONCE                    1

#define NO_ERROR                0

#define INT_CALLBACK_EXT0       0
#define INT_CALLBACK_EXT1       1

#define MEM_CB_TIME_UPDATE      -1

#define VERBOSITY_LVL_OFF       0
#define VERBOSITY_LVL_1         1

// Type definitions for external interrupt callback functions ( "int mycallback(int cycle_time, int* wakeup_time)" )
// User function returns 1 if an interrupt is to be generated, otherwise 0. The current cycle time is passed
// in, and a pointer to an int to return a wakeup time. I.e. the callback will not be invoked until after the
// instruction that has the cycle_time greater than, or equal to the wakeup_time. If the value is left unchanged
// or set to -1, the callback will be called after each instruction (assuming interrupt enabled and of sufficient 
// priority).
typedef int  (*pintcallback_t) (int time, int *wakeup_time);

// Type defintion for a general purpose time callback. Same as for interrupt, but does not generate
// an interrupt, but is for those user routine requiring a scheduled time update from once per instruction
// to some arbitrary cycle in the future
typedef int  (*ptimecallback_t) (int time, int *wakeup_time);

// Call back type for external memory. User routine is expected to intercept any address
// corresponding to hardware it's modelling, but pass on any other accesses to the
// buffer pointed to by mem[].
typedef int  (*pmemcallback_t) (int addr, int data, int rnw, uint8_t *mem, int time);

// Function prototypes of user accessible routines...

// Callback registering function for external interrupts
extern int  register_int_callback  (pintcallback_t callback_func, int callback_type);

// Callback registering function for regular time update
extern int register_time_callback (ptimecallback_t callback_func);

// Callback registering function for external memory
extern int register_ext_mem_callback (pmemcallback_t callback_func);

// Callback registering function for SFRs. This is *only* called if an SFR access
// is not one of the standard 8051 SFRs, which are modelled internally
extern int register_sfr_callback (pmemcallback_t callback_func);

// Configuration routines 
extern void set_verbosity_lvl      (int lvl);
extern void set_output_stream      (FILE* file_pointer);
extern void set_disable_lock_break ();
extern void clr_disable_lock_break ();

// User access routines...

// Return current cycle count
extern int  get_cycle_time         (void);

// Access internal RAM (including SFRs)
extern int  get_iram_byte          (int addr); 
extern void set_iram_byte          (int addr, int data);

// Access external RAM. If an external RAM callback functions is registered, these
// functions will invoke that callback.
extern int  get_ext_ram_byte       (int addr);
extern void set_ext_ram_byte       (int addr, int data);

// Main execution function
extern int  run_program     (char* ihx_fname, int run_cycles, int break_addr, int timer_enable);

#endif
