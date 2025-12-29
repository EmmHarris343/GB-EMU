
#include "mmu.h"
#include "mmu_interface.h"

#include <stdlib.h>

#include "logger.h"

// Using 1, 16-bit Address space. It can point to 65536 memory locations


/*
MMU is:
A series of Pointers, Configurations, and Rules to direct ANY traffic 
Directs data from: 0000 to FFFF


MMU is/does NOT:
Does not store any DATA, 
Does not store current ROM or RAM Banks
Does NOT control RAM, HRAM, External Ram or Timer.

*/

bus_tag_t bus_loc;

uint8_t memory_map[M_MAP_size];


static mmu_map_entry *mmu_map = NULL;
static int mmu_map_size = 0;

void mmu_init(mmu_map_entry *map, int num_entries) {
    mmu_map = map;
    mmu_map_size = num_entries;
}

uint8_t mmu_read(uint16_t addr) {
    //printf(":MMU: Read 0x%04X\n", addr);
    uint8_t read_8bit_val = 0x00;
    if (mmu_map == NULL) {
        printf("ERROR: mmu_map is null!\n");
        exit(1);
    }
    for (int i = 0; i < mmu_map_size; i++) {
        if (addr >= mmu_map[i].start && addr <= mmu_map[i].end) {       // Changed >= is this right?
            read_8bit_val = mmu_map[i].read(addr);
            //printf("What is MMU_map[i] value? %d", mmu_map[i]);
            trace_mmu_read(addr, read_8bit_val, i, (uint8_t)mmu_map[i].tag);
            return read_8bit_val;
        }
    }

    //trace_mmu_read(addr, 0xFF, (uint8_t)BUS_UNMAPPED);
    return read_8bit_val;
}

void mmu_write(uint16_t addr, uint8_t write_val){
    //printf(":MMU: Write to memory Space: %04X, Value: %02X\n", addr, write_val);
    for (int i = 0; i < mmu_map_size; i++) {
        if (addr >= mmu_map[i].start && addr <= mmu_map[i].end) {       // Changed >= is this right?
            mmu_map[i].write(addr, write_val);
            trace_mmu_write(addr, write_val, i, (uint8_t)mmu_map[i].tag);
        }
    }
}

void mmu_debugger(uint16_t addr) {
    printf("::: NOTICE ::: MMU DEBUGGER\n");
    printf(":MMU: Addr %04X\n", addr);
    printf(":MMU: map size = %d\n", mmu_map_size);

    if (mmu_map == NULL) {
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



// void test_bank_switch() {
//     write_intercept(0x2044, 0x01A);     // 0x01A => 26 | SWITCH to bank 26.

// }