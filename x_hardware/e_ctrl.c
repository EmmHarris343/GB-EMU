#include "e_ctrl.h"

#include "rom_fetch.h"
#include "cart.h"
#include "loc_ram.h"

#include "cpu.h"
#include "ppu.h"
#include "oam.h"
#include "apu.h"
#include "mmu.h"
#include "mmu_interface.h"


extern Cartridge cart; 


void e_int(void) {
    static mmu_map_entry mmu_map[] = {
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
    const size_t mmu_map_size = sizeof(mmu_map) / sizeof(mmu_map_entry);
    //mmu_init(mmu_map, sizeof(mmu_map) / sizeof(mmu_map_entry));
    mmu_init(mmu_map, mmu_map_size);
}


// void e_run(void) {
//     while (1) {
//         // cpu_step();
//         // ppu_step();
//         // apu_step();
//     }
// }

int startup_sequence() {
    printf(":E_CTRL: Startup Sequence Beginning\n");

    const char *rom_file = "../rom/pkmn_red.gb";
    printf("NOTE: Using rom file: %s\n\n", rom_file);

    // Need to get th ROMs header first. To know how big the entire Rom file is.
    //get_RomHeader(rom_file);
    //parse_cart_header(rom_file, &cartridge);     // Loads the header, reads the data, parses each setting, sets easy to use flags for each header.
    //decode_cart_features(&cartridge);
    //configure_mbc(&cartridge);  // Load Cart.c's Configure MBC function.

    if (load_headers(rom_file) !=0) {
        fprintf(stderr, "Error Loading Headers:\n");
        return -1;
    }

    if (decode_cart_features() !=0) {
        fprintf(stderr, "Error Decoding Cartrige Features, from Loaded Headers\n");
        return -1;
    }

    if (load_cartridge(rom_file) !=0) {
        fprintf(stderr, "Error Loading ROM file / Cartridge:\n");
        return -1;
    }
    
    if (initialize_cartridge() !=0) {
        fprintf(stderr, "Error Initialize Cartridge Settings:\n");
        return -1;
    }
    

    

    printf(":DEBUG: => ROM_RAW: Cart_type: 0x%02X ROM Size: 0x%02X RAM Size: 0x%02X\n", cart.headers.cart_type_code, cart.headers.rom_size_code, cart.headers.ram_size_code);

    // Load the entire Rom into memory. (Deal with banks after, if any)
    //load_entire_rom(rom_file, cartridge.config.rom_size);
    
    // Split into Banks?
    /// TODO: Add Function/ Code for splitting into seperate ROM Banks
    printf("Rom Bank? %02X\n", cart.resrce.current_rom_bank);
    
    // Setup the MMU memory Map.
    e_int();

    // Pass ROM Entry point INTO the CPU module.
    uint8_t *rom_entry = cart.headers.entry_point;
    
    // Initialize the CPU, (To default setting, pass the Roms Entry Point.)
    cpu_init(rom_entry);


    test_step_instruction();


    /// TODO: START CPU Emulation!
}

void reset_sequence() {

}