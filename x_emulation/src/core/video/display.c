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



// Based on the BG, OB10 OB01
static uint32_t decode_gray_from_palette(uint8_t palette, uint8_t color_id) {
    uint8_t shade = (palette >> ((color_id & 0x03) * 2)) & 0x03;

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

// Loads a single BG/Window tile based on the the map indexes calculated vram and the tiles x,y coordinate.
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

OAM_Entry oam_read_entry(PPU *ppu, uint8_t obj_index) {
    uint16_t  offset = ((uint16_t)obj_index * 4);
    OAM_Entry entry = {
        .y = ppu->oam[offset + 0],           // Byte 0 = Y
        .x = ppu->oam[offset + 1],           // Byte 1 = X
        .tile_id = ppu->oam[offset + 2],     // Byte 2 = Tile Index
        .attributes = ppu->oam[offset + 3],  // Attributes :: 7 = Priority; 6 = Y flip; 5 X = flip; 4 = DMG Palette; 3-0 for GBC.
    };
    // DMG Palette means the 0 OBP0 or 1 OBP1 palette.

    return entry;
}

/*
    For every ScanLine this checks if any any of the objects inside the OAM[] register
    Matches the Current Scanline Position (Y).
    Any that do, are added to the scnl_objects.

    The objects are loaded sequentially.
    Once the maximum objects per scanline has been reached. (10).
    Break and end function.
*/

uint8_t add_scnl_objects(PPU *ppu, OAM_Entry *scnl_objects, uint8_t obj_height, uint8_t screen_y) {
    uint8_t obj_count = 0;
    for (uint8_t objs = 0; objs < 40; objs++) {
        OAM_Entry obj_entry = oam_read_entry(ppu, objs);

        // Using int, so it can have -/+ addressing
        int16_t obj_y = (int16_t)obj_entry.y -16;

        // Object is inside the scanline:
        if (screen_y >= obj_y && screen_y < obj_y + obj_height) {
            scnl_objects[obj_count] = obj_entry;
            obj_count++;

            if (obj_count == 10) {  // Max objecters per scanline (10).
                break;
            }
        }
    }
    return obj_count;
}


// Gets a single pixel for the window layer
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

// Gets a single pixel for the background (BG) layer
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

uint8_t get_obj_pixel(PPU *ppu, const uint8_t *vram, OAM_Entry *obj, uint8_t screen_y, uint8_t screen_x, uint8_t object_height) {
    int16_t obj_y = (int16_t)obj->y - 16;
    int16_t obj_x = (int16_t)obj->x - 8;
    uint8_t tile_id = obj->tile_id;

    uint8_t pixel_y = (uint8_t)(int16_t)screen_y - obj_y;
    uint8_t pixel_x = (uint8_t)(int16_t)screen_x - obj_x;

    if (obj->attributes & (1u << 6)) {  // y flipped
        pixel_y = (object_height - 1 - pixel_y);
    }
    if (obj->attributes & (1u << 5)) {  // x flipped
        pixel_x = (7 - pixel_x);
    }

    if (object_height == 16) {
        tile_id &= 0XFE;

        if (pixel_y >= 8) {
            tile_id += 1;
            pixel_y -= 8;
        }
    }

    // Object tiles use same format as BG tiles, but are in tile blocks 0 and 1 located at $8000-8FFF and have unsigned numbering.
    const uint8_t *tile_data = &vram[(uint16_t)tile_id * 16];

    return decode_tile_pixel(tile_data, pixel_y, pixel_x);
}


/*

=====-- Main Pixel Generation --=====

This ties together the Background tiles, the Window Tiles, the OAM Objects (Sprites)

Calls each separate function to find, decode, extract of the pixels, and add them to the frame_buffer.

This adds each pixel, by the current scanline.

- Note this function is being called from the PPU, when the LY (SCANLINE) advances.
(This is printing each line at a time)

*/


void gen_pixel_line(PPU *ppu, const uint8_t *vram) {
    uint8_t lcdc = ppu->lcdc;

    uint8_t bg_enabled = (lcdc & (1u << 0)) ? 1 : 0;
    uint8_t window_enabled = (lcdc & (1u << 5)) ? 1 : 0;

    // OBJ/ Sprite:
    uint8_t obj_enabled = (lcdc & (1u << 1)) ? 1 : 0;
    uint8_t obj_height = (lcdc & (1u << 2)) ? 16 : 8;

    // The Objects for this scanline. (Max only 10 allowed)
    OAM_Entry scanl_objects[10];

    // The Y Location inside the BG 256x256 layer.
    uint8_t screen_y = ppu->ly;

    // Total OAM objects found for this Y scanline.
    uint8_t object_count = add_scnl_objects(ppu, scanl_objects, obj_height, screen_y);

    for (uint8_t screen_x = 0; screen_x < GB_LCD_WIDTH; screen_x ++) {
        uint8_t merged_color_id = 0x00;
        uint8_t merged_palette = 0x00;
        uint8_t obj_palette = 0x00;
        uint8_t bg_color_id = 0x00;
        uint8_t obj_color_id = 0x00;
        uint8_t obj_priority = 0x00;
        uint8_t obj_found = 0;


        // Window / BG (Background):
        if (window_enabled &&
            screen_y >= ppu->wy &&
            screen_x >= (ppu->wx - 7)) {    // Risk of roll-over if wx = 0.
            bg_color_id = get_win_pixel(ppu, vram, screen_y, screen_x);
        } else {
            bg_color_id = get_bg_pixel(ppu, vram, screen_y, screen_x);
        }

        // OAM Objects:
        for (uint8_t iobj = 0; iobj < object_count; iobj++ ){
            if (obj_enabled) {
                OAM_Entry obj = scanl_objects[iobj];

                int16_t obj_x = (int16_t)obj.x -8;
                if ((int16_t)screen_x < obj_x || (int16_t)screen_x >= obj_x +8) {
                    continue;
                }

                obj_color_id = get_obj_pixel(ppu, vram, &obj, screen_y, screen_x, obj_height);
                if (obj_color_id == 0) {
                    continue;   // 0 = transparent
                }

                // Is object priority bit set?
                obj_priority = (obj.attributes & (1u << 7)) ? 1 : 0;

                // DMG palette select:
                obj_palette = (obj.attributes & (1u << 4)) ? ppu->obp1 : ppu->obp0;

                obj_found = 1;
                break;
            } else{
                continue;
            }
        }

        if (obj_found) {
            if (obj_priority && bg_color_id !=0) {
                merged_color_id = bg_color_id;
                merged_palette = ppu->bgp;
            } else {
                merged_color_id = obj_color_id;
                merged_palette = obj_palette;
            }
        } else {
            merged_color_id = bg_color_id;
            merged_palette = ppu->bgp;
        }


        ppu->vp_rgba_buffer[((uint16_t) screen_y * GB_LCD_WIDTH) + screen_x] = decode_gray_from_palette(0, merged_color_id);
        // ALSO NOTE: 0,1,2,3 colorid for oam 0 = transparent.
    }
}