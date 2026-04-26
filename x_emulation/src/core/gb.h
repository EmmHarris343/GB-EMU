#ifndef GB_H
#define GB_H

#include <stdint.h>

#include "cpu/cpu.h"
#include "mmu.h"
#include "io.h"
#include "joy/joy.h"
#include "cart/cart.h"
#include "cart/mbc.h"
#include "loc_ram.h"
#include "ppu/ppu.h"
#include "apu/apu.h"
#include "timer/timer.h"
#include "../debug/logger.h"

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


typedef struct {
    uint16_t pc;
    uint8_t opcode;
    uint8_t operand1;
    uint8_t operand2;

    uint16_t af;
    uint16_t bc;
    uint16_t de;
    uint16_t hl;
    uint16_t sp;

    uint8_t ime;
    uint8_t iF;
    uint8_t iE;

    uint64_t instruction_count;
} TraceEntry;

typedef struct {
    TraceEntry entries[TRACE_CAPACITY];
    size_t write_index;
    size_t count;
} TraceBuffer;

void trace_buffer_push(TraceBuffer *trace, TraceEntry entry);

typedef struct DebugStats {
    uint64_t cpu_steps;
    uint64_t cpu_cycles;
    uint64_t frames;
    uint64_t ppu_tick_calls;
    uint64_t ppu_cycles_seen;
    uint64_t ly_wraps;
    uint64_t vblank_entries;
} DebugStats;

typedef enum {
    GB_RUN_OK = 0,
    GB_RUN_PAUSE,
    GB_RUN_RESET,
    GB_RUN_STOP,
    GB_RUN_EXIT,
    GB_RUN_PANIC = 9
} RunStates;

typedef struct {


} SaveSate;



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
    JOY joy;

    struct oam_s *oam;


    // Only the IE/ IF interupts:
    Interrupts interrupts;

    // Cycles, ticks, timer:
    Timer timer;
    uint64_t step_count;
    uint64_t total_cycles;
    uint32_t frame_cycles;

    // Debug / Crash data
    DebugStats db_stats;
    TraceBuffer trace_buffer;

    // Very important, controls the running, pause, reset, stop, exit, panic conditions.
    // Worth mentioning only a few are actually setup.
    uint8_t run_state;


    /// TODO: move to gb state?
    uint8_t panic;      // Move this to 'machine' panic, not just cpu panic level.

    const char *rom_path;
    const char *ram_save_path;
} GB;


enum {
    GB_INTERRUPT_VBLANK   = 0,
    GB_INTERRUPT_LCD_STAT = 1,  // 0x48
    GB_INTERRUPT_TIMER    = 2,
    GB_INTERRUPT_SERIAL   = 3,
    GB_INTERRUPT_JOYPAD   = 4
};

int gb_init(GB *gb, const char *rom_file);

void gb_shutdown(GB *gb, const char *save_file);


// Run by frame..
void gb_step_frame(GB *gb, uint64_t *next_frame_time_ns);

// other run functions..
int gb_run(GB *gb);

int gb_run_steps(GB *gb, int max_steps);
void gb_run_time(GB *gb, uint64_t max_time);


// Special Interupts. Request Interrupt, IF (Interrupt Flag), IE (Interrupt Enable)
void gb_request_interrupt(GB *gb, uint8_t bit);

uint8_t interrupt_read(GB *gb, uint16_t addr);
void interrupt_write(GB *gb, uint16_t addr, uint8_t interrupt_hex);



#endif