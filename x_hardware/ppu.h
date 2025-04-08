#ifndef PPU_H
#define PPU_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>



// HRAM 
#define VRAM_SIZE 0x2000          // 8KB total size, (8192 Byte in Decimal, 2000 in Hex)


// Prototypes:
int init_ppu();


uint8_t ppu_read(uint16_t addr);
void ppu_write(uint16_t addr, uint8_t val);

#endif