#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..

#include <e_core.h>




// Global Memory:
uint8_t WRAM[WRAM_size];
uint8_t HRAM[HRAM_size];
uint8_t VRAM[VRAM_size];

uint8_t EXT_RAM;                // Dynamic Memory

uint8_t memory_map[M_MAP_size];

// Some CPU Registers:
uint16_t PC = 0x100;            // Maybe delete this...
uint16_t SP = 0xFFFE;           // Maybe.. delete this..

