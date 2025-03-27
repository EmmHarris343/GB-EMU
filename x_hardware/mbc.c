#include "cart.h"

#include <stdio.h>  // for print / debugging stuff




// MBC Write functions. Matches to the TypeDef inside the cart.h header file





// Untested, but might work. (More of a placeholder, to test the Function pointers actually work)
void mbc1_write(uint16_t addr, uint8_t val) {
    if (addr >= 0x2000 && addr <= 0x3FFF) {
        
        // Change the ROM bank based on the value
        uint8_t current_rom_bank = val & 0x1F; // Only 5 bits used
        if (current_rom_bank == 0) current_rom_bank = 1; // Bank 0 is forbidden
    }
}


void mbc3_write(uint16_t addr, uint8_t val) {
    // MBC3-specific behavior
}