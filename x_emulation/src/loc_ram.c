// lc_ram (Local RAM)
// HRAM
// WRAM


#include "loc_ram.h"
static uint8_t HRAM[HRAM_SIZE];
static uint8_t WRAM[WRAM_SIZE];
static uint8_t ERAM[ERAM_SIZE];



int init_loc_ram() {
    printf("Initialize ERAM, WRAM, HRAM..\n");
    memset(HRAM, 0x76, HRAM_SIZE);  // Make all HRAM data HALT OPCODES
    memset(WRAM, 0, WRAM_SIZE);
    memset(ERAM, 0, ERAM_SIZE); // This is mostly a route area. Technically NOTHIGN should be written here. If it is, I believe the gameboy would freeze up normally
    
    printf("Done.\n");

    // for (int i = 0; i < HRAM_SIZE; i++) {
    //     printf("HRAM[%02X] = %02X\n", i + 0xFF80, HRAM[i]);
    // }

    return 0;
}


// RAM Functions

// WRAM
uint8_t loc_wram_read(uint16_t addr) {
    //printf(":loc_ram: Hit WRAM Read!\n");
    if (addr < 0xC000 || addr > 0xCFFF) {
        printf("loc_ram: WRAM invalid Read!\n");
        // Invalid
        return 0xFF;
    }
    return WRAM[addr - 0xC000];
}
void loc_wram_write(uint16_t addr, uint8_t val) {
    printf(":loc_ram: Hit WRAM WRITE!\n");
    if (addr < 0xC000 || addr > 0xCFFF) {
        // Invalid
        return;
    }
    WRAM[addr - 0xC000] = val;
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
    //printf(":loc_ram: Hit HRAM WRITE!\n");
    if (addr < 0xFF80 || addr > 0xFFFE) {
        // Invalid
        return;
    }
    HRAM[addr - 0xFF80] = val;
}