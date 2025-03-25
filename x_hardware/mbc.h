#ifndef MBC_H
#define MBC_H

#include <stdint.h>

// Expose these function signatures to cart.c or mmu.c
void mbc1_write(uint16_t addr, uint8_t val);
void mbc3_write(uint16_t addr, uint8_t val);

#endif