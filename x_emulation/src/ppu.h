#ifndef PPU_H
#define PPU_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct gb_s GB;

// HRAM
#define VRAM_SIZE 0x2000          // 8KB total size, (8192 Byte in Decimal, 2000 in Hex)

typedef struct ppu_s {
    uint8_t lcdc;   // LCD control
    uint8_t stat;   // LCD status; Will be set when ly=lyc
    uint8_t scy;    // SCY/ SCX define the x,y coordinate of the viewport.
    uint8_t scx;
    uint8_t ly;     // Line that is going to be drawn. (IE: The current scanline.)
    uint8_t lyc;    // When ly=lyc (ly reach end), interupt can be set / stat value will be set.

    // Paletts:
    uint8_t bgp;    // Big Palettte
    uint8_t obp0;   // 0/1 split colour palette for sprites, these work the same as the bgp, last 2 bits for transparency.
    uint8_t obp1;

    // Window:
    uint8_t wy;
    uint8_t wx;

    // Time / mode:
    uint32_t line_cycles;    // Basically out of 144. When y reaches 144. Set flags, and request interupt.
    uint8_t mode;


    // Data storage:
    uint8_t oam[0xA0];      // Object Attribute Memory, can reference sprite / tiles.
    uint8_t vram[0x2000];   // Vram total size.

} PPU;

typedef enum {
    PPU_IO_LCDC,
    PPU_IO_STAT,
    PPU_IO_SCY,
    PPU_IO_SCX,
    PPU_IO_LY,
    PPU_IO_LYC,
    PPU_IO_BTMP,
    PPU_IO_WDOW
} ppu_io_tag;    // BUS tag names.

// PPU Mode values:
enum {
    PPU_MODE_HBLANK   = 0,
    PPU_MODE_VBLANK   = 1,
    PPU_MODE_OAM      = 2,
    PPU_MODE_TRANSFER = 3
};

// Init ppu function
int ppu_init(GB *gb);

// PPU IO Read/Write functions.
uint8_t ppu_io_read(GB *gb, uint16_t addr);
void ppu_io_write(GB *gb, uint16_t addr, uint8_t val);

// PPU Read/Write VRAM functions.
uint8_t ppu_vram_read(GB *gb, uint16_t addr);
void ppu_vram_write(GB *gb, uint16_t addr, uint8_t val);

// PPU Timer/ Tick:
void ppu_tick(GB *gb, PPU *ppu, uint32_t cycles);

#endif