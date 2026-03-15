#include "video_debug.h"
#include "../ppu/ppu.h"

#include <stdint.h>
//#include <string.h>

#define VRAM_TILE_DATA_8000_OFFSET  0x0000
#define VRAM_BG_MAP_9800_OFFSET     0x1800

void build_test_pattern(PPU *ppu) {
    int y;
    int x;

    for (y = 0; y < GB_BG_HEIGHT; y++) {
        for (x = 0; x < GB_BG_WIDTH; x++) {
            int block_x;
            int block_y;
            uint32_t color;

            block_x = x / 8;
            block_y = y / 8;

            color = ((block_x + block_y) & 1) ? 0xFFFFFFFFu : 0xFF000000u;

            ppu->debug_bg_rgba[y * GB_BG_WIDTH + x] = color;
        }
    }
}


/// DECODE: TILE!

static uint32_t debug_gray_from_color_id(uint8_t color_id) {
    switch (color_id & 0x03) {
        case 0: return 0xFFFFFFFFu;
        case 1: return 0xFFAAAAAAu;
        case 2: return 0xFF555555u;
        case 3: return 0xFF000000u;
        default: return 0xFFFF00FFu;
    }
}

static uint8_t decode_tile_pixel_2bpp(const uint8_t *tile_data, int x, int y) {
    uint8_t low_byte;
    uint8_t high_byte;
    uint8_t bit_index;
    uint8_t low_bit;
    uint8_t high_bit;

    low_byte = tile_data[y * 2];
    high_byte = tile_data[y * 2 + 1];

    bit_index = (uint8_t)(7 - x);

    low_bit = (low_byte >> bit_index) & 0x01;
    high_bit = (high_byte >> bit_index) & 0x01;

    return (uint8_t)((high_bit << 1) | low_bit);
}

void build_debug_test_bg(PPU *ppu, const uint8_t *vram) {
    int map_y;
    int map_x;

    for (map_y = 0; map_y < 32; map_y++) {
        for (map_x = 0; map_x < 32; map_x++) {
            uint16_t map_index;
            uint8_t tile_index;
            const uint8_t *tile_data;
            int tile_y;
            int tile_x;

            map_index = (uint16_t)(VRAM_BG_MAP_9800_OFFSET + (map_y * 32) + map_x);
            tile_index = vram[map_index];

            tile_data = &vram[VRAM_TILE_DATA_8000_OFFSET + (tile_index * 16)];

            for (tile_y = 0; tile_y < 8; tile_y++) {
                for (tile_x = 0; tile_x < 8; tile_x++) {
                    int pixel_x;
                    int pixel_y;
                    uint8_t color_id;

                    pixel_x = (map_x * 8) + tile_x;
                    pixel_y = (map_y * 8) + tile_y;

                    color_id = decode_tile_pixel_2bpp(tile_data, tile_x, tile_y);

                    ppu->debug_bg_rgba[pixel_y * GB_BG_WIDTH + pixel_x] =
                        debug_gray_from_color_id(color_id);

                    // Debugging: will throw a bunch of red lines on display
                    // if ((map_y + tile_y) % 4) {
                    //     ppu->debug_bg_rgba[pixel_y * GB_BG_WIDTH + pixel_x] = 0xFF000000; // black
                    // } else{
                    //     ppu->debug_bg_rgba[pixel_y * GB_BG_WIDTH + pixel_x] =
                    //         debug_gray_from_color_id(color_id);
                    // }
                }
            }
        }
    }
}