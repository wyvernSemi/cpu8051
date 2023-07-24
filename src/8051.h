//=============================================================
// 
// Copyright (c) 2012 Simon Southwell. All rights reserved.
//
// Date: 25th August 2012  
//
// This file is part of the cpu8051 instruction set simulator.
//
// This file is the main internal header for the ISS.
//
// $Id: 8051.h,v 1.5 2020/04/18 06:09:19 simon Exp $
// $Source: /home/simon/CVS/src/cpu/8051/src/8051.h,v $
//
//=============================================================

#ifndef _8051_H_
#define _8051_H_

#include <stdint.h>

#include "cpu8051.h"

// Break point types
#define UNIMP_BREAK 1
#define USER_BREAK  2
#define LOCK_BREAK  3

// Size of memories
#define MAXCODEMEM 65536
#define MAXEXTRAM  65536
#define MAXINTRAM  256
#define MAXSFRNUM  128

// Offsets in IRAM 
#define BIT_RAM_START 0x20
#define SFR_START     0x80

// Addressing modes
#define NONE          0
#define IMM           1         
#define DIR           2         /* MOV  A, 30h     : direct from iram */
#define IND0          3         /* MOV  A, @R0     : Indirect from address in R0 */
#define IND1          4         /* MOV  A, @R1     : Indirect from address in R1 */
#define EXT           5         /* MOVX A, @DPTR   : External memory pointed to by DPTR */
#define EXT0          6         /* MOVX A, @R0     : External memory pointed to by R0 */
#define EXT1          7         /* MOVX A, @R1     : External memory pointed to by R1 */
#define CODI          8         /* MOVC A, @A+DPTR : Code indirect */

// Register addressing modes
#define ACC           9         /* Accumulator */
#define REG0          10        /* Register 0  */
#define REG1          11        /* Register 1  */
#define REG2          12        /* Register 2  */
#define REG3          13        /* Register 3  */
#define REG4          14        /* Register 4  */
#define REG5          15        /* Register 5  */
#define REG6          16        /* Register 6  */
#define REG7          17        /* Register 7  */
#define DPTR          18        /* Data pointer */
#define REL           19        /* Relative */
#define CODE          20        /* Code address */
#define CC            21        /* Carry flag */
#define BIT           22        /* Bit address */
#define NBIT          23        /* Bit address */
#define BREG          24        /* register B */
#define IMM16         25        /* Immediate, 16 bits */
#define PC            26        /* Program counter */

// PSW bit definitions
#define PSW_CY        7
#define PSW_AC        6
#define PSW_F0        5
#define PSW_RS1       4
#define PSW_RS0       3
#define PSW_OV        2
#define PSW_UD        1
#define PSW_P         0


// Bit twiddling of the program status word
#define SET_PSW_P(_psw_, _arg_)  { _psw_ = ((_psw_) & ~(1 << PSW_P))  | ((_arg_) ? (1 << PSW_P)  : 0); }
#define SET_PSW_CY(_psw_, _arg_) { _psw_ = ((_psw_) & ~(1 << PSW_CY)) | ((_arg_) ? (1 << PSW_CY) : 0); }
#define SET_PSW_AC(_psw_, _arg_) { _psw_ = ((_psw_) & ~(1 << PSW_AC)) | ((_arg_) ? (1 << PSW_AC) : 0); }
#define SET_PSW_OV(_psw_, _arg_) { _psw_ = ((_psw_) & ~(1 << PSW_OV)) | ((_arg_) ? (1 << PSW_OV) : 0); }
#define SET_PSW_RS(_psw_, _arg_) { \
                                  _psw_ = ((_psw_) & ~((1 << PSW_RS1) | (1 << PSW_RS0))) | (((_arg_) & 0x3) << PSW_RS0); \
                                   r = &int_ram[(((_arg_) & 0x3) << 3)];                                                 \
                                  }

#define GET_PSW_CY(_psw_)  (((_psw_) & (1 << PSW_CY)) ? 1 : 0)
#define GET_PSW_AC(_psw_)  (((_psw_) & (1 << PSW_AC)) ? 1 : 0)
#define GET_PSW_OV(_psw_)  (((_psw_) & (1 << PSW_OV)) ? 1 : 0)
#define GET_PSW_RS(_psw_)  ((((_psw_) & ((1 << PSW_RS1) | (1 << PSW_RS0))) >> PSW_RS0) & 0x3)
#define GET_PSW_P(_psw_)   (((_psw_) & (1 << PSW_P)) ? 1 : 0))

// Standard SFR addresses
#define SFR_P0        0x80
#define SFR_SP        0x81
#define SFR_DPL       0x82
#define SFR_DPH       0x83
#define SFR_PCON      0x87
#define SFR_TCON      0x88
#define SFR_TMOD      0x89
#define SFR_TL0       0x8A
#define SFR_TL1       0x8B
#define SFR_TH0       0x8C
#define SFR_TH1       0x8D
#define SFR_P1        0x90
#define SFR_SCON      0x98
#define SFR_SBUF      0x99
#define SFR_P2        0xA0
#define SFR_IE        0xA8
#define SFR_P3        0xB0
#define SFR_IP        0xB8
#define SFR_PSW       0xD0
#define SFR_ACC       0xE0
#define SFR_B         0xF0

// Interrupt level definitions
#define INT_LVL_OFF   0
#define INT_LVL_LO    1
#define INT_LVL_HI    2

// Interrupt priority bits in IP SFR register
#define IP_PX0_BIT    0
#define IP_PT0_BIT    1
#define IP_PX1_BIT    2
#define IP_PT1_BIT    3
#define IP_PS_BIT     4
#define IP_EA_BIT     7

#define IP_PX0_MSK    0x01
#define IP_PT0_MSK    0x02
#define IP_PX1_MSK    0x04
#define IP_PT1_MSK    0x08
#define IP_PS_MSK     0x10
#define IP_EA_MSK     0x80

#define TCON_TF1_MSK  0x80
#define TCON_TR1_MSK  0x40
#define TCON_TF0_MSK  0x20
#define TCON_TR0_MSK  0x10
#define TCON_IE1_MSK  0x08
#define TCON_IT1_MSK  0x04
#define TCON_IE0_MSK  0x02
#define TCON_IT0_MSK  0x01

// Interrupt priority bit addresses
#define IP_PX0_BIT_ADDR         0xB8
#define IP_PT0_BIT_ADDR         0xB9
#define IP_PX1_BIT_ADDR         0xBA
#define IP_PT1_BIT_ADDR         0xBB
#define IP_PS_BIT_ADDR          0xBC

#define EXT0_INT_VECTOR         0x0003
#define TIM0_INT_VECTOR         0x000B
#define EXT1_INT_VECTOR         0x0013
#define TIM1_INT_VECTOR         0x001B
#define SER_INT_VECTOR          0x0023

// Timer mode definitions
#define TIMER_13BIT             0
#define TIMER_16BIT             1
#define TIMER_8BIT_AUTO         2
#define TIMER_SPLIT             3

// Memory access callback read/write definitions
#define MEM_CB_READ             1
#define MEM_CB_WRITE            0

// Remap some of the 
#define ADDC                    ADD
 
#define DECODE_TABLE_SIZE       256

#define DECODE_TABLE { \
\
/* 0x00 */ {NOP,  "NOP                    ", 1, 1, NONE, NONE},  {AJMP,  "AJMP  page0            ", 2, 3, NONE, NONE}, {LJMP,  "LJMP  caddr            ", 3, 4, NONE, NONE}, {RR,    "RR    A                ", 1, 1, ACC,  NONE}, \
/* 0x04 */ {INC,  "INC   A                ", 1, 1, ACC,  NONE},  {INC,   "INC   iaddr            ", 2, 2, DIR,  NONE}, {INC,   "INC   @R0              ", 1, 2, IND0, NONE}, {INC,   "INC   @R1              ", 1, 2, IND1, NONE}, \
/* 0x08 */ {INC,  "INC   R0               ", 1, 1, REG0, NONE},  {INC,   "INC   R1               ", 1, 1, REG1, NONE}, {INC,   "INC   R2               ", 1, 1, REG2, NONE}, {INC,   "INC   R3               ", 1, 1, REG3, NONE}, \
/* 0x0C */ {INC,  "INC   R4               ", 1, 1, REG4, NONE},  {INC,   "INC   R5               ", 1, 1, REG5, NONE}, {INC,   "INC   R6               ", 1, 1, REG6, NONE}, {INC,   "INC   R7               ", 1, 1, REG7, NONE}, \
 \
/* 0x10 */ {JBC,  "JBC   baddr, raddr     ", 3, 3, BIT,  REL},   {ACALL, "ACALL page0            ", 2, 3, NONE, NONE}, {LCALL, "LCALL caddr            ", 3, 4, CODE, NONE}, {RRC,   "RRC   A                ", 1, 1, ACC,  NONE}, \
/* 0x14 */ {DEC,  "DEC   A                ", 1, 1, ACC,  NONE},  {DEC,   "DEC   iaddr            ", 2, 2, DIR,  NONE}, {DEC,   "DEC   @R0              ", 1, 2, IND0, NONE}, {DEC,   "DEC   @R1              ", 1, 2, IND1, NONE}, \
/* 0x18 */ {DEC,  "DEC   R0               ", 1, 1, REG0, NONE},  {DEC,   "DEC   R1               ", 1, 1, REG1, NONE}, {DEC,   "DEC   R2               ", 1, 1, REG2, NONE}, {DEC,   "DEC   R3               ", 1, 1, REG3, NONE}, \
/* 0x1C */ {DEC,  "DEC   R4               ", 1, 1, REG4, NONE},  {DEC,   "DEC   R5               ", 1, 1, REG5, NONE}, {DEC,   "DEC   R6               ", 1, 1, REG6, NONE}, {DEC,   "DEC   R7               ", 1, 1, REG7, NONE}, \
 \
/* 0x20 */ {JB,   "JB    baddr, raddr     ", 3, 3, BIT,  REL},   {AJMP,  "AJMP  page1            ", 2, 3, NONE, NONE}, {RET,   "RET                    ", 1, 5, NONE, NONE},  {RL,   "RL    A                ", 1, 1, NONE, NONE}, \
/* 0x24 */ {ADD,  "ADD   A, #data         ", 2, 2, ACC,  IMM},   {ADD,   "ADD   iaddr            ", 2, 2, ACC,  DIR},  {ADD,   "ADD   @R0              ", 1, 2, ACC,  IND0},  {ADD,  "ADD   @R1              ", 1, 2, ACC,  IND1}, \
/* 0x28 */ {ADD,  "ADD   R0               ", 1, 1, ACC,  REG0},  {ADD,   "ADD   R1               ", 1, 1, ACC,  REG1}, {ADD,   "ADD   R2               ", 1, 1, ACC,  REG2},  {ADD,  "ADD   R3               ", 1, 1, ACC,  REG3}, \
/* 0x2C */ {ADD,  "ADD   R4               ", 1, 1, ACC,  REG4},  {ADD,   "ADD   R5               ", 1, 1, ACC,  REG5}, {ADD,   "ADD   R6               ", 1, 1, ACC,  REG6},  {ADD,  "ADD   R7               ", 1, 1, ACC,  REG7}, \
 \
/* 0x30 */ {JNB,  "JNB   baddr, raddr     ", 3, 3, BIT,  REL},   {ACALL, "ACALL page1            ", 2, 3, NONE, NONE}, {RETI,  "RETI                   ", 1, 5, NONE, NONE},  {RLC,  "RLC   A                ", 1, 1, NONE, NONE}, \
/* 0x34 */ {ADDC, "ADDC  A, #data         ", 2, 2, ACC,  IMM},   {ADDC,  "ADDC  iaddr            ", 2, 2, ACC,  DIR},  {ADDC,  "ADDC  @R0              ", 1, 2, ACC,  IND0},  {ADDC, "ADDC  @R1              ", 1, 2, ACC,  IND1}, \
/* 0x38 */ {ADDC, "ADDC  R0               ", 1, 1, ACC,  REG0},  {ADDC,  "ADDC  R1               ", 1, 1, ACC,  REG1}, {ADDC,  "ADDC  R2               ", 1, 1, ACC,  REG2},  {ADDC, "ADDC  R3               ", 1, 1, ACC,  REG3}, \
/* 0x3C */ {ADDC, "ADDC  R4               ", 1, 1, ACC,  REG4},  {ADDC,  "ADDC  R5               ", 1, 1, ACC,  REG5}, {ADDC,  "ADDC  R6               ", 1, 1, ACC,  REG6},  {ADDC, "ADDC  R7               ", 1, 1, ACC,  REG7}, \
 \
/* 0x40 */ {JC,   "JC    raddr            ", 2, 2, CC,   REL},   {AJMP,  "AJMP  page2            ", 2, 3, NONE, NONE}, {ORL,   "ORL   iaddr, A         ", 2, 2, DIR,  ACC},   {ORL,  "ORL   iaddr, #data     ", 3, 3, DIR,  IMM},  \
/* 0x44 */ {ORL,  "ORL   A, #data         ", 2, 2, ACC,  IMM},   {ORL,   "ORL   A, iaddr         ", 2, 2, ACC,  DIR},  {ORL,   "ORL   A, @R0           ", 1, 2, ACC,  IND0},  {ORL,  "ORL   A, @R1           ", 1, 2, ACC,  IND1}, \
/* 0x48 */ {ORL,  "ORL   R0               ", 1, 1, ACC,  REG0},  {ORL,   "ORL   R1               ", 1, 1, ACC,  REG1}, {ORL,   "ORL   R2               ", 1, 1, ACC,  REG2},  {ORL,  "ORL   R3               ", 1, 1, ACC,  REG3}, \
/* 0x4C */ {ORL,  "ORL   R4               ", 1, 1, ACC,  REG4},  {ORL,   "ORL   R5               ", 1, 1, ACC,  REG5}, {ORL,   "ORL   R6               ", 1, 1, ACC,  REG6},  {ORL,  "ORL   R7               ", 1, 1, ACC,  REG7}, \
 \
/* 0x50 */ {JNC,  "JNC   raddr            ", 2, 2, CC,   REL},   {ACALL, "ACALL page2            ", 2, 3, NONE, NONE}, {ANL,   "ANL   iaddr, A         ", 2, 2, DIR,  ACC},   {ANL,  "ANL   iaddr, #data     ", 3, 3, DIR,  IMM},  \
/* 0x54 */ {ANL,  "ANL   A, #data         ", 2, 2, ACC,  IMM},   {ANL,   "ANL   A, iaddr         ", 2, 2, ACC,  DIR},  {ANL,   "ANL   A, @R0           ", 1, 2, ACC,  IND0},  {ANL,  "ANL   A, @R1           ", 1, 2, ACC,  IND1}, \
/* 0x58 */ {ANL,  "ANL   R0               ", 1, 1, ACC,  REG0},  {ANL,   "ANL   R1               ", 1, 1, ACC,  REG1}, {ANL,   "ANL   R2               ", 1, 1, ACC,  REG2},  {ANL,  "ANL   R3               ", 1, 1, ACC,  REG3}, \
/* 0x5C */ {ANL,  "ANL   R4               ", 1, 1, ACC,  REG4},  {ANL,   "ANL   R5               ", 1, 1, ACC,  REG5}, {ANL,   "ANL   R6               ", 1, 1, ACC,  REG6},  {ANL,  "ANL   R7               ", 1, 1, ACC,  REG7}, \
 \
/* 0x60 */ {JZ,   "JZ    raddr            ", 2, 2, ACC,  REL},   {AJMP,  "AJMP  page3            ", 2, 3, NONE, NONE}, {XRL,   "XRL   iaddr, A         ", 2, 2, DIR , ACC},   {XRL,  "XRL   iaddr, #data     ", 3, 3, DIR,  IMM},  \
/* 0x64 */ {XRL,  "XRL   A, #data         ", 2, 2, ACC,  IMM},   {XRL,   "XRL   A, iaddr         ", 2, 2, ACC,  DIR},  {XRL,   "XRL   A, @R0           ", 1, 2, ACC,  IND0},  {XRL,  "XRL   A, @R1           ", 1, 2, ACC,  IND1}, \
/* 0x68 */ {XRL,  "XRL   R0               ", 1, 1, ACC,  REG0},  {XRL,   "XRL   R1               ", 1, 1, ACC,  REG1}, {XRL,   "XRL   R2               ", 1, 1, ACC,  REG2},  {XRL,  "XRL   R3               ", 1, 1, ACC,  REG3}, \
/* 0x6C */ {XRL,  "XRL   R1               ", 1, 1, ACC,  REG4},  {XRL,   "XRL   R5               ", 1, 1, ACC,  REG5}, {XRL,   "XRL   R6               ", 1, 1, ACC,  REG6},  {XRL,  "XRL   R7               ", 1, 1, ACC,  REG7}, \
 \
/* 0x70 */ {JNZ,  "JNZ   raddr            ", 2, 2, ACC,  REL},   {ACALL, "ACALL page3            ", 2, 3, NONE, NONE}, {ORL,   "ORL   C, baddr         ", 2, 2, CC,   BIT},   {JMP,  "JMP   @A+DPTR          ", 1, 3, NONE, NONE}, \
/* 0x74 */ {MOV,  "MOV   A, #data         ", 2, 2, ACC,  IMM},   {MOV,   "MOV   iaddr, #data     ", 3, 3, DIR,  IMM},  {MOV,   "MOV   @R0, #data       ", 2, 2, IND0, IMM},   {MOV,  "MOV   @R1, #data       ", 2, 2, IND1, IMM},  \
/* 0x78 */ {MOV,  "MOV   R0, #data        ", 2, 2, REG0, IMM},   {MOV,   "MOV   R1, #data        ", 2, 2, REG1, IMM},  {MOV,   "MOV   R2, #data        ", 2, 2, REG2, IMM},   {MOV,  "MOV   R3, #data        ", 2, 2, REG3, IMM},  \
/* 0x7C */ {MOV,  "MOV   R4, #data        ", 2, 2, REG4, IMM},   {MOV,   "MOV   R5, #data        ", 2, 2, REG5, IMM},  {MOV,   "MOV   R6, #data        ", 2, 2, REG6, IMM},   {MOV,  "MOV   R7, #data        ", 2, 2, REG7, IMM},  \
 \
/* 0x80 */ {SJMP, "SJMP  raddr            ", 2, 3, REL,  NONE},  {AJMP,  "AJMP  page4            ", 2, 3, NONE, NONE}, {ANL,   "ANL   C, baddr         ", 2, 2, CC,   BIT},   {MOVC, "MOVC  A,@A+PC          ", 1, 3, ACC,  PC},   \
/* 0x84 */ {DIV,  "DIV   AB               ", 1, 8, ACC,  BREG},  {MOV,   "MOV   iaddr, iaddr     ", 3, 3, DIR,  DIR},  {MOV,   "MOV   iaddr, @R0       ", 2, 2, DIR,  IND0},  {MOV,  "MOV   iaddr, @R1       ", 2, 2, DIR,  IND1}, \
/* 0x88 */ {MOV,  "MOV   iaddr, R0        ", 2, 2, DIR,  REG0},  {MOV,   "MOV   iaddr, R1        ", 2, 2, DIR,  REG1}, {MOV,   "MOV   iaddr, R2        ", 2, 2, DIR,  REG2},  {MOV,  "MOV   iaddr, R0        ", 2, 2, DIR,  REG3}, \
/* 0x8C */ {MOV,  "MOV   iaddr, R4        ", 2, 2, DIR,  REG4},  {MOV,   "MOV   iaddr, R5        ", 2, 2, DIR,  REG5}, {MOV,   "MOV   iaddr, R6        ", 2, 2, DIR,  REG6},  {MOV,  "MOV   iaddr, R7        ", 2, 2, DIR,  REG7}, \
 \
/* 0x90 */ {MOV,  "MOV   DPTR, #data16    ", 3, 3, DPTR, IMM16}, {ACALL, "ACALL page4            ", 2, 3, NONE, NONE}, {MOV,   "MOV   baddr, C         ", 2, 2, BIT,  CC},    {MOVC, "MOVC  A,@A+DPTR        ", 1, 3, ACC,  DPTR}, \
/* 0x94 */ {SUBB, "SUBB  A, #data         ", 2, 2, ACC,  IMM},   {SUBB,  "SUBB  A, iaddr         ", 2, 2, ACC,  DIR} , {SUBB,  "SUBB  A, @R0           ", 1, 2, ACC,  IND0},  {SUBB, "SUBB  A, @R1           ", 1, 2, ACC,  IND1}, \
/* 0x98 */ {SUBB, "SUBB  R0               ", 1, 1, ACC,  REG0},  {SUBB,  "SUBB  R1               ", 1, 1, ACC,  REG1}, {SUBB,  "SUBB  R2               ", 1, 1, ACC,  REG2},  {SUBB, "SUBB  R3               ", 1, 1, ACC,  REG3}, \
/* 0x9C */ {SUBB, "SUBB  R4               ", 1, 1, ACC,  REG4},  {SUBB,  "SUBB  R5               ", 1, 1, ACC,  REG5}, {SUBB,  "SUBB  R6               ", 1, 1, ACC,  REG6},  {SUBB, "SUBB  R7               ", 1, 1, ACC,  REG7}, \
 \
/* 0xA0 */ {ORL,  "ORL   C, /baddr        ", 2, 2, CC,   NBIT},  {AJMP,  "AJMP  page5            ", 2, 3, NONE, NONE}, {MOV,   "MOV   C, baddr         ", 2, 2, CC,   BIT},   {INC,  "INC   DPTR             ", 1, 1, DPTR, NONE}, \
/* 0xA4 */ {MUL,  "MUL   AB               ", 1, 4, ACC,  BREG},  {UNIMP, "UNIMP                  ", 1, 1, NONE, NONE}, {MOV,   "MOV   @R0, iaddr       ", 2, 2, IND0, DIR} ,  {MOV,  "MOV   @R1, iaddr       ", 2, 2, IND1, DIR},  \
/* 0xA8 */ {MOV,  "MOV   R0, iaddr        ", 2, 2, REG0, DIR},   {MOV,   "MOV   R1, iaddr        ", 2, 2, REG1, DIR},  {MOV,   "MOV   R2, iaddr        ", 2, 2, REG2, DIR},   {MOV,  "MOV   R3, iaddr        ", 2, 2, REG3, DIR},  \
/* 0xAC */ {MOV,  "MOV   R4, iaddr        ", 2, 2, REG4, DIR},   {MOV,   "MOV   R5, iaddr        ", 2, 2, REG5, DIR},  {MOV,   "MOV   R6, iaddr        ", 2, 2, REG6, DIR},   {MOV,  "MOV   R7, iaddr        ", 2, 2, REG7, DIR},  \
 \
/* 0xB0 */ {ANL,  "ANL   C, /baddr        ", 2, 2, CC,   NBIT},  {ACALL, "ACALL page5            ", 2, 3, NONE, NONE}, {CPL,   "CPL   baddr            ", 2, 2, BIT,  NONE},  {CPL,  "CPL   C                ", 1, 1, CC,   NONE}, \
/* 0xB4 */ {CJNE, "CJNE  A, #data, raddr  ", 3, 3, ACC,  IMM},   {CJNE,  "CJNE  A, iaddr, raddr  ", 3, 3, ACC,  DIR},  {CJNE,  "CJNE  @R0, #data, raddr", 3, 4, IND0, IMM},   {CJNE, "CJNE  @R1, #data, raddr", 3, 4, IND1, IMM},  \
/* 0xB8 */ {CJNE, "CJNE  R0, #data, raddr ", 3, 3, REG0, IMM},   {CJNE,  "CJNE  R1, #data, raddr ", 3, 3, REG1, IMM},  {CJNE,  "CJNE  R2, #data, raddr ", 3, 3, REG2, IMM},   {CJNE, "CJNE  R3, #data, raddr ", 3, 3, REG3, IMM},  \
/* 0xBC */ {CJNE, "CJNE  R4, #data, raddr ", 3, 3, REG4, IMM},   {CJNE,  "CJNE  R5, #data, raddr ", 3, 3, REG5, IMM},  {CJNE,  "CJNE  R6, #data, raddr ", 3, 3, REG6, IMM},   {CJNE, "CJNE  R7, #data, raddr ", 3, 3, REG7, IMM},  \
 \
/* 0xC0 */ {PUSH, "PUSH  iaddr            ", 2, 2, DIR,  NONE},  {AJMP,  "AJMP  page6            ", 2, 3, NONE, NONE}, {CLR,   "CLR   baddr            ", 2, 2, BIT,  NONE},  {CLR,  "CLR   C                ", 1, 1, CC,   NONE}, \
/* 0xC4 */ {SWAP, "SWAP  A                ", 1, 1, NONE, NONE},  {XCH,   "XCH   A, iaddr         ", 2, 2, ACC,  DIR},  {XCH,   "XCH   A, @R0           ", 1, 2, ACC,  IND0},  {XCH,  "XCH   A, @R1           ", 1, 2, ACC,  IND1}, \
/* 0xC8 */ {XCH,  "XCH   A, R0            ", 1, 1, ACC,  REG0},  {XCH,   "XCH   A, R1            ", 1, 1, ACC,  REG1}, {XCH,   "XCH   A, R2            ", 1, 1, ACC,  REG2},  {XCH,  "XCH   A, R3            ", 1, 1, ACC,  REG3}, \
/* 0xCC */ {XCH,  "XCH   A, R4            ", 1, 1, ACC,  REG4},  {XCH,   "XCH   A, R5            ", 1, 1, ACC,  REG5}, {XCH,   "XCH   A, R6            ", 1, 1, ACC,  REG6},  {XCH,  "XCH   A, R7            ", 1, 1, ACC,  REG7}, \
           \
/* 0xD0 */ {POP,  "POP   iaddr            ", 2, 2, DIR,  NONE},  {ACALL, "ACALL page6            ", 2, 3, NONE, NONE}, {SETB,  "SETB  baddr            ", 2, 2, BIT,  NONE},  {SETB, "SETB  C                ", 1, 1, CC,   NONE}, \
/* 0xD4 */ {DA,   "DA                     ", 1, 1, ACC,  NONE},  {DJNZ,  "DJNZ  iaddr, raddr     ", 3, 3, DIR,  REL},  {XCHD,  "XCHD  A, @R0           ", 1, 2, ACC,  IND0},  {XCHD, "XCHD  A, @R1           ", 1, 2, ACC,  IND1}, \
/* 0xD8 */ {DJNZ, "DJNZ  R0, raddr        ", 2, 2, REG0, REL},   {DJNZ,  "DJNZ  R1, raddr        ", 2, 2, REG1, REL},  {DJNZ,  "DJNZ  R2, raddr        ", 2, 2, REG2, REL},   {DJNZ, "DJNZ  R3, raddr        ", 2, 2, REG3, REL},  \
/* 0xDC */ {DJNZ, "DJNZ  R4, raddr        ", 2, 2, REG4, REL},   {DJNZ,  "DJNZ  R5, raddr        ", 2, 2, REG5, REL},  {DJNZ,  "DJNZ  R6, raddr        ", 2, 2, REG6, REL},   {DJNZ, "DJNZ  R7, raddr        ", 2, 2, REG7, REL},  \
 \
/* 0xE0 */ {MOVX, "MOVX  A, @DPTR         ", 1, 3, ACC,  EXT},   {AJMP,  "AJMP  page7            ", 2, 3, NONE, NONE}, {MOVX,  "MOVX  A, @R0           ", 1, 3, ACC,  EXT0},  {MOVX, "MOVX  A, @R1           ", 1, 3, ACC,  EXT1}, \
/* 0xE4 */ {CLR,  "CLR   A                ", 1, 1, ACC,  NONE},  {MOV,   "MOV   A, iaddr         ", 2, 2, ACC,  DIR},  {MOV,   "MOV   A, @R0           ", 1, 2, ACC,  IND0},  {MOV,  "MOV   A, @R1           ", 1, 2, ACC,  IND1}, \
/* 0xE8 */ {MOV,  "MOV   A, R0            ", 1, 1, ACC,  REG0},  {MOV,   "MOV   A, R1            ", 1, 1, ACC,  REG1}, {MOV,   "MOV   A, R2            ", 1, 1, ACC,  REG2},  {MOV,  "MOV   A, R3            ", 1, 1, ACC,  REG3}, \
/* 0xEC */ {MOV,  "MOV   A, R4            ", 1, 1, ACC,  REG4},  {MOV,   "MOV   A, R5            ", 1, 1, ACC,  REG5}, {MOV,   "MOV   A, R6            ", 1, 1, ACC,  REG6},  {MOV,  "MOV   A, R7            ", 1, 1, ACC,  REG7}, \
 \
/* 0xF0 */ {MOVX, "MOVX  DPTR, A          ", 1, 3, EXT,  ACC},   {ACALL, "ACALL page7            ", 2, 3, NONE, NONE}, {MOVX,  "MOVX  @R0, A           ", 1, 3, EXT0, ACC},   {MOVX, "MOVX  @R1, A           ", 1, 3, EXT1, ACC},  \
/* 0xF4 */ {CPL,  "CPL   A                ", 1, 1, ACC,  NONE},  {MOV,   "MOV   iaddr, A         ", 2, 2, DIR,  ACC},  {MOV,   "MOV   @R0, A           ", 1, 2, IND0, ACC},   {MOV,  "MOV   @R1, A           ", 1, 2, IND1, ACC},  \
/* 0xF8 */ {MOV,  "MOV   R0, A            ", 1, 1, REG0, ACC},   {MOV,   "MOV   R1, A            ", 1, 1, REG1, ACC},  {MOV,   "MOV   R2, A            ", 1, 1, REG2, ACC},   {MOV,  "MOV   R3, A            ", 1, 1, REG3, ACC},  \
/* 0xFC */ {MOV,  "MOV   R4, A            ", 1, 1, REG4, ACC},   {MOV,   "MOV   R5, A            ", 1, 1, REG5, ACC},  {MOV,   "MOV   R6, A            ", 1, 1, REG6, ACC},   {MOV,  "MOV   R7, A            ", 1, 1, REG7, ACC}   \
}


// Defer structure definition
struct  DecodeStruct;

typedef struct DecodeStruct *pDecode_t;
typedef struct DecodeStruct Decode_t;
typedef void (*pfunc_t) (pDecode_t);

// Decode table element type definition
typedef struct {
    pfunc_t  func;             // Pointer to execution function
    char*    instr_name;       // Instruction name
    int      instr_size;       // Instruction size in bytes
    int      clk_cycles;       // Number of clock cycles for instruction execution
    int      addr_mode_op1;    // Addressing mode operand 1
    int      addr_mode_op2;    // Addressing mode operand 2
} DecodeData_t, *pDecodeData_t;

// Current decoded instruction type
struct DecodeStruct {
    uint8_t             opcode;    // Instruction opcode
    uint8_t             arg0;      // First byte (where applicable)
    uint8_t             arg1;      // Second byte (where applicable)
    pDecodeData_t       decode;    // Pointer to entry in decode table for opcode
} ;

// Type definition for timer callback function (internal callback, not visible to user)
typedef int  (*ptimcallback_t) (int time, int *wakeup_time, int timer);

// Shared variables from inst.c
extern uint8_t  code_mem [MAXCODEMEM];
extern uint8_t  int_ram  [MAXINTRAM];
extern uint8_t  ext_ram  [MAXEXTRAM];
extern int      acc;            // Accumulator
extern uint8_t* r;              // Registers (pointer mapped over active register bank in int_ram)
extern int      pc;
extern int      sp;
extern int      dptr;
extern int      cycle_count;
extern int      break_point;
extern int      ie;
extern int      ip;
extern int      int_level;
extern int      last_int_level;
extern int      wakeup_time_ext_mem;
extern int      wakeup_time_sfr;
extern int      always_call_sfr_cb;

extern pmemcallback_t pExtCallback;
extern pmemcallback_t pSfrCallback;

// Function prototypes from inst.c

extern void reset_cpu  ();
extern int  fetch_sfr  (int addr);
extern void set_sfr    (int addr, int arg);

// Instruction functions
void ACALL      (pDecode_t d);
void ADD        (pDecode_t d);
void AJMP       (pDecode_t d);
void ANL        (pDecode_t d);
void CJNE       (pDecode_t d);
void CLR        (pDecode_t d);
void CPL        (pDecode_t d);
void DA         (pDecode_t d);
void DEC        (pDecode_t d);
void DIV        (pDecode_t d);
void DJNZ       (pDecode_t d);
void INC        (pDecode_t d);
void JB         (pDecode_t d);
void JBC        (pDecode_t d);
void JC         (pDecode_t d);
void JMP        (pDecode_t d);
void JNB        (pDecode_t d);
void JNC        (pDecode_t d);
void JNZ        (pDecode_t d);
void JZ         (pDecode_t d);
void LCALL      (pDecode_t d);
void LJMP       (pDecode_t d);
void MOV        (pDecode_t d);
void MOVC       (pDecode_t d);
void MOVX       (pDecode_t d);
void MUL        (pDecode_t d);
void NOP        (pDecode_t d);
void ORL        (pDecode_t d);
void POP        (pDecode_t d);
void PUSH       (pDecode_t d);
void RET        (pDecode_t d);
void RETI       (pDecode_t d);
void RL         (pDecode_t d);
void RLC        (pDecode_t d);
void RR         (pDecode_t d);
void RRC        (pDecode_t d);
void SETB       (pDecode_t d);
void SJMP       (pDecode_t d);
void SUBB       (pDecode_t d);
void SWAP       (pDecode_t d);
void XCH        (pDecode_t d);
void XCHD       (pDecode_t d);
void XRL        (pDecode_t d);
void UNIMP      (pDecode_t d);


#endif
