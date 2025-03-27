#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..

#include "e_core.h"
#include "cart.h"


extern Cartridge cartridge;             // Global instance of this, so it can be passed around (Declared only once)


/*
Comment / Note taking note.. (Ironic yes.)

1. Is this for me to figure out what I'm doing? (thinking aloud)
    OR
2. Is this so others understand my code?

1: Yes -> Write in obsidian..
2: Yes -> Write in Code

*/








int main() {
    const char *rom_file = "../rom/pkmn_red.gb";    
    printf("NOTE: Using rom file: %s\n\n", rom_file);

    // Need to get th ROMs header first. To know how big the entire Rom file is.
    //get_RomHeader(rom_file);
    parse_cart_header(rom_file, &cartridge);     // Loads the header, reads the data, parses each setting, sets easy to use flags for each header.

    decode_cart_features(&cartridge);
    
    // Check Rom Data
    // Raw Codes:
    printf("::ROM_RAW:: Cart_type: 0x%02X ROM Size: 0x%02X RAM Size: 0x%02X\n", cartridge.header_config.cart_type_code, cartridge.header_config.rom_size_code, cartridge.header_config.ram_size_code);

    // Cart settings.
    printf("::ROM:: \n| MBC_type 0x%02X | ROM Size: 0x%02X | RAM Size: 0x%02X | ROM BANKS: %d\n", cartridge.config.mbc_type, cartridge.config.rom_size, cartridge.config.ram_size, cartridge.config.rom_bank_count);


    configure_mbc(&cartridge);  // Load Cart.c's Configure MBC function.

    // Load the entire Rom into memory. (Deal with banks after, if any)



    uint8_t test_8bit = 0xA;

    // Simple test:
    if (test_8bit == 0x0A) {
        printf("Test is true\n");
    }

    return 0;
}