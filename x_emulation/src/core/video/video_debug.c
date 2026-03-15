#include "video_debug.h"
#include "../ppu/ppu.h"

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