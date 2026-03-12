#include "gb.h"
#include <stdlib.h>

uint64_t machine_total_cycles;

// BIG NOTE:
// subsystem headers
// should avoid including gb.h unless absolutely necessary

// subsystem .c files
// can include both their own header and gb.h when needed
// This avoids circular include damage.


// Local subsystem functions:
// void cpu_reset(struct cpu_s *cpu);
// void ppu_reset(struct ppu_s *ppu);
// void timer_reset(struct timer_s *timer);


// Subsystem functions that would need machine-wide access:
// uint32_t cpu_step(struct gb_s *gb);
// void ppu_tick(struct ppu_s *ppu, struct gb_s *gb, uint32_t cycles);
// void timer_tick(struct timer_s *timer, struct gb_s *gb, uint32_t cycles);

// Cross-cutting helper APIs
// uint8_t gb_read_ie(struct gb_s *gb);
// uint8_t gb_read_if(struct gb_s *gb);
// void gb_request_interrupt(struct gb_s *gb, uint8_t mask);


// Maybe do as well:
// BAD: gb->cpu.if_reg |= 0x01;
// GOOD: gb_request_interrupt(gb, INT_VBLANK);

// BAD: gb->ppu.ly = 0;
// GOOD: ppu_set_ly(&gb->ppu, gb, value);

// BAD: gb->ppu.mode = 1;
// GOOD: ppu_enter_mode(&gb->ppu, gb, PPU_MODE_VBLANK);

// DO THIS: ------------------
// cycles = cpu_step(gb);
// gb_tick(gb, cycles);

/*

CURRENTLY doing (simple mode)
void gb_tick(struct gb_s *gb, uint32_t cycles)
{
    timer_tick(&gb->timer, gb, cycles);
    ppu_tick(&gb->ppu, gb, cycles);
}

LATER can do:
void gb_tick(struct gb_s *gb, uint32_t cycles)
{
    while (cycles > 0) {
        timer_tick(&gb->timer, gb, 1);
        ppu_tick(&gb->ppu, gb, 1);
        dma_tick(&gb->dma, gb, 1);
        cycles--;
    }
}

WAY more advanced:
void gb_tick(struct gb_s *gb, uint32_t cycles)
{
    scheduler_advance(&gb->scheduler, gb, cycles);
}
*/

/*

===-- gb.c --===
The Top-Level "Machine"s:
    The Entry-Point.
    The init for all 'key' sub-functions.
    The Connection between systems.
    The Handler of cylce tracking.



*/

/*
Functions that should be here:

gb_init
gb_reset
gb_step
gb_run_steps
gb_run_frame
gb_run_until

+ a bunch of API calls, to handle interupts, and certain machine-level useful states.

*/





// Top-Level "Machine" Setup/ Initializer
int gb_init(GB *gb) {
    printf("Initializing GB...");

    // Create the PPU 'object', this is passed as needed.

    gb->panic = 0;
    gb->cpu.state.panic = 0;

    const char *rom_file = "../../rom/pkmn_red.gb"; // Not ideal,  I should pass it.. or load it from something.
    //const char *rom_file = "../rom/cpu-individual/07-jr,jp,call,ret,rst.gb";
    printf("NOTE: Using rom file: %s\n\n", rom_file);

    if (cpu_init(gb) != 0){
        fprintf(stderr, "Error Initializing GB CPU:\n");
        return -1;
    }
    //mmu_init(gb);
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
    if (apu_init(gb) != 0) {
        fprintf(stderr, "Error Initializing GB APU config:\n");
        return -1;
    }

    return 0;
}

// The gb step, advances emulation by one cpu step and adds cycles to gb 'tick'.
uint32_t gb_step(GB *gb) {
    uint32_t cycles;

    cycles = cpu_step(gb);  // T-cycles / clock cycle (Not machine cycles)
    gb_tick(gb, cycles);
    gb->step_count++;

    return cycles;
}

/*

GB Tick => Machine timing dispatcher.

*/
// Tick; update the total_cycles / frame_cycles,
void gb_tick(GB *gb, uint32_t cycles){
    gb->total_cycles += cycles;
    gb->frame_cycles += cycles;

    timer_tick(gb, &gb->timer, cycles);
    ppu_tick(gb, &gb->ppu, cycles);
}

// The run loop, limited by steps.
int gb_run_steps(GB *gb, int max_steps) {
    printf("GB RUN => limit by steps\n");
    int step_count;
    for (step_count = 0; step_count < max_steps; step_count++) {
        if (gb->panic) {    // Include both as it's migrated.
            printf("GB Panic, Cancelling run..\n");
            exit(1);
            return -1;
        }
        if (gb->cpu.state.panic) {
            printf("CPU Panic, Cancelling run..\n");
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


// The run loop, limited by elasped time
void gb_run_time(GB *gb, uint64_t max_time) {


    gb_step(gb);
}

// The run loop, limited by cycles
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



void gb_request_interrupt(GB *gb, uint8_t bit) {
    //gb->state.Interrupt_IF = 1;

    // Set the IF interrupt based on the input bit:
    // VBLANK = 0, LCD_STAT = 1, TIMER = 2, SERIAL = 3, JOYPAD = 4
    gb->interrupts.IF |= (1 << bit);
}

// Special interupt handling both IE (Interrupt Enable) and IF (Interrupt Flag):
uint8_t ie_read(GB *gb, uint16_t addr) {
    return gb->interrupts.IE;
}
void ie_write(GB *gb, uint16_t addr, uint8_t val) {
    gb->interrupts.IE = val;
}
uint8_t if_read(GB *gb, uint16_t addr) {
    return gb->interrupts.IF;
}
void if_write(GB *gb, uint16_t addr, uint8_t val) {
    gb->interrupts.IF = val;
}

