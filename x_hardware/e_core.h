#pragma once    // Ensure header file is included once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define OP_Extra_size       0x50      // Lets just... load a bunch of data for fun

#define GB_VER              0x01       // (This is just my numbering system) DMG0 = 0x00, DMG = 0x01, ... MGB, SGB, SGB2

// ROM / Rom Banks
#define FIXED_BANK_SIZE     0x4000          // The fixed bank at 0x0000-0x3FFF
#define ROM_BANK_SIZE       0x4000          // 16 KB banks

// Ram sizes
#define WRAM_size           0x2000
#define HRAM_size           0x80
#define VRAM_size           0x2000

#define M_MAP_size          0x10000

#define OAM_size            0xA0        // (160)
#define io_rgstr_size       0x80        // (128)     // maybe don't use, as it's not normally an array



// Global Memory Variables
extern uint8_t WRAM[WRAM_size];
extern uint8_t HRAM[HRAM_size];
extern uint8_t VRAM[VRAM_size];

extern uint8_t EXT_RAM;                 // Dynamic.

extern uint8_t memory_map[M_MAP_size];

extern uint16_t PC;
extern uint16_t SP;

