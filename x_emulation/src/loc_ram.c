// lc_ram (Local RAM)
// HRAM
// WRAM


#include "loc_ram.h"
static uint8_t HRAM[HRAM_SIZE];
static uint8_t WRAM[WRAM_SIZE];
static uint8_t ERAM[ERAM_SIZE];



int init_loc_ram() {
    printf("Initialize ERAM, WRAM, HRAM..\n");

    // Set all the HRAM, WRAM, ERAM to completely blank values.
    memset(HRAM, 0x00, HRAM_SIZE);
    memset(WRAM, 0x00, WRAM_SIZE);
    memset(ERAM, 0x00, ERAM_SIZE);

    // memset(HRAM, 0x76, HRAM_SIZE);  // Make all HRAM data HALT OPCODES
    // memset(WRAM, 0, WRAM_SIZE);
    // memset(ERAM, 0, ERAM_SIZE); // This is mostly a route area. Technically NOTHIGN should be written here. If it is, I believe the gameboy would freeze up normally

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
        printf("loc_ram: WRAM invalid Read! -> Addr:0x%04X\n", addr);
        // Invalid
        return 0xFF;
    }
    return WRAM[addr - 0xC000];
}
void loc_wram_write(uint16_t addr, uint8_t val) {
    printf(":loc_ram: Hit WRAM WRITE! -> Addr:0x%04X\n", addr);
    if (addr < 0xC000 || addr > 0xDFFF) {
        // Invalid
        printf("loc_ram: WRAM invalid Write!\n");
        return;
    }
    WRAM[addr - 0xC000] = val;
}

// EchRAM (ECHO RAM) This is a mirror of the WRAM.
uint8_t loc_echram_read(uint16_t addr) {
    printf(":loc_ram: Hit EchRAM Read! -> Addr:0x%04X. CalcAddr: 0x%04X\n", addr, addr - 0x2000);
    return loc_wram_read(addr - 0x2000);    // is a mirror of wram.
}
void loc_echram_write(uint16_t addr, uint8_t val) {
    printf(":loc_ram: Hit EchRAM Write! -> Addr:0x%04X. CalcAddr: 0x%04X\n", addr, addr - 0x2000);
    loc_wram_write(addr - 0x2000, val);     // is a mirror of wram.
}

// HRAM
uint8_t loc_hram_read(uint16_t addr) {
    printf(":loc_ram: Hit HRAM Read! -> Addr:0x%04X\n", addr);
    if (addr < 0xFF80 || addr > 0xFFFE) {
        // Invalid
        printf("loc_ram: HRAM invalid Read, Returning 0xFF!\n");
        return 0xFF;
    }
    return HRAM[addr - 0xFF80];

}
void loc_hram_write(uint16_t addr, uint8_t val) {
    printf(":loc_ram: Hit HRAM WRITE! -> Addr:0x%04X Val:0x%02X\n", addr, val);
    if (addr < 0xFF80 || addr > 0xFFFE) {
        // Invalid
        printf("loc_ram: HRAM invalid Write\n");
        return;
    }
    HRAM[addr - 0xFF80] = val;
}