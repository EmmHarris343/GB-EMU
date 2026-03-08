#ifndef PPU_H
#define PPU_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct gb_s GB;

// HRAM
#define VRAM_SIZE 0x2000          // 8KB total size, (8192 Byte in Decimal, 2000 in Hex)


// Init ppu function
int ppu_init(GB *gb);

// Read/Write ppu functions.
uint8_t ppu_read(GB *gb, uint16_t addr);
void ppu_write(GB *gb, uint16_t addr, uint8_t val);

#endif