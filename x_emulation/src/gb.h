#ifndef GB_H
#define GB_H

#include <stdint.h>

#include "cpu.h"
#include "mmu.h"
#include "cart.h"
#include "mbc.h"
#include "loc_ram.h"
#include "ppu.h"
#include "apu.h"
#include "timer.h"

typedef struct {
    uint8_t Interrupt_IE;
    uint8_t Interrupt_IF;
} GB_State; // Make this just a general GB state.

typedef struct gb_s {
    // Move for debug info:
    instruction_T instruction;
    uint64_t step_count;

    // Hardware/ Sub-Functions for inter-connection:
    CPU cpu;
    MMU mmu;
    PPU ppu;
    Cartridge cart;
    struct io_s *io;
    struct apu_s *apu;  // Would APU be under io?
    struct oam_s *oam;

    // GB states, interupts, pause,
    GB_State state;

    // Cycles, ticks, timer:
    struct timer_s *timer;
    uint64_t total_cycles;
    uint32_t frame_cycles;

    /// TODO: move to gb state?
    uint8_t panic;      // Move this to 'machine' panic, not just cpu panic level.
} GB;

int gb_init(GB *gb);

// Special set state that comes from CPU -> MMU -> CPU.STATE.IE
uint8_t ie_read(GB *gb, uint16_t addr);
void ie_write(GB *gb, uint16_t addr, uint8_t val);

int gb_run_steps(GB *gb, int max_steps);
void gb_run_time(GB *gb, uint64_t max_time);

uint32_t gb_step(GB *gb);
void gb_tick(GB *gb, uint32_t cycles);
void gb_reset(GB *gb);


#endif