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

    CPU expected_cpu;
    // MMU expected_mmu;

    //bool (*custom_check)(const CPU *, const MMU *);  // Test callback? use, don't use?
    //bool (*custom_check)(const CPU *);  // 
} test_case_t;

// void run_test_case(const test_case_t *test);


// Entry point from e_ctrl.c
void unit_test_instruction();    // This one is more specific (A direct cpu_instruction test)


#endif



