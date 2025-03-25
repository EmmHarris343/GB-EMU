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

// ROM / Rom Banks
#define FIXED_BANK_SIZE     0x4000          // The fixed bank at 0x0000-0x3FFF
#define ROM_BANK_SIZE       0x4000          // 16 KB banks
#define ROM_BANK_SIZE       0x4000          // 16 KB banks
#define MAX_ROM_SIZE        0x800000        // 8MB Max Rom Size. (Note 800000 => 8,388,608 bytes. /1024 = 8,192 KB)

// Ram sizes
#define WRAM_size           0x2000
#define HRAM_size           0x80
#define VRAM_size           0x2000

#define M_MAP_size          0x10000         // Total memory Map Size (full range CPU as access to, THS IS NOT ROM SIZE)

#define OAM_size            0xA0            // (160)
#define io_rgstr_size       0x80            // (128)     // maybe don't use, as it's not normally an array



// 1024 bytes + 1024 bytes = 2 kilobytes
// 1024 Kilobytes = 1 mb
// 8mb (max rom size) = 8192 kilobytes, 8192 kilobytes = 8388608 bytes? 
// So 8mb = 800000 (hex)


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


// This meth useses this junk:
// GCC Compiler thing.. what ever this is...
// #define GB_SECTION(name, ...) _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wpedantic\"") alignas(8) char _align_##name[0]; __VA_ARGS__ _Pragma("GCC diagnostic pop")

// GB_SECTION(unsaved,
//     /* ROM */
//     uint8_t *rom_file;
//     uint32_t rom_size;
//     //const GB_cartridge_t *cartridge_type;
//     // enum {
//     //     GB_STANDARD_MBC1_WIRING,
//     //     GB_MBC1M_WIRING,
//     // } mbc1_wiring;
//     //bool is_mbc30;

//     unsigned pending_cycles;
           
//     /* Various RAMs */
//     uint8_t *ram;
//     uint8_t *vram;
//     uint8_t *mbc_ram;
// )




// Global Memory Variables
extern uint8_t WRAM[WRAM_size];
extern uint8_t HRAM[HRAM_size];
extern uint8_t VRAM[VRAM_size];

extern uint8_t EXT_RAM;                     // Dynamic.

extern uint8_t ROM;                         // Dynamic.
extern uint8_t ROM_header_raw[HEADER_SIZE];     // This is a constant size in all ROMs.

struct {
    uint8_t Hd_entry_point[4];
    uint8_t Hd_logo[47];
    uint8_t Hd_title[16];       // +1 cause string.. I guess. --- Depending on rom not used at all, and non populated.
    uint8_t Hd_cart_type;
    uint8_t Hd_rom_size;
    uint8_t Hd_ram_size;
    uint8_t Hd_chksm;
    uint8_t Hd_gbl_chksm[2];    // Technically 16 bit....
    uint8_t Hd_cgb_f;
    uint8_t Hd_sgb_f;
    uint8_t Hd_new_lic[2];      // Only used if old licence = $33
    uint8_t Hd_old_lic;
} Header;



extern uint8_t memory_map[M_MAP_size];


extern uint16_t PC;
extern uint16_t SP;


