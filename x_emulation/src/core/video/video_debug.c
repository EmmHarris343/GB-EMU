#include <SDL2/SDL_pixels.h>
#include "video_debug.h"
#include "../ppu/ppu.h"

#include <stdint.h>
//#include <string.h>

#define VRAM_TILE_DATA_8000_OFFSET 0x0000
#define VRAM_TILE_DATA_9000_OFFSET 0x1000
#define VRAM_BG_MAP_9800_OFFSET    0x1800
#define VRAM_BG_MAP_9C00_OFFSET    0x1C00

void build_test_pattern(PPU *ppu, SDL_PixelFormat *gb_pixel_format){
    int y;
    int x;

    for (y = 0; y < GB_BG_HEIGHT; y++) {
        for (x = 0; x < GB_BG_WIDTH; x++) {
            int block_x;
            int block_y;
            uint32_t color;

            block_x = x / 8;
            block_y = y / 8;

            //Debugging: will throw a bunch of red lines on display
            // if ((y) % 4) {
            //     ppu->debug_bg_rgba[y * GB_BG_WIDTH + x] = 0xFF000000; // black
            // } else{
            //     ppu->debug_bg_rgba[y * GB_BG_WIDTH + x] = 0xFFFFFFFFu;
            //         //debug_gray_from_color_id(color_id);
            // }

            if ((block_x + block_y) & 1) {
                color = SDL_MapRGBA(gb_pixel_format , 0, 0, 0, 255);
                //color = SDL_MapRGBA(gb_pixel_format , 255, 255, 255, 255);
            } else {
                color = SDL_MapRGBA(gb_pixel_format, 85, 85, 85, 255);
            }

            ppu->debug_bg_rgba[y * GB_BG_WIDTH + x] = color;
            //ppu->debug_bg_rgba[y * GB_BG_WIDTH + x] = color;
        }
    }
}




// static uint32_t debug_gray_from_color_id_bad(uint8_t color_id) {
//     switch (color_id & 0x03) {
//         case 0: return 0xFFFFFFFFu;
//         case 1: return 0xFFAAAAAAu;
//         case 2: return 0xFF555555u;
//         case 3: return 0xFF000000u;
//         default: return 0xFFFF00FFu;
//     }
// }

/// DECODE: TILE!
static uint32_t debug_gray_from_color_id(SDL_PixelFormat *pixel_format, uint8_t color_id) {
    switch (color_id & 0x03) {
        case 0:
            return SDL_MapRGBA(pixel_format, 255, 255, 255, 255);

        case 1:
            return SDL_MapRGBA(pixel_format, 170, 170, 170, 255);

        case 2:
            return SDL_MapRGBA(pixel_format, 85, 85, 85, 255);

        case 3:
            return SDL_MapRGBA(pixel_format, 0, 0, 0, 255);

        default:
            return SDL_MapRGBA(pixel_format, 255, 0, 255, 255);
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

void build_debug_test_bg(PPU *ppu, const uint8_t *vram, SDL_PixelFormat *gb_pixel_format) {
    int map_y;
    int map_x;
    // uint32_t color_count[4] = {0, 0, 0, 0};

    uint8_t lcdc;
    uint16_t map_base_offset;
    int use_unsigned_tile_indices;

    lcdc = ppu->lcdc;

    map_base_offset = (lcdc & (1u << 3))
    ? VRAM_BG_MAP_9C00_OFFSET
    : VRAM_BG_MAP_9800_OFFSET;

    use_unsigned_tile_indices = (lcdc & (1u << 4)) ? 1 : 0;

    /*
    8000-87FF         Tile data block 0
    8800-8FFF         Tile data block 1
    9000-97FF         Tile data block 2 (signed tile mode base)
    9800-9BFF         BG tile map #0
    9C00-9FFF         BG tile map #1
    */

    /*
    Array Offset      Purpose
    --------------------------------
    0000-07FF         Tile data
    0800-0FFF         Tile data
    1000-17FF         Tile data (signed mode region)
    1800-1BFF         BG map #0
    1C00-1FFF         BG map #1
    */

    // printf("map: %02X %02X %02X %02X\n",
    //     vram[map_base + 0],
    //     vram[map_base + 1],
    //     vram[map_base + 2],
    //     vram[map_base + 3]);

    for (map_y = 0; map_y < 32; map_y++) {
        for (map_x = 0; map_x < 32; map_x++) {
            uint16_t map_index;
            uint8_t tile_index;
            const uint8_t *tile_data;
            int tile_y;
            int tile_x;

            //map_index = (uint16_t)(VRAM_BG_MAP_9800_OFFSET + (map_y * 32) + map_x); // Offset is 0x1800
            // map_index = (uint16_t)(VRAM_BG_MAP_9C00_OFFSET + (map_y * 32) + map_x); // Offset is 0x1C00. But points to 0x9C00.
            // tile_index = vram[map_index];

            // tile_data = &vram[VRAM_TILE_DATA_8000_OFFSET + (tile_index * 16)];  // offset is 0x0000

            map_index = (uint16_t)(map_base_offset + (map_y * 32) + map_x);
            tile_index = vram[map_index];

            if (use_unsigned_tile_indices) {
                tile_data = &vram[VRAM_TILE_DATA_8000_OFFSET + (tile_index * 16)];
            } else {
                int8_t signed_tile_index;
                uint16_t tile_offset;

                signed_tile_index = (int8_t)tile_index;

                /* 0x9000 relative to VRAM base 0x8000 => offset 0x1000 */
                tile_offset = (uint16_t)(0x1000 + (signed_tile_index * 16));

                tile_data = &vram[tile_offset];
            }

            for (tile_y = 0; tile_y < 8; tile_y++) {
                for (tile_x = 0; tile_x < 8; tile_x++) {
                    int pixel_x;
                    int pixel_y;
                    uint8_t color_id;

                    pixel_x = (map_x * 8) + tile_x;
                    pixel_y = (map_y * 8) + tile_y;

                    color_id = decode_tile_pixel_2bpp(tile_data, tile_x, tile_y);

                    // add color_id to histogram.
                    // color_count[color_id & 0x03]++;

                    ppu->debug_bg_rgba[pixel_y * GB_BG_WIDTH + pixel_x] =
                        debug_gray_from_color_id(gb_pixel_format, color_id);

                    // ppu->debug_bg_rgba[pixel_y * GB_BG_WIDTH + pixel_x] =
                    //     debug_gray_from_color_id_bad(color_id);

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
        // printf("BG debug color counts: c0=%u c1=%u c2=%u c3=%u\n",
        // color_count[0], color_count[1], color_count[2], color_count[3]);
    }
}