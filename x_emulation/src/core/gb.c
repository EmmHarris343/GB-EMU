
#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#define _POSIX_C_SOURCE 200809L // This tells glibc to expose the POSIX.1-2008 APIs

#include "cpu/cpu.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "gb.h"

uint64_t machine_total_cycles;

// BIG NOTE:
// Avoid including "gb.h" inside any header file. Instead do: typedef struct gb_s GB;
// Only include gb.h inside the c file, and only import it if it is absolutely needed.

/*
GB Normal run time/ HZ.

4,194,304 Hz
about 69,905 cycles per frame
about 59.73 frames per second

*/



// PRINT REPORT:
static void debug_print_stats(GB *gb) {
    printf(
        "\n[DBG] frames=%llu cpu_steps=%llu cpu_cycles=%llu "
        "ppu_tick_calls=%llu ppu_cycles_seen=%llu \n"
        "ly_wraps=%llu vblank_entries=%llu\n"
        "LY=%u stat-mode=%u LCDC=%02X... \n",
        (unsigned long long) gb->db_stats.frames,
        (unsigned long long) gb->db_stats.cpu_steps,
        (unsigned long long) gb->db_stats.cpu_cycles,
        (unsigned long long) gb->db_stats.ppu_tick_calls,
        (unsigned long long) gb->db_stats.ppu_cycles_seen,
        (unsigned long long) gb->db_stats.ly_wraps,
        (unsigned long long) gb->db_stats.vblank_entries,
        gb->ppu.ly,
        gb->ppu.stat & 0x03,
        gb->ppu.lcdc
    );
    printf("[DBG] IE=%02X, IF=%02X, IE&IF=%04X, IME=%02X, HALT=%02X\n",
        gb->interrupts.IE,
        gb->interrupts.IF,
        (gb->interrupts.IE & gb->interrupts.IF),
        gb->cpu.state.IME,
        gb->cpu.state.halt
    );
}

// Top-Level "Machine" GB Setup/ Initializer
int gb_init(GB *gb, const char *rom_file) {
    printf("Initializing GB...\n");

    gb->interrupts.IE = 0x00,    // IE at location 0xFFFF = 0x00
    gb->interrupts.IF = 0xE1,     // IF at location 0xFF0F = 0xE1

    gb->panic = 0;
    gb->cpu.state.panic = 0;
    gb->quit = 0;

    memset(&gb->db_stats, 0, sizeof(gb->db_stats));


    printf(":GB: Using rom file: %s\n\n", rom_file);

    if (cpu_init(gb) != 0){
        fprintf(stderr, "Error Initializing GB CPU:\n");
        return -1;
    }
    if (cartridge_init(gb, rom_file) != 0) {
        fprintf(stderr, "Error Initializing GB Cartridge:\n");
        return -1;
    }
    if (loc_ram_init(gb) != 0) {
        fprintf(stderr, "Error Initializing GB Loc_Ram config:\n");
        return -1;
    }
    if (ppu_init(gb) != 0) {
        fprintf(stderr, "Error Initializing GB PPU config:\n");
        return -1;
    }
    if (timer_init(gb) != 0) {
        fprintf(stderr, "Error Initializing GB Timer config:\n");
        return -1;
    }
    if (apu_init(gb) != 0) {
        fprintf(stderr, "Error Initializing GB APU config:\n");
        return -1;
    }

    printf("GB Initialization Complete.\n");

    return 0;
}

// The gb step, advances emulation by one cpu step and adds cycles to gb 'tick'.
uint32_t gb_step(GB *gb) {
    uint32_t cycles;

    cycles = cpu_step(gb);  // T-cycles / clock cycle (Not machine cycles)
    gb_tick(gb, cycles);
    gb->step_count++;

    gb->db_stats.cpu_steps++;
    gb->db_stats.cpu_cycles += cycles;

    return cycles;
}

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



/*

GB Tick => Machine timing dispatcher.

*/
// Tick; update the total_cycles / frame_cycles,
void gb_tick(GB *gb, uint32_t cycles){
    gb->total_cycles += cycles;
    gb->frame_cycles += cycles;

    gb->db_stats.ppu_tick_calls++;
    gb->db_stats.ppu_cycles_seen += cycles;

    timer_tick(gb, &gb->timer, cycles);
    ppu_tick(gb, &gb->ppu, cycles);
}
/*
WAY more advanced timer/ scheduler. Consider for later:
void gb_tick(struct gb_s *gb, uint32_t cycles)
{
    scheduler_advance(&gb->scheduler, gb, cycles);
}
*/


void gb_step_frame(GB *gb, uint64_t next_frame_time_ns) {
    uint32_t frame_cycles = 0;

    if (gb->cpu.state.stop){ // CPU does nothing until interrupt
        gb->cpu.cycle += 4;
        return;
    }

    while (frame_cycles < GB_CYCLES_PER_FRAME && !gb->panic) {
        uint32_t cycles = gb_step(gb);
        frame_cycles += cycles;
    }

    gb->db_stats.frames++;



    if ((gb->db_stats.frames % 60) == 0) {
        debug_print_stats(gb);
    }

    if (gb->panic) {
        printf("GB Panic, Cancelling run..\n");
        // DUMP GB Trace logs
        exit(1);
    }

    next_frame_time_ns += GB_FRAME_NS;
    sleep_until_ns(next_frame_time_ns);
}


// TEST -> The GB run loop. Limited by steps.
int gb_run_steps(GB *gb, int max_steps) {
    printf("GB RUN. Starting Emulation => limit by steps\n");
    int step_count;
    for (step_count = 0; step_count < max_steps; step_count++) {
        if (gb->panic) {    // Include both as it's migrated.
            printf("GB Panic, Cancelling run..\n");
            // DUMP GB Trace logs
            exit(1);
            return -1;
        }
        if (gb->cpu.state.panic) {
            printf("CPU Panic, Cancelling run..\n");
            // DUMP GB Trace logs
            exit(1);
            return -1;
        }
        gb_step(gb);
    }

    printf("Finished GB CPU run. - Limited by steps.\n");

    // POST run report:
    print_instruction_counts();

    printf("GB run finished. Exiting..\n");

    return step_count;
}
// TEST -> The GB run loop. Limited by elasped time.
void gb_run_time(GB *gb, uint64_t max_time) {


    gb_step(gb);
}
// TEST -> The GB run loop. Limited by cycles.
uint64_t gb_run_cycles(GB *gb, uint64_t cycle_budget) {
    uint64_t ran_cycles = 0;

    while (ran_cycles < cycle_budget) {
        if (gb->panic) {    // Include both as it's migrated.
            break;
        }
        if (gb->cpu.state.panic) {
            break;
        }
        uint32_t step_cycles;

        step_cycles = gb_step(gb);
        ran_cycles += step_cycles;
    }

    return ran_cycles;
}
// No longer used. Using gb_step_frame
int gb_run(GB *gb) {
    // This works by a emulating on a per frame setup.

    // WOULD BE BETTER: setup some kind of accumulator.
    // Technically called: time accumulator loop.

    uint64_t next_frame_time_ns = time_now_ns();

    while (!gb->quit) {
        uint32_t frame_cycles = 0;

        while (frame_cycles < GB_CYCLES_PER_FRAME && !gb->panic) {
            uint32_t cycles = gb_step(gb);
            frame_cycles += cycles;
        }

        gb->db_stats.frames++;

        if ((gb->db_stats.frames % 60) == 0) {
            debug_print_stats(gb);
        }

        if (gb->panic) {    // Include both as it's migrated.
            printf("GB Panic, Cancelling run..\n");
            // DUMP GB Trace logs
            exit(1);
            break;
        }

        next_frame_time_ns += GB_FRAME_NS;
        sleep_until_ns(next_frame_time_ns);
    }
    return 0;
}


// Called by PPU/ LCD for interrupt requests. IE: VBLANK etc
void gb_request_interrupt(GB *gb, uint8_t interrupt_bit) {
    // VBLANK = 0, LCD_STAT = 1, TIMER = 2, SERIAL = 3, JOYPAD = 4
    gb->interrupts.IF |= (1u << interrupt_bit);
    //printf("IF REQUEST INTERRUPT! hit. interrupts.IF=%02X\n", gb->interrupts.IF);
}

// Special interupt handling both IE (Interrupt Enable) and IF (Interrupt Flag):
uint8_t ie_read(GB *gb, uint16_t addr) {
    (void)addr;
    uint8_t read_value = gb->interrupts.IE;
    // printf("IE Read hit. PC=%04X , OPCODE=%02X, value=%02X\n",
    //      gb->cpu.reg.PC, gb->instruction.opcode, read_value);
    return read_value;
}
void ie_write(GB *gb, uint16_t addr, uint8_t interrupt_hex) {
    (void)addr;
    printf("IE Write hit. A-Reg=%02X, PC=%04X , OPCODE=%02X, Oprand=%02X, value=%02X\n",
        gb->cpu.reg.A,
        gb->cpu.reg.PC, gb->instruction.opcode, gb->instruction.operand1, interrupt_hex);
    gb->interrupts.IE = interrupt_hex & 0x1F;
}
uint8_t if_read(GB *gb, uint16_t addr) {
    (void)addr;
    return gb->interrupts.IF & 0x1F;

}
void if_write(GB *gb, uint16_t addr, uint8_t interrupt_hex) {
    (void)addr;
    printf("IF Write hit. Interrupt_Hex: %02X\n", interrupt_hex);
    gb->interrupts.IF = interrupt_hex & 0x1F;
}

