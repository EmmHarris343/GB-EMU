#include "ppu.h"
static uint8_t VRAM[VRAM_SIZE];




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
