#pragma once    // Ensure header file is included once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Component files:
#include "cpu.h"



#define OP_Extra_size       0x50            // Lets just... load a bunch of data for fun

#define GB_VER              0x01            // (This is just my numbering system) DMG0 = 0x00, DMG = 0x01, ... MGB, SGB, SGB2

// Rom Header:
#define HEADER_OFFSET       0x0100          // Same as Start, just easier to understand
#define HEADER_SIZE         0x50            // Header END point is: 0x014f (50 Bytes Total - 80 Decimal)





// 1024 bytes + 1024 bytes = 2 kilobytes
// 1024 Kilobytes = 1 mb
// 8mb (max rom size) = 8192 kilobytes, 8192 kilobytes = 8388608 bytes? 
// So 8mb = 800000 (hex)    -- Max of 8MB For GBC


// Time? Copy pasta...

typedef union {
    struct {
        uint8_t seconds;
        uint8_t minutes;
        uint8_t hours;
        uint8_t days;
    };
    struct {
        uint8_t seconds;
        uint8_t minutes;
        uint8_t hours:5;
        uint8_t weekday:3;
        uint8_t weeks;
    } time_strc;
    uint8_t time_data[5];
} CORE_rtc_time_t;

