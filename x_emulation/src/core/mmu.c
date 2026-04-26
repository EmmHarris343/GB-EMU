#include <stdlib.h>

#include "../debug/logger.h"

#include "gb.h"
#include "mmu.h"
#include "cart/cart.h"
#include "loc_ram.h"
#include "ppu/ppu.h"
#include "io.h"
/*
MMU is:
A series of Pointers, Configurations, and Rules to direct ANY traffic
Directs data from: 0000 to FFFF

MMU does NOT:
Does not store any DATA,
Does not store current ROM or RAM Banks
Does NOT control RAM, HRAM, External Ram or RTC Timer.

*/

// This is for memory space that cannot be used.
uint8_t unusable_read(GB *gb, uint16_t addr) {
    (void)addr;
    return 0xFF;
}
void unusable_write(GB *gb, uint16_t addr, uint8_t val) {
    (void)addr;
    (void)val;
}

/// NOTICE: This mmu map works for now, so keep it as is. But be aware... this is a 'hot' path.
// Because the map requires a for loop to decode the address, it can slow things down.
static MMU_MapRoute mmu_map[] = {
    {0x0000, 0x7FFF, cart_rom_read, cart_rom_write, BUS_ROM },          // Read ROM Data, Intercept WRITE functions
    {0xA000, 0xBFFF, cart_ram_read, cart_ram_write, BUS_ECRAM},         // External Cart RAM (ECRAM) -> The cartriges internal ram (save files)
    {0x8000, 0x9FFF, ppu_vram_read, ppu_vram_write, BUS_VRAM},
    {0xC000, 0xDFFF, loc_wram_read, loc_wram_write, BUS_WRAM },         // Working RAM (range not compatible with CGB)
    {0xE000, 0xFDFF, loc_echram_read, loc_echram_write, BUS_ECHO },     // Echo RAM (mirror of WRAM)
    {0xFE00, 0xFE9F, oam_read, oam_write, BUS_OAM },
    {0xFEA0, 0xFEFF, unusable_read, unusable_write, BUS_UNMAPPED},
    {0xFF00, 0xFF7F, io_read, io_write, BUS_IO},
    {0xFF80, 0xFFFE, loc_hram_read, loc_hram_write, BUS_HRAM },          // High RAM (Fast Ram)
    {0xFF0F, 0xFF0F, interrupt_read, interrupt_write, BUS_IF},           // IF Interrupt
    {0xFFFF, 0xFFFF, interrupt_read, interrupt_write, BUS_IE}           // IE Interrupt
};
static const size_t mmu_map_size = sizeof(mmu_map) / sizeof(MMU_MapRoute);

uint8_t mmu_read(GB *gb, uint16_t addr) {
    uint8_t read_8bit_val = 0x00;
    if (mmu_map_size <= 0) {
        printf("ERROR: mmu_map is emtpy!\n");
        exit(1);
    }
    for (int i = 0; i < mmu_map_size; i++) {
        if (addr >= mmu_map[i].start && addr <= mmu_map[i].end) {
            read_8bit_val = mmu_map[i].read(gb, addr);
            //trace_mmu_read(gb->instruction.opcode, addr, read_8bit_val, i, (uint8_t)mmu_map[i].tag);
            return read_8bit_val;
        }
    }
    return read_8bit_val;
}

void mmu_write(GB *gb, uint16_t addr, uint8_t write_val){
    for (int i = 0; i < mmu_map_size; i++) {
        if (addr >= mmu_map[i].start && addr <= mmu_map[i].end) {
            mmu_map[i].write(gb, addr, write_val);
            if (addr == 0xFF01) {
                uint8_t ch = write_val;
                if (ch >= 0x20 && ch <= 0x7E) {
                    printf("SERIAL: 0x%02X '%c'\n", ch, ch);
                } else if (ch == 0x0A) {
                    printf("SERIAL: 0x%02X '\\n'\n", ch);
                } else {
                    printf("SERIAL: 0x%02X\n", ch);
                }
                if (ch == 0x64) {   // the d in failed.
                    printf("OPCODE=%04X, PC=%04X\n", gb->instruction.opcode, gb->cpu.reg.PC);
                }

            }
            //trace_mmu_write(gb->instruction.opcode, addr, write_val, i, (uint8_t)mmu_map[i].tag);
        }
    }
}

void mmu_debugger(GB *gb, uint16_t addr) {
    printf("::: NOTICE ::: MMU DEBUGGER\n");
    printf(":MMU: Addr %04X\n", addr);
    printf(":MMU: map size = %zu\n", mmu_map_size);

    // THIS ISN'T NEEDED. I'm just tired of removing logger.h every time I disable the mmu trace logging.
    trace_mmu_write(gb->instruction.opcode, addr, 0, 0, 0);

    if (mmu_map_size <= 0) {
        printf("ERROR: mmu_map is null!\n");
        exit(1);
    }


    /// NOTE: Helpful to know exactly what the values are in the MMUMAP:
    for (int i = 0; i < mmu_map_size; i++) {
        printf("mmu_map values START: %04X | END: %04X\n", mmu_map[i].start, mmu_map[i].end);
        //printf("mmu_map values END: %04X\n", mmu_map[i].end);
    }

    for (int i = 0; i < mmu_map_size; i++) {
        if (addr >= mmu_map[i].start && addr <= mmu_map[i].end) {       // Changed >= is this right?
            printf("Landed on MMU_MAP #%d, Between Memory Space: 0x%04X | 0x%04X\n", i, mmu_map[i].start, mmu_map[i].end);
        }
    }
}



// OLD (Just for reference.)
void bus_entry(uint16_t address) {
    printf("Memory going to be directed");

    switch (address){
        case 0x0000 ... 0x3FFF:
            printf("ROM Bank 00 - Fixed Bank");
        case 0x4000 ... 0x7FFF:
            printf("ROM Bank 01-NN - Switchable Bank");
            // Read data from ROM BANK.
            // GOTO -> cart.c
        case 0x8000 ... 0x9FFF:
            printf("8 KiB VRAM");
            // GOTO -> ppu.c
        case 0xA000 ... 0xBFFF:
            printf("8 KiB E-RAM (External)");
            // GOTO -> cart.c
        case 0xC000 ... 0xCFFF:
            printf("4 KiB WRAM (Work Ram)");
            // GOTO -> loc_ram
        case 0xD000 ... 0xDFFF:
            printf("4 KiB WRAM - DMG Extra Bank");
            // GOTO -> loc_ram
        case 0xE000 ... 0xFDFF:
            printf("ECHO RAM - PROHIBITED");
            // GOTO -> loc_ram (OR stay in MMU.)
        case 0xFE00 ... 0xFE9F:
            printf("OAM (Object attribute memory)");
            // GOTO -> OAM
        case 0xFEA0 ... 0xFEFF:
            printf("NOT USABLE - PROHIBITED");
            // GOTO | NONE |
        case 0xFF00 ... 0xFF7F:
            printf("I/O Registers");             // Display, sound, input and such?
            // GOTO ?? IDK
        case 0xFF80 ... 0xFFFE:
            printf("HRAM - High Ram");          // I assume it's like a cpu cache
            // GOTO -> loc_ram
        case 0xFFFF:
            printf("Interupt Space");
            // ?? Interupts do what exactly?
        default:
            printf("Out of range... FAULT");
    }
}