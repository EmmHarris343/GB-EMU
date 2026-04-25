#include <SDL2/SDL_pixels.h>
#include "display.h"
#include "../ppu/ppu.h"

#include <SDL2/SDL_surface.h>
#include <stdint.h>

/*
A glossary to keep track of each X/Y coordinate and their name. As this can get confusing FAST.

screen_y, screen_x
    = location on the Game Boy LCD, 0..143 and 0..159

bg_y, bg_x
    = location inside the 256x256 background map after SCY/SCX

win_y, win_x
    = location inside the Window layer, starting at 0,0

tile_y, tile_x
    = which 8x8 tile inside the 32x32 tile map

tile_pixel_y, tile_pixel_x
    = which pixel inside that 8x8 tile

--- ADD object/ sprite x/y and sprite pixel x/y.


*/




static uint32_t decode_gray_from_color_id(uint8_t color_id) {
    switch (color_id & 0x03) {
        case 0:
            return 0xFFFFFFFF;
        case 1:
            return 0xAAAAAAFF;
        case 2:
            return 0x555555FF;
        case 3:
            return 0x000000FF;
        default:
            return 0xFF00FFFF;
    }
}
static uint8_t decode_tile_pixel(const uint8_t *tile_data, uint8_t y, uint8_t x) {
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

/*

The Background Layer / Background Tile Map:
These are technically different, but still the "same".
The Background Layer = 256x256 Pixels. | The Background Tile Map = 32x32 Tiles.
Each Tile = 8x8 pixels.

The window is it's own layer, that rests on top of the background.
    Which is often used for UI elements, progress bars, health, items etc
    The window has many requirements to be "enabled" including LCDC bit values, LY and WX WY values / coordinates.

The viewport is only 160x144. It scrolls the Background when it reaches the boundry, it wraps back.
    The view port is "what you see".
    The view port does NOT scroll with the window. It is constant. But it's coordinates can be changed.


*/





/// KEY: The main key takeaway of what is happening.
// Yes the VRAM has tiles, tile maps, sprites, objects, etc.
// BUT. They need to be drawn on the "LCD" at least from a GB perspective.

// I don't have an LCD, I have a window. Instead of drawing to a LCD, I'm populating a frame buffer.
// I'm taking advantage of the GB LCDC, LY, pixels, tile maps, working to render the LCD.
// TO set the correct bits inside the frame buffer. That the SDL2 can read and display in a Host-Window.









/// PAUSE:

/*

NOW TO DO:
As LY / the scanline progresses down the screen.
Draw one line at a time into the buffer.

The scanline would intersect the tiles as it passes through them.
Drawing 0,1,2,3,4,5,6,7 pixels until it passes those tiles. Then needs to load the next "chunk".

So basically. The current LY I would need to calculate which tiles it would pass through.

Worth also mentioning... I should likely do this ONLY for the "viewport".
The view port is 160x144. 144 is also the length of LY.

So SCY = LY, SCX = only a percentage of the BG

Basically.....
I would need to check in the current Y (144) and X (160) space.
Where SCY and SCX TOP-LEFT location is, and based on that..
What background Tiles and Window Tiles would need to be rendered/ partially rendered.

Basically a 256x256 box, with a smaller box inside that can be scrolled/ moved around.


Then after I would also need to add the OAM / object/ sprite support and draw those.


*/

const uint8_t *get_tile_data(int unsigned_indices, uint16_t map_index, const uint8_t *vram) {
    const uint8_t *tile_data;
    uint8_t tile_index = vram[map_index];

    if (unsigned_indices) {
        // Simple math, no negative tile_indexes.
        tile_data = &vram[tile_index * 16]; // This VRAM_TILE is just 0x0000... I can just remove that entirely
    } else {
        // Convert to +/- signed addressing, and load -128 to -1 thats in block 1;
        int8_t signed_tile_index = (int8_t)tile_index;
        uint16_t tile_offset = (uint16_t)(0x1000 + (signed_tile_index * 16)); // VRAM base 0x8000 => offset 0x1000

        tile_data = &vram[tile_offset];
    }
    return tile_data;   // Returns One(1) 8x8 tile. (64 pixels total size)
}


uint8_t get_win_pixel(PPU *ppu, const uint8_t *vram, uint8_t screen_y, uint8_t screen_x) {
    uint8_t lcdc = ppu->lcdc;
    uint8_t wx = ppu->wx;
    uint8_t wy = ppu->wy;

    // Bit 4 LCDC changes Reading from Block 0/1 and Block1/2. Bit cleared (0) => -128 to -1 are in block 1 (So needs Signed Int))
    // Bit 6 LCDC controls which win tile map area is used for rendering.
    uint8_t  unsigned_indices = (lcdc & (1u << 4)) ? 1 : 0;
    uint16_t win_map_offset = (lcdc & (1u << 6)) ? VRAM_BG_OFFSET_9C00 : VRAM_BG_OFFSET_9800;

    uint8_t win_y = screen_y - ppu->wy;
    uint8_t win_x = screen_x - (ppu->wx - 7);

    uint8_t tile_y = (win_y >> 3);
    uint8_t tile_x = (win_x >> 3);

    uint8_t tile_pixel_y = win_y & 0x07;
    uint8_t tile_pixel_x = win_x & 0x07;

    uint16_t map_index  = win_map_offset + (uint16_t)((tile_y * 32) + tile_x);

    const uint8_t *tile_data = get_tile_data(unsigned_indices, map_index, vram);

    // Get color ID for the pixel.
    return decode_tile_pixel(tile_data, tile_pixel_y, tile_pixel_x);;
}

uint8_t get_bg_pixel(PPU *ppu, const uint8_t *vram, uint8_t screen_y, uint8_t screen_x) {
    uint8_t lcdc = ppu->lcdc;

    // Bit 3 LCDC controls which BG tile map area is used for rendering.
    // Bit 4 LCDC changes Reading from Block 0/1 and Block1/2. With the +/- signed indexing.
    uint16_t bg_map_offset = (lcdc & (1u << 3)) ? VRAM_BG_OFFSET_9C00 : VRAM_BG_OFFSET_9800;
    uint8_t  unsigned_indices = (lcdc & (1u << 4)) ? 1 : 0;

    // Location inside the 256x256 background map after SCY/SCX
    uint8_t bg_y = ppu->scy + screen_y; // Should natrually wrap back to 0
    uint8_t bg_x = ppu->scx + screen_x;

    uint8_t tile_y = (bg_y >> 3);   // Tile row
    uint8_t tile_x = (bg_x >> 3);   // Tile column

    uint8_t tile_pixel_y = bg_y & 0x07;
    uint8_t tile_pixel_x = bg_x & 0x07;

    uint16_t map_index  = bg_map_offset + (uint16_t)((tile_y * 32) + tile_x);

    const uint8_t *tile_data = get_tile_data(unsigned_indices, map_index, vram);

    // Get & return color ID for the pixel.
    return decode_tile_pixel(tile_data, tile_pixel_y, tile_pixel_x);
}



// NOTE: This still needs to handle Objects/ Sprite priorities. (Which gets printed)
void gen_pixel_line(PPU *ppu, const uint8_t *vram) {
    uint8_t lcdc = ppu->lcdc;
    uint8_t window_enabled = (lcdc & (1u << 5)) ? 1 : 0;

    // The Y Location inside the BG 256x256 layer.
    uint8_t screen_y = ppu->ly;

    for (uint8_t screen_x = 0; screen_x < GB_LCD_WIDTH; screen_x ++) {
        uint8_t color_id = 0x00;

        if (window_enabled &&
            screen_y >= ppu->wy &&
            screen_x >= (ppu->wx - 7)) {
            color_id = get_win_pixel(ppu, vram, screen_y, screen_x);
        } else {
            color_id = get_bg_pixel(ppu, vram, screen_y, screen_x);
        }

        // Places the pixel in the correct Y,X location. Inside the viewport buffer
        ppu->vp_rgba_buffer[((uint16_t) screen_y * GB_LCD_WIDTH) + screen_x] = decode_gray_from_color_id(color_id);
    }
}








/*

IGNORE THE REST. This is wildly inefficient.


Maybe keep for some kind of "DEBUG" mode, to have it display the entire BG layer.

*/

// Generate the Background Layer Buffer. By decoding tile indices, vram offests, pixels, and stores it in a [256*256] array of pixels.
void gen_bg_layer(PPU *ppu, const uint8_t *vram, SDL_PixelFormat *gb_pixel_format) {
    // Note VRAM has: VRAM[2000]. So the offsets are -0x8000 so they aren't out of range.

    // While the PPU/ LCD is trying to draw each layer,

    uint8_t lcdc;
    lcdc = ppu->lcdc;

    // Bit 4 LCDC changes Reading from Block 0/1 and Block1/2. Bit cleared (0) => -128 to -1 are in block 1 (So needs Signed Int))
    int unsigned_indices = (lcdc & (1u << 4)) ? 1 : 0;

    // Bit 3 LCDC controls which BG tile map area is used for rendering.
    uint16_t map_base_offset = (lcdc & (1u << 3)) ? VRAM_BG_OFFSET_9C00 : VRAM_BG_OFFSET_9800;

    // Each Tile-Map is 32x32 tiles, this decodes the current selected Tile-Map based on the LCDC bit 3.
    for (uint8_t map_y = 0; map_y < 32; map_y++) {
        for (uint8_t map_x = 0; map_x < 32; map_x++) {
            const uint8_t *tile_data;

            // Calculates location/ offset of the BG Tile-Map in VRAM, to load the idividual Tile from VRAM.
            uint8_t map_index  = (uint16_t)(map_base_offset + (map_y * 32) + map_x);
            uint8_t tile_index = vram[map_index];

            if (unsigned_indices) {
                // Simple math, no negative tile_indexes.
                tile_data = &vram[VRAM_TILE_8000_OFFSET + (tile_index * 16)];
            } else {
                // This is to load the -128 to -1 signed addressing thats in block 1;
                int8_t signed_tile_index;
                uint16_t tile_offset;

                // Convert tile_index into +/- signed addressing
                signed_tile_index = (int8_t)tile_index;

                // 0x9000 as base pointer. VRAM base 0x8000 => offset 0x1000
                tile_offset = (uint16_t)(0x1000 + (signed_tile_index * 16));

                tile_data = &vram[tile_offset];
            }
        }
    }
}

// Generate the Window Layer Buffer.
void gen_win_layer(PPU *ppu, const uint8_t *vram, SDL_PixelFormat *gb_pixel_format) {
    // Consider, not generating a window layer buffer.
    // But doing it on demand while Rendering the "final" window. (Or the viewport window)

    // Window X/Y in range. x[0;166] y[0;143]
    // if ((window_x >= 0 && window_x <= 166) && (window_y >= 0 && window_y <= 143)) {
    //     if (bg_win_enabled && win_enabled) {
    //         win_render_enabled = (y_condition) ? 1 : 0;
    //     } else {
    //         win_render_enabled = 0;
    //     }
    // } else {
    //     win_render_enabled = 0;
    // }

}