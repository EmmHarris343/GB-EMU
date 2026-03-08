#include <unistd.h>

#include "e_ctrl.h"
#include "cart.h"
#include "cart_types.h"
#include "loc_ram.h"
#include "cpu.h"
#include "gb.h"

#include "logger.h"




#ifdef ENABLE_TESTS
// This is for The build flag.

#include "../test/cpu_test.h"

#endif

// Testing, because don't have any build flag set.
//#include "../test_cpu/cpu_test.h"

extern Headers headers;

extern FILE *debug_dump_file;
extern FILE *cpu_trace_file;
extern FILE *trace_log_file;
extern FILE *cart_mbc_log_file;






// void dump_hram_test() {
//     printf("Printing what's in WRAM..\n");
//     uint8_t wram_val = 0x00;
//     uint16_t wram_location = 0xC000;
//     for (int w = 0; w < 16; w++) {
//         wram_val = external_read(wram_location);
//         printf("ADDR: 0x%04X | 0x%02X | W%d\n", wram_location, wram_val, w);
//         wram_location ++;
//     }
// }

int init_log_files() {
    const char *log_file = "../log/debug_log.txt";
    if (logging_init(log_file) != 0) {
        fprintf(stderr, "Error Initializing DEBUG log File:\n");
        return -1;
    }
    const char *cpu_trc_logfile = "../log/cpu_trace_log.txt";
    if (cpu_trace_init(cpu_trc_logfile) != 0) {
        fprintf(stderr, "Error Initializing CPU Trace log File:\n");
        return -1;
    }
    const char *trace_log_file = "../log/trace_log.txt";
    if (trace_log_init(trace_log_file) != 0) {
        fprintf(stderr, "Error Initializing Trace log File:\n");
        return -1;
    }
    const char *cart_mbc_log_file = "../log/cart_mbc_log.txt";
    if (cart_mbc_log_init(cart_mbc_log_file) != 0) {
        fprintf(stderr, "Error Initializing Trace log File:\n");
        return -1;
    }
    return 0;
}




int startup_sequence() {
    printf(":E_CTRL: Startup Sequence Beginning\n");
    GB gb;

    // Initialize the GB at the "machine" level.
    if (gb_init(&gb) != 0) {
        fprintf(stderr, "unable to Initializing Cartridge Error:\n");
        return -1;
    }
    if (init_log_files() != 0) {
        printf("Unable to Initializing log files.");
        return -1;
    }

    printf(":DEBUG: => ROM_RAW: Cart_type: 0x%02X ROM Size: 0x%02X RAM Size: 0x%02X\n", headers.cart_type_code, headers.rom_size_code, headers.ram_size_code);
    sleep(2);   // Sleep is just so the initial startup can be readable.

    int max_steps = 300;
    gb_run_steps(&gb, max_steps);

    return 0;
}


int startup_seq_bytime() {
    printf(":E_CTRL: Exec Time Interval - Beginning\n");

    GB gb;

    const char *rom_file = "../../rom/pkmn_red.gb";
    printf("NOTE: Using rom file: %s\n\n", rom_file);

    // if (initialize_cartridge(rom_file) != 0) {
    //     fprintf(stderr, "Error Initializing Cartridge Settings:\n");
    //     return -1;
    // }
    // if (init_loc_ram(&gb) != 0) {
    //     fprintf(stderr, "Error Initializing LOC RAM:\n");
    //     return -1;
    // }
    if (init_log_files() != 0) {
        printf("Failure Initializing log files");
        return -1;
    }

    // --------------------------------------------



    printf(":DEBUG: => ROM_RAW: Cart_type: 0x%02X ROM Size: 0x%02X RAM Size: 0x%02X\n", headers.cart_type_code, headers.rom_size_code, headers.ram_size_code);


    sleep(2);
    // Setup the MMU memory Map.
    //e_mmu_init();

    // Initialize CPU to default state
    // cpu_init(&gb);

    /// TODO: START CPU (Timed limited) Emulation!
    // uint64_t max_time = 20000; // In MS. 8000 MS = 1 second.
    // run_cpu_bytime(&gb, max_time);



    // Noting left to do, Report success if reached here.
    return 0;
}


int test_sequence() {
    printf(":E_CTRL: -- TEST MODE -- Startup Beginning\n");

    GB gb;

    // Set to cartridge settings to MBC 1, and Mock a ROM file.
    // if (init_cart_test_mode() != 0) {
    //     fprintf(stderr, "Error Initializing Mock ROM Config (Cart.c)\n");
    //     return -1;
    // }
    // if (init_loc_ram(&gb) != 0) {
    //     fprintf(stderr, "Error Initializing LOC RAM:\n");
    //     return -1;
    // }
    // const char *log_file = "../log/debug_log.txt";
    // if (logging_init(log_file) != 0) {
    //     fprintf(stderr, "Error Initializing DEBUG File:\n");
    //     return -1;
    // }

    // Setup the MMU memory Map.
    //e_mmu_init();

    //instruction_test();
    //unit_test_instruction();
    // entry_test_case();


    printf("Closing Emulator ... bye\n");
    return 0; // Pass all good.
}


void reset_sequence() {

}