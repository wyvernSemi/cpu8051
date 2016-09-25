//=============================================================
// 
// Copyright (c) 2012 Simon Southwell. All rights reserved.
//
// Date: 25th August 2012  
//
// This file is part of the cpu8051 instruction set simulator.
//
// This file contains a command line front end to the ISS. It
// can be replaced with user code, which can link to libcpu8051.a
// which does not contain this file. Its main purpose is to allow
// code to be loaded to the ISS for test.
//
// $Id: cpu8051.c,v 1.3 2016-09-25 13:56:47 simon Exp $
// $Source: /home/simon/CVS/src/cpu/8051/src/cpu8051.c,v $
//
//=============================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef WIN32
#ifdef CYGWIN
#include <GetOpt.h>
#else
#include <unistd.h>
#endif
#else 
extern int getopt(int nargc, char** nargv, char* ostr);
extern char* optarg;
#endif

#include "cpu8051.h"

static int  ext0_interrupt (int time, int *wakeup_time);
static int  ext0_fire_time = -1;

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

int main(int argc, char** argv) {

    int num_run_instr  = FOREVER;
    int breakpoint     = -1;
    int iram_dump_addr = -1;
    int timer_enable   = 0;

    char *fname = "try.ihx";
    int option;

    // Put the output on STDOUT
    set_output_stream(stdout);

    // Process the command line options 
    while ((option = getopt(argc, argv, "f:n:vb:dr:i:t:")) != EOF)
        switch(option) {
        case 'f':
            fname = optarg;
            break;
        case 'n':
            num_run_instr = strtol(optarg, NULL, 0);
            if (num_run_instr < FOREVER)
                num_run_instr = FOREVER;
            break;
        case 'i':
            ext0_fire_time = strtol(optarg, NULL, 0);
            break;
        case 't':
            timer_enable = strtol(optarg, NULL, 0);
            break;
        case 'b':
            breakpoint = (uint32_t)strtol(optarg, NULL, 0);
            break;
        case 'v':
            set_verbosity_lvl(VERBOSITY_LVL_1);
            break;
        case 'r':
            iram_dump_addr = (uint32_t)strtol(optarg, NULL, 0);
            break;
        case 'd':
            set_disable_lock_break();
            break;
        case 'h':
        case '?':
        default:
            fprintf(stderr, 
                    "Usage: %s [-v] [-d] [-n <num instructions>] [-r <ram addr>] [-i <int cycle>] [-t <timer enables>] [-b <breakpoint addr>] [-f <filename>]\n"
                    "\n"
                    "    -n Specify number of instructions to run (default: run until UNIMP)\n"
                    "    -i Cycle to fire EXT0 interrupt\n"
                    "    -b Specify address for breakpoint\n"
                    "    -f Specify executable IHX file (default try.ihx)\n"
                    "    -v Specify verbose output\n"
                    "    -d Disable breaking on lock condition\n"
		    "    -r IRAM dump address\n"
                    "    -t Activate timers (0 = off, 1 = TIM0, 2 = TIM1, 3 = both)"
                    "\n"
                    , argv[0]);
            exit(NO_ERROR);
            break;
        }

    if (ext0_fire_time != -1) {
        register_int_callback(ext0_interrupt, INT_CALLBACK_EXT0);
    }

    run_program(fname, num_run_instr, breakpoint, timer_enable);

    if (iram_dump_addr >= 0 && iram_dump_addr < 256) {
        fprintf(stdout, "IRAM 0x%04x = %02x\n", iram_dump_addr, get_iram_byte(iram_dump_addr));
    }
}

// EXT 0 interrupt callback function
static int ext0_interrupt (int time, int *wakeup_time) {

    static int initialised = 0;

    if (initialised == 0) {
        initialised = 1;
        *wakeup_time = ext0_fire_time;
        return 0;
    } else {
        *wakeup_time = time + ext0_fire_time;
        return 1;
    }
}
