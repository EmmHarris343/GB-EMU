#ifndef CPU_INSTRUCTIONS_H
#define CPU_INSTRUCTIONS_H


// Include CPU Header Files
#include "cpu.h"

// Include the gb_s struct from gb.h, this avoids importing gb.h into this header.
typedef struct gb_s GB;


// Maybe later:
typedef struct {
    uint8_t opcode;
    const char* mnemonic;
    uint8_t length;
    uint8_t cycles;
    // maybe flags read/written
    // maybe operand type metadata
} instruction_meta_t;

// CPU interrupt handling function/ entry point:
uint8_t cpu_interrupt_handling(GB *gb);

// CPU INIT:
void init_cpu_instruction_test(GB *gb);
void init_cpu_instruction_mem(GB *gb);

// Main instruction entry points.
int execute_instruction(GB *gb, CPU *cpu, instruction_T instruction);
int execute_test(GB *gb, CPU *cpu, instruction_T instruction);

#endif