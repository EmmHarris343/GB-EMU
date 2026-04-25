
#include "oam.h"
#include "../gb.h"
#include "ppu.h"

/// TODO: Depending on how much code is in here. It might be better to move it to PPU.



// Read / Write OAM functions (OAM, is I think like sprites and stuff)
uint8_t oam_read(GB *gb, uint16_t addr) {
    return 0xFF;
}

void oam_write(GB *gb, uint16_t addr, uint8_t val) {

}


uint16_t sprite(GB *gb, uint16_t n) {
    uint16_t sprite_oam_index = n * 4;

    // Each byte of the sprite is:
    // byte 0: Y pos
    // byte 1: X position
    // Byte 2: Tile ID
    // Byte 3: Flags/ attributes

    // Sprite locations as:
    /*
        screen_y = oam_y - 16
        screen_x = oam_x - 8

        So if OAM says Y = 16 x = 8 for sprite location. It's actually in:
        screen_y = 0;
        screen_x = 0;

        This is to account for sprites exisintg partially off-screen
    */

    OAMSprite oam_s;
    oam_s.flags = 0;    // The Flags is usually called the OBJ attributes.
    // With each byte representing details.
    /*
        Bit 7: BG/window priority
        Bit 6: Y flip
        Bit 5: X flip
        Bit 4: Palette number
        Bit 3-0 used for GBC ignored on DMG.
    */

    // The Palette number/ Bit means:
    // 0 = OBP0 | 1 = OBP1

    // The ppu/ sprite height.
    // So code like uint8_t sprite_height = (ppu->lcdc & (1u << 2)) ? 16 : 8

    // So sprites are either 8x8 or 8x16.
    // For 8x16 tile_id works different. The gameboy ignores bit 0.


    // Sprites always use:  0x8000 - 0x8FFF in VRAM.
    // It never uses offsets like BG / window does for other banks.

    // Sprites are basically going to be:
    // uint16_t tile_offset = tile_id * 16;




    return 0x00;
}

/*
Each sprite is is stored in OAM sprite data/ registers. With an index of:

n = sprite
n*4 = sprites.

40 Sprites *4 bytes each = 160 Bytes

*/