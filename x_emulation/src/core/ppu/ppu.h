#ifndef PPU_H
#define PPU_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define GB_LCD_WIDTH   160
#define GB_LCD_HEIGHT  144
#define GB_BG_WIDTH    256
#define GB_BG_HEIGHT   256

typedef struct gb_s GB;

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
    uint8_t y_condition;

    // Time / mode:
    uint32_t line_cycles;    // Basically out of 144. When y reaches 144. Set flags, and request interupt.
    uint8_t mode;


    // Data storage:
    uint8_t oam[0xA0];      // Object Attribute Memory, can reference sprite / tiles.
    uint8_t vram[0x2000];   // Vram total size.

    // I don't love these here. Maybe it's fine..
    // Because I confuse myself. These are (in C terms), an "object array" that holds many values that can be indexed.
    // In terms of graphics/ display these are: Pixel Buffers, Frame Buffers, Background Buffers, Raster Buffers etc.
    // It will only hold the data intended for a specific frame, or group of pixels.
    uint32_t debug_lcd_rgba[GB_LCD_WIDTH * GB_LCD_HEIGHT];  // The viewport size.
    uint32_t debug_bg_rgba[GB_BG_WIDTH * GB_BG_HEIGHT];     // The background size.

    uint32_t vp_rgba_buffer[GB_LCD_WIDTH * GB_LCD_HEIGHT];    // The viewport buffer size.
    uint32_t win_rgba_buffer[GB_BG_WIDTH * GB_BG_HEIGHT];   // The window buffer size.
    uint32_t bg_rgba_buffer[GB_BG_WIDTH * GB_BG_HEIGHT];      // The background buffer size.

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