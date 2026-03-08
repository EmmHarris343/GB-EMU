#include "loc_ram.h"
#include "gb.h"

static uint8_t HRAM[HRAM_SIZE];
static uint8_t WRAM[WRAM_SIZE];
static uint8_t ERAM[ECHRAM_SIZE];



int loc_ram_init(GB *gb) {
    printf("Initialize ERAM, WRAM, HRAM..\n");

    // Set all the HRAM, WRAM, ERAM to completely blank values.
    memset(HRAM, 0x00, HRAM_SIZE);
    memset(WRAM, 0x00, WRAM_SIZE);
    memset(ERAM, 0x00, ECHRAM_SIZE);

    // memset(HRAM, 0x76, HRAM_SIZE);  // Make all HRAM data HALT OPCODES <-- don't do that. causing major problems.
    // memset(WRAM, 0, WRAM_SIZE);
    // memset(ERAM, 0, ERAM_SIZE); // This is mostly a route area. Technically NOTHIGN should be written here. If it is, I believe the gameboy would freeze up normally

    printf("Done.\n");

    return 0;
}


// RAM Functions

// WRAM (Working Ram)
uint8_t loc_wram_read(GB *gb, uint16_t addr) {
    //printf(":loc_ram: Hit WRAM Read!\n");
    if (addr < 0xC000 || addr > 0xCFFF) {
        printf("loc_ram: WRAM invalid Read! -> Addr:0x%04X\n", addr);
        // Invalid
        return 0xFF;
    }
    return WRAM[addr - 0xC000];
}
void loc_wram_write(GB *gb, uint16_t addr, uint8_t val) {
    printf(":loc_ram: Hit WRAM WRITE! -> Addr:0x%04X\n", addr);
    if (addr < 0xC000 || addr > 0xDFFF) {
        // Invalid
        printf("loc_ram: WRAM invalid Write!\n");
        return;
    }
    WRAM[addr - 0xC000] = val;
}

// EchRAM (ECHO RAM) This is a mirror of the WRAM.
uint8_t loc_echram_read(GB *gb, uint16_t addr) {
    printf(":loc_ram: Hit EchRAM Read! -> Addr:0x%04X. CalcAddr: 0x%04X\n", addr, addr - 0x2000);
    return loc_wram_read(gb, addr - 0x2000);    // is a mirror of wram.
}
void loc_echram_write(GB *gb, uint16_t addr, uint8_t val) {
    printf(":loc_ram: Hit EchRAM Write! -> Addr:0x%04X. CalcAddr: 0x%04X\n", addr, addr - 0x2000);
    loc_wram_write(gb, addr - 0x2000, val);     // is a mirror of wram.
}

// HRAM
uint8_t loc_hram_read(GB *gb, uint16_t addr) {
    printf(":loc_ram: Hit HRAM Read! -> Addr:0x%04X\n", addr);
    if (addr < 0xFF80 || addr > 0xFFFE) {
        // Invalid
        printf("loc_ram: HRAM invalid Read, Returning 0xFF!\n");
        return 0xFF;
    }
    return HRAM[addr - 0xFF80];

}
void loc_hram_write(GB *gb, uint16_t addr, uint8_t val) {
    printf(":loc_ram: Hit HRAM WRITE! -> Addr:0x%04X Val:0x%02X\n", addr, val);
    if (addr < 0xFF80 || addr > 0xFFFE) {
        // Invalid
        printf("loc_ram: HRAM invalid Write\n");
        return;
    }
    HRAM[addr - 0xFF80] = val;
}