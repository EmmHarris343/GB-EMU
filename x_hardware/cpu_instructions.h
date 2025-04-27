#ifndef CPU_INSTRUCTIONS_H
#define CPU_INSTRUCTIONS_H


// Include CPU Header Files
#include "cpu.h"




// Maybe later:
typedef struct {
    uint8_t opcode;
    const char* mnemonic;
    uint8_t length;
    uint8_t cycles;
    // maybe flags read/written
    // maybe operand type metadata
} instruction_meta_t;

// Each instruction works on the CPU state

// void instr_LD(CPU *cpu, uint8_t reg, uint8_t value);
// void instr_ADD(CPU *cpu, uint8_t value);
// void instr_SUB(CPU *cpu, uint8_t value);

int execute_instruction(CPU *cpu, instruction_T instrc, int step_count);

// ... more instructions

#endif