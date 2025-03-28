#include "e_ctrl.h"

#include "rom_fetch.h"
#include "cart.h"
#include "loc_ram.h"

//#include "cpu.h"
#include "ppu.h"
#include "oam.h"
#include "apu.h"
#include "mmu.h"
#include "mmu_interface.h"


extern Cartridge cartridge; 


void e_int(void) {
    mmu_map_entry map[] = {
        {0x0000, 0x7FFF, cart_read, cart_write },           // Read ROM Data, Intercept WRITE functions
        {0xA000, 0xBFFF, cart_ram_read, cart_ram_write },   // External (Cart) RAM
        {0xC000, 0xCFFF, loc_wram_read, loc_wram_write },   // Working RAM
        {0xE000, 0xFDFF, loc_eram_read, loc_eram_write },   // Echo RAM
        {0xFF80, 0xFFFE, loc_hram_read, loc_hram_write },   // High RAM (Fast Ram)
        {0xFE00, 0xFE9F, oam_read, oam_write },
        {0x8000, 0x9FFF, ppu_read, ppu_write },
        {0xFF10, 0xFF7F, apu_read, apu_write }
        // OTHER:
        // 0xD000 ... 0xDFFF: (4KiB WRAM, Extra bank?)
        // 0xFF00 ... 0xFF7F: I/O Registers?
        // 0xFFFF: Interupt space?
        // Unused: 0xFEA0 ... 0xFEFF (prohibited )
    };
    mmu_init(map, sizeof(map) / sizeof(mmu_map_entry));
}


// void e_run(void) {
//     while (1) {
//         // cpu_step();
//         // ppu_step();
//         // apu_step();
//     }
// }

void startup_sequence() {
    printf(":E_CTRL: Startup Sequence Beginning\n");

    size_t expected_rom_size = cartridge.config.rom_size;

    const char *rom_file = "../rom/pkmn_red.gb";    
    printf("NOTE: Using rom file: %s\n\n", rom_file);

    // Need to get th ROMs header first. To know how big the entire Rom file is.
    //get_RomHeader(rom_file);
    parse_cart_header(rom_file, &cartridge);     // Loads the header, reads the data, parses each setting, sets easy to use flags for each header.

    decode_cart_features(&cartridge);


    configure_mbc(&cartridge);  // Load Cart.c's Configure MBC function.

    // Load the entire Rom into memory. (Deal with banks after, if any)

    load_entire_rom(rom_file, cartridge.config.rom_size);





    //  test_bank_switch();     THIS Goes into mmu


    printf(":DEBUG: => ROM_RAW: Cart_type: 0x%02X ROM Size: 0x%02X RAM Size: 0x%02X\n", cartridge.header_config.cart_type_code, cartridge.header_config.rom_size_code, cartridge.header_config.ram_size_code);


    printf("Rom Bank? %02X\n ", cartridge.cart_res.cur_ROM_BANK);
}


void reset_sequence() {
    
}