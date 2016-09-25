//=============================================================
// 
// Copyright (c) 2012 Simon Southwell. All rights reserved.
//
// Date: 25th August 2012  
//
// This file is part of the cpu8051 instruction set simulator.
//
// This file is the header of the intel hex format loader.
//
// $Id: read_ihx.h,v 1.1 2013-06-25 18:40:49 simon Exp $
// $Source: /home/simon/CVS/src/cpu/8051/src/read_ihx.h,v $
//
//=============================================================

#include <stdint.h>

#ifndef _READ_IHX_H_
#define _READ_IHX_H_

#define NO_ERROR                0
#define FILE_ERROR              1
#define FORMAT_ERROR            2
#define UNRECOGNISED_TYPE       3

#define MAXLINE                 1024

#define TYPE_DATA               0
#define TYPE_EOF                1

extern int read_ihx (char *filename, uint8_t* code_mem);

#endif
