#ifndef LOC_RAM_H
#define LOC_RAM_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Indeed...

#define HRAM_SIZE 0x80          // 127 bytes from 0xFF80 to 0xFFFE
#define WRAM_SIZE 0x2000
#define ERAM_SIZE 0x1E00





// Prototypes:
int init_loc_ram();
uint8_t loc_wram_read(uint16_t addr);
void loc_wram_write(uint16_t addr, uint8_t val);
uint8_t loc_eram_read(uint16_t addr);
void loc_eram_write(uint16_t addr, uint8_t val);
uint8_t loc_hram_read(uint16_t addr);
void loc_hram_write(uint16_t addr, uint8_t val);

#endif