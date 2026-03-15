#ifndef GB_H
#define GB_H

#include <stdint.h>

#include "cpu/cpu.h"
#include "mmu.h"
#include "io.h"
#include "cart/cart.h"
#include "cart/mbc.h"
#include "loc_ram.h"
#include "ppu/ppu.h"
#include "apu/apu.h"
#include "timer/timer.h"

#define GB_CPU_HZ 4194304
#define GB_FPS 59.7275
#define GB_CYCLES_PER_FRAME 70224

#define GB_CYCLES_PER_FRAME 70224 // ISH
#define GB_FRAME_NS 16742706ULL   // 16.742706 ms ISH

#define GB_UL_VAL 1000000000ULL


typedef struct {
    uint8_t IE;
    uint8_t IF;
} Interrupts; // Make this just a general GB state.

typedef struct {
    // CPUTraceBuffer cpu_trace;
    // MemTraceBuffer mem_trace;
    // DebugConfig config;
} DebugState;

typedef struct DebugStats {
    uint64_t cpu_steps;
    uint64_t cpu_cycles;
    uint64_t frames;
    uint64_t ppu_tick_calls;
    uint64_t ppu_cycles_seen;
    uint64_t ly_wraps;
    uint64_t vblank_entries;
} DebugStats;

typedef struct gb_s {
    // This is the instruction OPCODE + OP1 + OP2.
    instruction_T instruction;

    // Hardware/ Sub-Functions for inter-connection:
    CPU cpu;
    PPU ppu;
    //APU apu;
    Memory memory;
    Cartridge cart;

    // Routing:
    MMU mmu;
    IO io;

    struct oam_s *oam;


    // Only the IE/ IF interupts:
    Interrupts interrupts;

    // Cycles, ticks, timer:
    Timer timer;
    uint64_t step_count;
    uint64_t total_cycles;
    uint32_t frame_cycles;

    DebugStats db_stats;

    /// TODO: move to gb state?
    uint8_t panic;      // Move this to 'machine' panic, not just cpu panic level.
    uint8_t quit;
} GB;


enum {
    GB_INTERRUPT_VBLANK   = 0,
    GB_INTERRUPT_LCD_STAT = 1,  // 0x48
    GB_INTERRUPT_TIMER    = 2,
    GB_INTERRUPT_SERIAL   = 3,
    GB_INTERRUPT_JOYPAD   = 4
};

int gb_init(GB *gb);


// Run by frame..
int gb_run(GB *gb);

int gb_run_steps(GB *gb, int max_steps);
void gb_run_time(GB *gb, uint64_t max_time);

uint32_t gb_step(GB *gb);
void gb_tick(GB *gb, uint32_t cycles);
void gb_reset(GB *gb);


// Special Interupts. Request Interrupt, IF (Interrupt Flag), IE (Interrupt Enable)
void gb_request_interrupt(GB *gb, uint8_t bit);

uint8_t if_read(GB *gb, uint16_t addr);
uint8_t ie_read(GB *gb, uint16_t addr);
void if_write(GB *gb, uint16_t addr, uint8_t val);
void ie_write(GB *gb, uint16_t addr, uint8_t val);


#endif