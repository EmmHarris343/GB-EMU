#include "io.h"

uint8_t io_read(uint16_t addr) {
    switch (addr) {
        case 0xFF:
            return 144;
        default:
            return 0xFF;
    }
}

void io_write(uint16_t addr, uint8_t val)
{
    // set to nothing and move on.
    (void)addr;
    (void)val;
}


/*
Some names for concepts:

Master clock — the central timing reference
T-cycle / clock cycle / dot — good base time unit for Game Boy
Instruction-stepped emulation — advance hardware once per instruction
Cycle-stepped emulation — advance hardware once per cycle
Accumulator-based timing — add cycles to counters and process thresholds
Finite state machine — how PPU modes progress over time
Frame pacing / throttling — preventing the emulator from running too fast
Memory-mapped IO — registers like FF44 for LY
Device-backed register — IO register read reflects device state
Central scheduler / coordinator — top-level function distributing elapsed time

*/