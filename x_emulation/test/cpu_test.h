// #ifdef ENABLE_TESTS
// // This is for The build flag.

// #include <stdio.h>
// #include "../src/cpu.h"

// #endif

#ifndef CPU_TEST_H
#define CPU_TEST_H

#include <stdint.h>
#include <stdbool.h>
#include "../src/cpu.h"
//#include "../src/mmu.h"

/// CONSIDER:
// Maybe do this? 

typedef struct {
    const char *name;         // e.g., "INC BC x1"
    CPU initial_cpu;
    // MMU initial_mmu;

    uint8_t opcode;           // instruction to run
    uint8_t operand1;
    uint8_t operand2;

    CPU expected_cpu;
    // MMU expected_mmu;

    //bool (*custom_check)(const CPU *, const MMU *);  // Test callback? use, don't use?
    //bool (*custom_check)(const CPU *);  // 
} Test_Case_t;

typedef void (*predict_fn)(CPU *cpu);
typedef void (*exec_fn)(CPU *cpu);


typedef struct {
    const char *name;
    predict_fn predict;
    exec_fn execute;
    uint8_t initial_B;
    uint8_t initial_flags;
} InstructionTest;

typedef struct {
    instruction_T instr;
    uint8_t initial_A;
    uint8_t expected_A;
    uint8_t from_reg_val;
    uint8_t operand;
    uint8_t memory_val;    // For [HL] if needed
    uint8_t expected_flags;
    
} add8_test_case;

// void run_test_case(const test_case_t *test);


// Entry point from e_ctrl.c
//void unit_test_instruction();    // This one is more specific (A direct cpu_instruction test)
//void unt_test_case();   // Shouldn't b e called directly
void entry_test_case();

#endif



