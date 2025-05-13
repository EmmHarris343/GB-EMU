#include "e_ctrl.h"

#include "cart.h"
#include "loc_ram.h"

#include "cpu.h"
#include "ppu.h"
#include "oam.h"
#include "apu.h"
// #include "mmu.h"
#include "mmu_interface.h"

#include "logger.h"


#include <unistd.h>

#ifdef ENABLE_TESTS
// This is for The build flag.

#include "../test/cpu_test.h"

#endif

// Testing, because don't have any build flag set.
#include "../test/cpu_test.h"

extern Cartridge cart;
extern FILE *debug_dump_file;

void e_mmu_init(void) {
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
        // Unused: 0xFEA0 ... 0xFEFF (prohibited)
    };
    const size_t mmu_map_size = sizeof(mmu_map) / sizeof(mmu_map_entry);
    //mmu_init(mmu_map, sizeof(mmu_map) / sizeof(mmu_map_entry));
    mmu_init(mmu_map, mmu_map_size);
}



void dump_hram_test() {
    printf("Printing what's in WRAM..\n");
    uint8_t wram_val = 0x00;
    uint16_t wram_location = 0xC000;
    for (int w = 0; w < 16; w++) {
        wram_val = external_read(wram_location);
        printf("ADDR: 0x%04X | 0x%02X | W%d\n", wram_location, wram_val, w);
        wram_location ++;
    }
}

int test_sequence() {
    printf(":E_CTRL: -- TEST MODE -- Startup Beginning\n");

    // Set to cartridge settings to MBC 1, and Mock a ROM file.
    if (init_cart_test_mode() != 0) {
        fprintf(stderr, "Error Initializing Mock ROM Config (Cart.c)\n");
        return -1;        
    }

    // Initialize some settings:
    if (init_loc_ram() != 0) {
        fprintf(stderr, "Error Initializing LOC RAM:\n");
        return -1;
    }

    const char *log_file = "../log/debug_log.txt";
    if (logging_init(log_file) != 0) {
        fprintf(stderr, "Error Initializing DEBUG File:\n");
        return -1;
    }

    // Setup the MMU memory Map.
    e_mmu_init();

    instruction_test();
    return 0; // Pass all good.
}


int startup_sequence() {
    printf(":E_CTRL: Startup Sequence Beginning\n");

    const char *rom_file = "../../rom/pkmn_red.gb";
    //const char *rom_file = "../rom/cpu-individual/07-jr,jp,call,ret,rst.gb";
    
    // 06-ld_r,r
    printf("NOTE: Using rom file: %s\n\n", rom_file);


    if (load_headers(rom_file) != 0) {
        fprintf(stderr, "Error Loading Headers:\n");
        return -1;
    }

    if (decode_cart_features() != 0) {
        fprintf(stderr, "Error Decoding Cartrige Features, from Loaded Headers\n");
        return -1;
    }

    if (load_cartridge(rom_file) != 0) {
        fprintf(stderr, "Error Loading ROM file / Cartridge:\n");
        return -1;
    }
    
    if (initialize_cartridge() != 0) {
        fprintf(stderr, "Error Initializing Cartridge Settings:\n");
        return -1;
    }
    
    if (init_loc_ram() != 0) {
        fprintf(stderr, "Error Initializing LOC RAM:\n");
        return -1;
    }

    const char *log_file = "../log/debug_log.txt";
    if (logging_init(log_file) != 0) {
        fprintf(stderr, "Error Initializing DEBUG File:\n");
        return -1;
    }
    
    

    

    printf(":DEBUG: => ROM_RAW: Cart_type: 0x%02X ROM Size: 0x%02X RAM Size: 0x%02X\n", cart.headers.cart_type_code, cart.headers.rom_size_code, cart.headers.ram_size_code);

   
    sleep(2);
    // Setup the MMU memory Map.
    e_mmu_init();

    // Pass ROM Entry point INTO the CPU module.
    uint8_t *rom_entry = cart.headers.entry_point;
    
    // Initialize the CPU, (To default setting, pass the Roms Entry Point.)
    cpu_init(rom_entry);

    /// TODO: START CPU Emulation!
    //int max_steps = 86;       // This will complete the random ROM test.

    int max_steps = 8000;
    run_cpu(max_steps);

    dump_hram_test();


    //test_step_instruction();


    // Noting left to do, Report success if reached here.
    return 0;
}

void reset_sequence() {

}