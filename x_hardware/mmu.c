
#include "mmu.h"

// Using 1, 16-bit Address space. It can point to 65536 memory locations


/*
MMU is:
A series of Pointers, Configurations, and Rules to direct ANY traffic 
Directs data from: 0000 to FFFF
Any read, or write, or access functions do no care what it's accessing
The MMU takes Address Spaces, Ranges, Vales. 
Directing all of it to the appropriate sub controls. 


MMU is/does NOT:
Does not store any DATA, 
Does not store current ROM or RAM Banks
Does NOT control RAM, HRAM, External Ram or Timer.

*/



// Question. What actually NEEDS to access the memory? 
// CPU, APU, PPU, Cart.c,   Anything else realistically?
// Maybe core (But just for EASY testing.)



// Global Memory:
uint8_t WRAM[WRAM_size];
uint8_t HRAM[HRAM_size];
uint8_t VRAM[VRAM_size];

uint8_t memory_map[M_MAP_size];





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