// lc_ram (Local RAM)
// HRAM
// WRAM


#include "loc_ram.h"
static uint8_t HRAM[HRAM_SIZE];
static uint8_t WRAM[WRAM_SIZE];
static uint8_t ERAM[ERAM_SIZE];



int init_loc_ram() {
    printf("Initialize ERAM, WRAM, HRAM..\n");
    memset(HRAM, 0, HRAM_SIZE);
    memset(ERAM, 0, ERAM_SIZE);
    memset(WRAM, 0, WRAM_SIZE);
    

    return 0;
}


// RAM Functions

// WRAM
uint8_t loc_wram_read(uint16_t addr) {
    printf(":loc_ram: Hit WRAM Read!\n");
    return 0xFF;

}
void loc_wram_write(uint16_t addr, uint8_t val) {

}

// ERAM
uint8_t loc_eram_read(uint16_t addr) {
    printf(":loc_ram: Hit ERAM Read!\n");
    return 0xFF;

}
void loc_eram_write(uint16_t addr, uint8_t val) {

}

// HRAM
uint8_t loc_hram_read(uint16_t addr) {
    printf(":loc_ram: Hit HRAM Read!\n");
    if (addr < 0xFF80 || addr > 0xFFFE) {
        // Invalid
        return 0xFF;
    }
    return HRAM[addr - 0xFF80];

}
void loc_hram_write(uint16_t addr, uint8_t val) {
    printf(":loc_ram: Hit HRAM WRITE!\n");
    if (addr < 0xFF80 || addr > 0xFFFE) {
        // Invalid
        return;
    }
    HRAM[addr - 0xFF80] = val;
}