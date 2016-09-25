//=============================================================
// 
// Copyright (c) 2012 Simon Southwell. All rights reserved.
//
// Date: 25th August 2012  
//
// This file is part of the cpu8051 instruction set simulator.
//
// This file contains the intel hex format code loader for the 
// ISS.
//
// $Id: read_ihx.c,v 1.1 2013-06-25 18:40:49 simon Exp $
// $Source: /home/simon/CVS/src/cpu/8051/src/read_ihx.c,v $
//
//=============================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "read_ihx.h"

// -------------------------------------------------------------------------
// hex2int()
//
//   Convert characters in a buffer representing hex data into an integer
//   value
// -------------------------------------------------------------------------
static int hex2int(uint8_t *buf, int num_bytes) {

    int idx;
    int value = 0;

    for (idx = 0; idx < (num_bytes*2); idx++) {

        int digit;

        digit = *(buf+idx);

        value <<= 4;

        if ((digit >= 'a') && (digit <= 'f'))
            value |= digit - 'a' + 10;
        else if ((digit >= 'A') && (digit <= 'F'))
            value |= digit - 'A' + 10;
        else
            value |= digit - '0';

    }

    return value;
}

// -------------------------------------------------------------------------
// read_ihx()
//
//   Read a file in intel hex format and place in a provided buffer
// -------------------------------------------------------------------------
int read_ihx (char *filename, uint8_t* code_mem) {

    uint32_t byte_count;
    uint32_t address;
    uint32_t record_type;
    uint32_t data_byte;

    char line [MAXLINE];
    int eof = 0;

    FILE *file = fopen (filename, "r");

    if (file == NULL) 
        return FILE_ERROR;

    // Read in a line
    while (fgets (line, sizeof line, file) != NULL) {

        uint8_t* buf_ptr = line;

        // Check for intel hex format leading colon
        if (*buf_ptr != ':') 
            return FORMAT_ERROR;
        else
            buf_ptr++;

        // Get byte count
        byte_count = hex2int(buf_ptr, 1);
        buf_ptr += 2;

        // Get address
        address    = hex2int(buf_ptr, 2);
        buf_ptr += 4;

        // Get record type
        record_type = hex2int(buf_ptr, 1);
        buf_ptr += 2;
        
        // If data record, then process
        if (record_type == TYPE_DATA) {

            unsigned idx;

            for (idx = 0; idx < byte_count; idx++) {

                // Get data bytes
                data_byte = hex2int(buf_ptr, 1);
                buf_ptr += 2;

                code_mem[address++] = data_byte;

#ifdef DEBUG
                printf("code_mem[0x%04x] = %02x\n", address-1, data_byte);
#endif
            }

        // If EOF type, the break from loop
        } else if (record_type == TYPE_EOF)
            break;
        // Got a type we don't support
        else
            return UNRECOGNISED_TYPE;
    }

    // Got here if everything okay
    return NO_ERROR;
}

#ifdef DEBUG
int main()
{
    uint8_t buf[64*1024];

    read_ihx("try.ihx", buf);
}
#endif
