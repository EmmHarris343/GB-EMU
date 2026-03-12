#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

typedef struct gb_s GB;

typedef struct {
    const char *name;
    const char *description;
    uint8_t bytes;
    uint8_t base_cycles;
    uint8_t taken_cycles;
} OpcodeInfo;


void init (GB *gb);

// Not sure if that would work.
typedef enum {
    NOP,
    RLCA,
    RLA,
    RRCA,
    RRA,
    CPL,
    XOR_A_r8,
    XOR_A_n8
} debug_mnonic;    // BUS tag names.


#endif