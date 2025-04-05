#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <stdio.h>

// Ram sizes
#define WRAM_size           0x2000
#define HRAM_size           0x80
#define VRAM_size           0x2000

#define M_MAP_size          0x10000         // Total memory Map Size (full range CPU as access to, THS IS NOT ROM SIZE)

#define OAM_size            0xA0            // (160)
#define io_rgstr_size       0x80            // (128)     // maybe don't use, as it's not normally an array


// extern uint8_t WRAM[WRAM_size];
// extern uint8_t HRAM[HRAM_size];
// extern uint8_t VRAM[VRAM_size];
// extern uint8_t EXT_RAM;                     // Dynamic.
// extern uint8_t ROM;                         // Dynamic.
// extern uint8_t memory_map[M_MAP_size];
// extern uint16_t PC;
// extern uint16_t SP;





//void test_bank_switch();


#endif