#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#define _POSIX_C_SOURCE 200809L // This tells glibc to expose the POSIX.1-2008 APIs


#include <unistd.h>
#include <time.h>


#include "e_ctrl.h"
#include "core/gb.h"
#include "core/ppu/ppu.h"

// The display / test video portion:
//#include <SDL2/SDL.h>
#include "core/video/adapter.h"
#include "host/basic_viewer.h"

#include "debug/logger.h"




#ifdef ENABLE_TESTS
// This is for The build flag.

#include "../test/cpu_test.h"

#endif

extern Headers headers;

extern FILE *debug_dump_file;
extern FILE *cpu_trace_file;
extern FILE *trace_log_file;
extern FILE *cart_mbc_log_file;



/// TODO: Remove e_ctrl and combine with e_core.
/// WHY: this basically does the same task, it just exposes functions to e_core, to execute functions in other files..
// Realistically. it's kind of redundant.

// Linux specific time functions:
static uint64_t time_now_ns(void) {
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ((uint64_t) ts.tv_sec * GB_UL_VAL) + (uint64_t) ts.tv_nsec;
}
static void sleep_until_ns(uint64_t target_ns) {
    struct timespec ts;
    uint64_t now_ns;
    uint64_t remaining_ns;

    for (;;) {  // Creates infinit loop
        now_ns = time_now_ns();
        if (now_ns >= target_ns) {
            break;
        }

        remaining_ns = target_ns - now_ns;

        ts.tv_sec = (time_t)(remaining_ns / GB_UL_VAL);
        ts.tv_nsec = (long) (remaining_ns % GB_UL_VAL);

        clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
    }
}





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


int start_emulation() {
    printf(":E_CTRL: Beginning Emulation\n");
    GB gb;

    // Which rom file to use.
    //const char *rom_file = "../../rom/cpu-individual/07-jr,jp,call,ret,rst.gb"; // MBC1

    //const char *rom_file = "../../rom/cpu-individual/02-interrupts.gb"; // MBC1
    //const char *rom_file = "../../rom/cpu-individual/03-op_sp,hl.gb"; // MBC1
    //const char *rom_file = "../../rom/cpu-individual/07-jr,jp,call,ret,rst.gb"; // MBC1
    //const char *rom_file = "../../rom/cpu-individual/07-jr,jp,call,ret,rst.gb"; // MBC1

    const char *rom_file = "../../rom/zelda_awkng(mbc1).gb"; // MBC1
    //const char *rom_file = "../../rom/pokemon_blue.gb"; // MBC3 DMG GB Game.
    //const char *rom_file = "../../rom/wrio_land_2.gb"; // MBC3 DMG GB Game.
    //const char *rom_file = "../../rom/pkmn_red.gb"; // NOTICE!! pkmn_red is a mbc3 gameboy COLOUR only game!
    //const char *rom_file = "../rom/cpu-individual/07-jr,jp,call,ret,rst.gb";

    // Strickly the video stuff.
    DebugVideoSource video_source;
    BasicViewer viewer;
    // Technically the "running" thingy:
    int running;

    video_init_source(&gb, &video_source);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Failure during the SDL_init.\n");
        return 1;
    }
    if (basic_viewer_init(&viewer, video_source, DEBUG_VIEW_BG_MAP, 2) != 0) {
        SDL_Quit();
        printf("Failure during basic_viewer init.\n");
        return 1;
    }

    // Initialize the GB at the "machine" level.
    if (gb_init(&gb, rom_file) != 0) {
        fprintf(stderr, "unable to Initializing Cartridge Error:\n");
        return -1;
    }
    if (init_log_files() != 0) {
        printf("Unable to Initializing log files.");
        return -1;
    }

    printf(":DEBUG: => ROM_RAW: Cart_type: 0x%02X ROM Size: 0x%02X RAM Size: 0x%02X\n", headers.cart_type_code, headers.rom_size_code, headers.ram_size_code);
    sleep(2);   // Sleep is just so the initial startup can be readable.

    // The main emulation loop. (Moved from the old gb_run(&gb) function.)

    running = 1;

    uint64_t next_frame_time_ns = time_now_ns();

    while (running) {
        SDL_Event event;
        if (gb.panic) {
            break;
        }
        if (gb.quit) {
            break;
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        gb_step_frame(&gb, next_frame_time_ns);

        //build_test_bg_pattern(&gb.ppu, gb.ppu.vram);
        //build_test_pattern(&gb.ppu);
        build_debug_test_bg(&gb.ppu, gb.ppu.vram);

        basic_viewer_present(&viewer);

        SDL_Delay(16);
    }
    basic_viewer_shutdown(&viewer);
    SDL_Quit();
    return 0;
}

// OLD Version... just did the basic emulation no video / decode anything..
/*
int start_emulation() {
    printf(":E_CTRL: Beginning Emulation\n");
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

    gb_run(&gb);

    return 0;
}
*/


// Bad name, DO NOT USE! this is is more of a "Init GB, and begin execution"
int startup_sequence() {
    printf(":E_CTRL: Startup Sequence Beginning\n");
    GB gb;

    const char *rom_file = "../../rom/wrio_land_2.gb"; // MBC3 DMG GB Game.
    //const char *rom_file = "../../rom/pkmn_red.gb"; // NOTICE!! pkmn_red is a mbc3 gameboy COLOUR only game!
    //const char *rom_file = "../rom/cpu-individual/07-jr,jp,call,ret,rst.gb";

    // Initialize the GB at the "machine" level.
    if (gb_init(&gb, rom_file) != 0) {
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

// Bad. DO NOT USE! name, this is is more of a "Init GB, and begin execution"
int startup_seq_bytime() {
    printf(":E_CTRL: Exec Time Interval - Beginning\n");

    // GB 'core', encapsulates everything from CPU, to APU, to Cycles/Time.
    GB gb;

    const char *rom_file = "../../rom/wrio_land_2.gb"; // MBC3 DMG GB Game.
    //const char *rom_file = "../../rom/pkmn_red.gb"; // NOTICE!! pkmn_red is a mbc3 gameboy COLOUR only game!
    //const char *rom_file = "../rom/cpu-individual/07-jr,jp,call,ret,rst.gb";

    // Initialize the GB 'core',
    if (gb_init(&gb, rom_file) != 0) {
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