#include "cpu_test.h"
#ifdef ENABLE_TESTS
// This is for The build flag.

#include <stdio.h>
#include "../src/cpu.h"

#endif


#include <stdio.h>
#include "../src/cpu.h"
#include "../src/cpu_instructions.h"

#include "../src/logger.h"


#include <assert.h>

extern CPU cpu_reg_simple_tstate;


void check_flags(CPU* inital_cpu, CPU* expected_cpu) {
    printf("\n:CPU: === Registers === \n");
    (inital_cpu->reg.F & FLAG_Z) ? printf("  Z Flag set | ") : printf("  Z Flag NOT set | ");
    (inital_cpu->reg.F & FLAG_N) ? printf("  N Flag set | ") : printf("  N Flag NOT set | ");
    (inital_cpu->reg.F & FLAG_H) ? printf("  H Flag set | ") : printf("  H Flag NOT set | ");
    (inital_cpu->reg.F & FLAG_C) ? printf("  C Flag set") : printf("  C Flag NOT set");
}

void view_regs(CPU* inital_cpu, CPU* expected_cpu) {
    // 
    // printf("\n:CPU: === Registers === \n");
    // printf("  A: 0x%02X\n", inital_cpu->reg.A);
    // printf("  B: 0x%02X, C: 0x%02X\n", inital_cpu->reg.B, inital_cpu->reg.C);
    // printf("  D: 0x%02X, E: 0x%02X\n", inital_cpu->reg.D, inital_cpu->reg.E);
    // printf("  H: 0x%02X, L: 0x%02X\n", inital_cpu->reg.H, inital_cpu->reg.L);
    // printf("\n");

    printf("\n:CPU: === Registers === \n");
    printf("  A: (Orig/EX)[0x%02X/ 0x%02X]\n", inital_cpu->reg.A, expected_cpu->reg.A);
    printf("  B: (Orig/EX)[0x%02X/ 0x%02X] | C: (Orig/EX)[0x%02X/ 0x%02X]\n", inital_cpu->reg.B, expected_cpu->reg.B, inital_cpu->reg.C, expected_cpu->reg.C);
    printf("  D: (Orig/EX)[0x%02X/ 0x%02X] | E: (Orig/EX)[0x%02X/ 0x%02X]\n", inital_cpu->reg.D, expected_cpu->reg.D, inital_cpu->reg.E, expected_cpu->reg.E);
    printf("  H: (Orig/EX)[0x%02X/ 0x%02X] | L: (Orig/EX)[0x%02X/ 0x%02X]\n", inital_cpu->reg.H, expected_cpu->reg.H, inital_cpu->reg.L, expected_cpu->reg.L);
    printf("\n");
}

void reg_compare2(CPU *working, CPU *expected) {
    (working->reg.AF == expected->reg.AF) ? printf("[PASS]=AF :: ") : printf("[FAIL] AF\n");
    (working->reg.BC == expected->reg.BC) ? printf("[PASS]=BC :: ") : printf("[FAIL] BC\n");
    (working->reg.DE == expected->reg.DE) ? printf("[PASS]=DE :: ") : printf("[FAIL] DE\n");
    (working->reg.HL == expected->reg.HL) ? printf("[PASS]=HL\n") : printf("[FAIL] HL\n");
    //(working->reg. == expected->reg.AF) ? printf("[PASS] AF\n") : printf("[FAIL] AF\n");
}

bool reg_compare(CPU *working, CPU *expected) {
    return \
    working->reg.AF == expected->reg.AF &&
    working->reg.BC == expected->reg.BC &&
    working->reg.DE == expected->reg.DE &&
    working->reg.HL == expected->reg.HL;
    //working->reg.PC == expected->reg.PC &&
    //working->reg.SP == expected->reg.SP;
}

void get_expected_ld_reg(instruction_T instruction, CPU* initial_cpu, CPU* expected_cpu, char* spec_message, uint8_t p_hl_val) {
    uint8_t opcode = instruction.opcode;
    uint8_t dest_code = (opcode >> 3) & 0x07;           // Dest bits, 5-3
    uint8_t src_code = opcode & 0x07;                   // Source Bits 2-0

    const char* reg_names[8] = { "B", "C", "D", "E", "H", "L", "[HL]", "A" };


    size_t max_length = 32;
    snprintf(spec_message, max_length, "LD %s, %s", reg_names[dest_code], reg_names[src_code]);
    
    uint8_t *reg_lookup_initial[8] = {
        &initial_cpu->reg.B, &initial_cpu->reg.C, &initial_cpu->reg.D, &initial_cpu->reg.E, 
        &initial_cpu->reg.H, &initial_cpu->reg.L, NULL, &initial_cpu->reg.A
    };

    uint8_t *reg_lookup_expected[8] = {
        &expected_cpu->reg.B, &expected_cpu->reg.C, &expected_cpu->reg.D, &expected_cpu->reg.E, 
        &expected_cpu->reg.H, &expected_cpu->reg.L, NULL, &expected_cpu->reg.A
    };
    
    if (dest_code == 6) {
        // LD [HL], x 
        // I don't think I need to do technically anything. Because I have to Load the data from RAM.
    }
    if (src_code == 6) {
        // LD x, [HL]
        // Set value to "special HL value." IE: D0
        *reg_lookup_expected[dest_code] = 0xD0;
    }
    if ((src_code != 6) & (dest_code != 6)) {
        *reg_lookup_expected[dest_code] = *reg_lookup_initial[src_code];
    }

    //return *expected_cpu;
}

void unt_ld_tcase(instruction_T local_instrc) {
    printf("---------\nStart Unit LD Test. OPCODE=0x%02X\n", local_instrc.opcode);
    CPU initial_cpu_state = cpu_reg_simple_tstate;
    CPU expected_cpu_state = cpu_reg_simple_tstate;

    uint8_t p_hl_val = 0xD0;
    char instruc_name_val[32];

    get_expected_ld_reg(
        local_instrc,
        &initial_cpu_state,
        &expected_cpu_state,
        instruc_name_val,
        p_hl_val);
    
    Test_Case_t ld_test;
    ld_test.name = instruc_name_val;
    ld_test.opcode = local_instrc.opcode;
    ld_test.initial_cpu = initial_cpu_state;

    ld_test.expected_cpu = expected_cpu_state;

    CPU working_cpu = ld_test.initial_cpu;
    if (execute_test(&working_cpu, local_instrc) != 0) { printf("ERROR [cpu_test], instruction failed to execute"); }
    else { printf("Execution successful\n"); }

    // // CPU Registry Compare.
    // view_regs(&working_cpu, &ld_test.expected_cpu);
    // // Print out pass fail for each AF - HL.
    // reg_compare2(&working_cpu, &ld_test.expected_cpu);


    if (reg_compare(&working_cpu, &ld_test.expected_cpu)) {
        printf("[PASS] Name:[%s] OPCODE: [0x%02X]\n", ld_test.name, ld_test.opcode);
        // Save to log file:
        logging_log("[PASS] Name:[%s] OPCODE: [0x%02X]\n", ld_test.name, ld_test.opcode);
    }
    else {
        printf("[FAILED] Test failed. Register did not match expected values.\n");
        logging_log("[FAILED] Name:[%s] OPCODE: [0x%02X]\n", ld_test.name, ld_test.opcode);
        // Don't like this... This is a HARD CODED Test. I need to make this modular.
    }
}

void entry_test_case(){
    instruction_T instrc;
    instrc.opcode = 0x5C;               // Pass instrc direction into cpu_instructions.c
    instrc.operand1 = 0x00;
    instrc.operand2 = 0x00;

    for (int i = 0x40; i < 0x66; i++) {
        // Thing to do.... Sighhhh
        instrc.opcode = i;
        unt_ld_tcase(instrc);

        if (i == 0x7F) { break; }   // Hit the last line. STOP and close
    }
}


/* ------ */
/*
My Main Test option.

This is so I can. Relatively quickly test.. the question:
"Does my instruction even work?"


This  Directly calls instructions in cpu_instructions.c
It by-passes reading of ROM, Reading of PC, Decoding, Execution. Any extra steps/ states, writes to console, etc
*/


void integration_test_instruction() {
    // Run integration test through cpu.c
    // Test full cycle of read, decode, execute of OPCODE
    // Test the connection, reading, execution and return of cpu_instruction.c
    
    
    // Another words.... I need to set the PC to have the OPCODE and operands. 
    // So it can decode them. THEN Execute them.


    // IE: Do this a little bit later.
    // Right now, I need to figure out if my instructions work as intended.
}


void run_multiple_cpu_test() {
    int total_tests = 40;

    for(int i = 0; i < total_tests; i++){
        /// EXECUTE: CPU Instruction Test.
    }
}