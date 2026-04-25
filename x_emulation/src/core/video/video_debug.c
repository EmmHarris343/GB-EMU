#include <SDL2/SDL_pixels.h>
#include "video_debug.h"
#include "../ppu/ppu.h"

#include <SDL2/SDL_surface.h>
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

// Tile does not encode color informmation. Instead a tile assign color indices toe each pixel 0-3.

void build_debug_test_bg(PPU *ppu, const uint8_t *vram, SDL_PixelFormat *gb_pixel_format) {
    int map_y;
    int map_x;
    // uint32_t color_count[4] = {0, 0, 0, 0};

    uint8_t lcdc;
    uint16_t map_base_offset;
    int use_unsigned_tile_indices;

    lcdc = ppu->lcdc;

    map_base_offset = (lcdc & (1u << 3))    // Which map is LCDC currently on.
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

    for (map_y = 0; map_y < 32; map_y++) {
        for (map_x = 0; map_x < 32; map_x++) {
            uint16_t map_index;
            uint8_t tile_index;
            const uint8_t *tile_data;
            int tile_y;
            int tile_x;

            // Background offset. Either 9C00 or 9800.
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


// This builds the "background" map.
void build_bg(PPU *ppu, const uint8_t *vram, SDL_PixelFormat *gb_pixel_format) {

}



void build_bg_window(PPU *ppu, const uint8_t *vram, SDL_PixelFormat *gb_pixel_format) {
    int map_y;
    int map_x;
    // uint32_t color_count[4] = {0, 0, 0, 0};

    // Besides the BG there is also a Window overlaying it. The content of the Window is NOT scrollable.
    // The TOP-LEFT cord of the window are: (WX-7, WY). Doing wx=7, wy=0 sets the window to the top left of the "screen"
    // The tiles for the Window are stored in the Tile Data table. (Both BG and Window share the same table)
    // Window is visible (if enabled) when WX and WY are in the range: [0; 166] and [0;143].
    int window_x;
    int window_y;

    // These are loaded from the registers: FF42-FF43. SCY and SCX respectively.
    // These are the top-left coordinates of the visabler 160x144 pixel area. Within the 256x256 BG map.
    int viewport_y;
    int viewport_x;

    // This is the calculated bottom-right pixel. Which will wrap the screen based on the math.
    // Math:
    // bottom := (SCY +143) % 256
    // Right: (SCX + 159) % 256
    int BR_viewport_y;
    int BR_viewport_x;


    // Some of these might not be needed. Oh welllll
    uint8_t lcdc;
    uint16_t map_base_offset;
    uint16_t window_map_offset;
    uint8_t win_enabled;
    uint8_t y_condition;
    uint8_t win_render_enabled;

    uint8_t OBJ_size_control;
    uint8_t OBJ_enabled;
    uint8_t bg_win_enabled;
    int use_unsigned_tile_indices;

    lcdc = ppu->lcdc;

    y_condition = ppu->y_condition;

    // The window:
    window_y = ppu->wy;
    window_x = ppu->wx;

    // The Scrolling Viewport:
    viewport_y = ppu->scy;
    viewport_x = ppu-> scx;
    BR_viewport_y = (viewport_y + 143) % 256;   // Mod allows wrapping if goes past 256.
    BR_viewport_x = (viewport_x + 159) % 256;


    // The VRAM Tile maps containts two 32x32 tile maps. These are used for the BG or the Window.
    // Bit 5, controls wether the window should be displayed or not.
    win_enabled = (lcdc & (1u << 5)) ? 1 : 0;
    // Bit 6 controls which background map the Window will use for rendering.
    window_map_offset = (lcdc & (1u << 6))
    ? VRAM_BG_MAP_9C00_OFFSET
    : VRAM_BG_MAP_9800_OFFSET;
    // Bit 3 controls which BG tile map area is used for rendering.
    map_base_offset = (lcdc & (1u << 3))
    ? VRAM_BG_MAP_9C00_OFFSET
    : VRAM_BG_MAP_9800_OFFSET;
    // Bit 2 controls size of object. If 1 or 2 tiles are stacked vertically.
    OBJ_size_control = (lcdc & (1u << 2)) ? 1 : 0;
    // Bit 1 controls enable / disable of OBJ if they are displayed or not.
    OBJ_enabled = (lcdc & (1u << 1)) ? 1 : 0;
    // Bit 0 BG / window enabled/ disabled. (Will be come blank white), and window display bit is ignored.
    bg_win_enabled = (lcdc & (1u << 0)) ? 1 : 0;

    // Window X/Y in range. x[0;166] y[0;143]
    if ((window_x >= 0 && window_x <= 166) && (window_y >= 0 && window_y <= 143)) {
        if (bg_win_enabled && win_enabled) {
            win_render_enabled = (y_condition) ? 1 : 0;
        } else {
            win_render_enabled = 0;
        }
    } else {
        win_render_enabled = 0;
    }


    use_unsigned_tile_indices = (lcdc & (1u << 4)) ? 1 : 0;

    // Tile data is in memory at: $8000-$97FF. Each tile takes 16 bytes.
    // This area of VRAM defines data for 384 tiles.

    // This builds the full "Background" map.
    for (map_y = 0; map_y < 32; map_y++) {  // This prints Top/Down. Left/Right.
        for (map_x = 0; map_x < 32; map_x++) {
            uint16_t map_index;
            uint8_t tile_index;
            const uint8_t *tile_data;
            int tile_y;
            int tile_x;

            // Background offset. Either 9C00 or 9800.
            map_index = (uint16_t)(map_base_offset + (map_y * 32) + map_x); // Generates a 2d Map index.
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


            // These tiles can be Background/Window Maps. Or as objects (Movable sprites).
            // Note Color 0 for sprites / objects mean it's transparent. Allowing background/ other to show through.

            // This prints the actual tiles / with pixel color data.
            for (tile_y = 0; tile_y < 8; tile_y++) {
                for (tile_x = 0; tile_x < 8; tile_x++) {
                    int pixel_x;
                    int pixel_y;
                    uint8_t color_id;

                    // Each tile is 8x8, with 2bit color depth per pixel.
                    pixel_x = (map_x * 8) + tile_x;
                    pixel_y = (map_y * 8) + tile_y;

                    color_id = decode_tile_pixel_2bpp(tile_data, tile_x, tile_y);

                    // Puts the "pixel data" into the debug_bg[BG_SIZE] value.
                    ppu->debug_bg_rgba[pixel_y * GB_BG_WIDTH + pixel_x] =
                        debug_gray_from_color_id(gb_pixel_format, color_id);
                }
            }
        }
    }
}