#ifndef LOC_RAM_H
#define LOC_RAM_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>


typedef struct gb_s GB;

// Indeed...

#define HRAM_SIZE 0x80          // 127 bytes from 0xFF80 to 0xFFFE
#define WRAM_SIZE 0x2000
#define ERAM_SIZE 0x1E00





// Init
int loc_ram_init(GB *gb);

// Read/ Write ram functions:
uint8_t loc_wram_read(GB *gb, uint16_t addr);
void loc_wram_write(GB *gb, uint16_t addr, uint8_t val);
uint8_t loc_echram_read(GB *gb, uint16_t addr);
void loc_echram_write(GB *gb, uint16_t addr, uint8_t val);
uint8_t loc_hram_read(GB *gb, uint16_t addr);
void loc_hram_write(GB *gb, uint16_t addr, uint8_t val);

#endif