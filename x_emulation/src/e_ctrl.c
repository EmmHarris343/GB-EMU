#include <unistd.h>

#include "e_ctrl.h"
#include "cart.h"
#include "cart_types.h"
#include "loc_ram.h"
#include "cpu.h"
#include "gb.h"
#include "ppu.h"

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



/// TODO: Remove e_ctrl and combine with e_core.
/// WHY: this basically does the same task, it just exposes functions to e_core, to execute functions in other files..
// Realistically. it's kind of redundant.



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



// Bad name, this is is more of a "Init GB, and begin execution"
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

    int max_steps = 150000;
    gb_run_steps(&gb, max_steps);

    return 0;
}

// Bad name, this is is more of a "Init GB, and begin execution"
int startup_seq_bytime() {
    printf(":E_CTRL: Exec Time Interval - Beginning\n");

    // GB 'core', encapsulates everything from CPU, to APU, to Cycles/Time.
    GB gb;

    const char *rom_file = "../../rom/pkmn_red.gb";
    printf("NOTE: Using rom file: %s\n\n", rom_file);

    // Initialize the GB 'core',
    if (gb_init(&gb) != 0) {
        fprintf(stderr, "unable to Initializing Cartridge Error:\n");
        return -1;
    }
    if (init_log_files() != 0) {
        printf("Unable to Initializing log files.");
        return -1;
    }
    printf(":DEBUG: => ROM_RAW: Cart_type: 0x%02X ROM Size: 0x%02X RAM Size: 0x%02X\n", headers.cart_type_code, headers.rom_size_code, headers.ram_size_code);

    sleep(2);

    uint64_t max_time = 20000; // In MS. 8000 MS = 1 second.
    gb_run_time(&gb, max_time);

    return 0;
}


int test_sequence() {
    printf(":E_CTRL: -- TEST MODE -- Startup Beginning\n");

    printf("Closing Emulator ... bye\n");
    return 0; // Pass all good.
}


void reset_sequence() {

}