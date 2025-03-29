#ifndef LOC_RAM_H
#define LOC_RAM_H

#include <stdint.h>










// Prototypes:
uint8_t loc_wram_read(uint16_t addr);
void loc_wram_write(uint16_t addr, uint8_t val);
uint8_t loc_eram_read(uint16_t addr);
void loc_eram_write(uint16_t addr, uint8_t val);
uint8_t loc_hram_read(uint16_t addr);
void loc_hram_write(uint16_t addr, uint8_t val);

#endif