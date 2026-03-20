#include "loc_ram.h"
#include "gb.h"

int loc_ram_init(GB *gb) {
    printf("Initialize WRAM, HRAM.\n");

    // Set the HRAM, WRAM to completely blank 0x00 values.
    memset(gb->memory.hram, 0x00, sizeof(gb->memory.hram));
    memset(gb->memory.wram, 0x00, sizeof(gb->memory.wram));

    printf("Done.\n");
    return 0;
}

// RAM Functions

// WRAM (Working Ram)
uint8_t loc_wram_read(GB *gb, uint16_t addr) {

    //printf(":loc_ram: Hit WRAM Read!\n");
    if (addr >= 0xC000 && addr <= 0xDFFF) { // Allowed range:
        uint16_t offset = addr - 0xC000;
        return gb->memory.wram[offset];
    }
    printf("loc_ram: WRAM invalid Read! -> Addr:0x%04X. Returning 0xFF.\n", addr);
    return 0xFF;
}
void loc_wram_write(GB *gb, uint16_t addr, uint8_t write_val) {
    if (addr >= 0xC000 && addr <= 0xDFFF) { // Allowed range:
        uint16_t offset = addr - 0xC000;
        gb->memory.wram[offset] = write_val;
        return;
    }
    printf("loc_ram: WRAM invalid Write! -> Addr: 0x%04X. WriteVal: 0x%02X\n", addr, write_val);
    return;
}

// EchRAM (ECHO RAM) This is a mirror of the WRAM.
uint8_t loc_echram_read(GB *gb, uint16_t addr) {
    return loc_wram_read(gb, addr - 0x2000);    // is a mirror of wram.
}
void loc_echram_write(GB *gb, uint16_t addr, uint8_t write_val) {
    loc_wram_write(gb, addr - 0x2000, write_val);     // is a mirror of wram.
}

// HRAM
uint8_t loc_hram_read(GB *gb, uint16_t addr) {
    if (addr >= 0xFF80 && addr <= 0xFFFE) {   // Allowed range:
        uint16_t offset = addr - 0xFF80;
        return gb->memory.hram[offset];
    }
    printf("loc_ram: HRAM invalid Read! -> Addr: 0x%04X. Returning 0xFF.\n", addr);
    return 0xFF;
}
void loc_hram_write(GB *gb, uint16_t addr, uint8_t write_val) {
    if (addr >= 0xFF80 && addr <= 0xFFFE) { // Allowed Range:
        uint16_t offset = addr - 0xFF80;
        gb->memory.hram[offset] = write_val;
        return;
    }
    printf("loc_ram: HRAM invalid Write! -> Addr: 0x%04X. WriteVal: 0x%02X\n", addr, write_val);
    return;
}