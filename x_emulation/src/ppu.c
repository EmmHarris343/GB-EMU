#include "ppu.h"
static uint8_t VRAM[VRAM_SIZE];


// Let PPU own it's own values, and time values.
struct ppu_s {
    uint8_t lcdc;
    uint8_t stat;
    uint8_t scy;
    uint8_t scx;
    uint8_t ly;
    uint8_t lyc;
    uint8_t wy;
    uint8_t wx;

    uint16_t line_dots;
    uint8_t mode;
};

struct timer_s {
    uint8_t div;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;

    uint16_t div_counter;
    uint16_t tima_counter;
};




int init_ppu() {
    printf("Initialize ERAM, WRAM, HRAM..\n");
    memset(VRAM, 0, VRAM_SIZE);

    return 0;
}


uint8_t ppu_read(uint16_t addr){
    printf(":ppu: Hit VRAM Read!\n");
    if (addr < 0x8000 || addr > 0x9FFF) {
        // Invalid
        return 0xFF;
    }
    return VRAM[addr - 0x8000];

}
void ppu_write(uint16_t addr, uint8_t val){
    printf(":ppu: Hit VRAM WRITE!\n");
    if (addr < 0x8000 || addr > 0x9FFF) {
        // Invalid
        return;
    }
    VRAM[addr - 0x8000] = val;
}
