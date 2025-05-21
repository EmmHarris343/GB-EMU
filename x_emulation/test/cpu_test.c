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


#define tFLAG_Z 0x80  // 1000 0000
#define tFLAG_N 0x40  // 0100 0000
#define tFLAG_H 0x20  // 0010 0000
#define tFLAG_C 0x10  // 0001 0000


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
    //printf("\n");
    printf("--------\nRaw Hex of FLAGS: (Orig/EX)[0x%02X/ 0x%02X]\n--------\n", inital_cpu->reg.F, expected_cpu->reg.F);
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
    working->reg.HL == expected->reg.HL &&
    working->reg.PC == expected->reg.PC;
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
        // White in RAM where [HL] points to. The valu of the Source value/ Register.e
        external_write(initial_cpu->reg.HL, *reg_lookup_initial[src_code]);
    }
    if (src_code == 6) {
        // LD x, [HL]
        // Seems counter intuitive. But WRITE 0xD0, so when it reads from ram in the value pointed by [HL]. 
        // It will match what's in the expected_cpu val.
        external_write(initial_cpu->reg.HL, p_hl_val);
        *reg_lookup_expected[dest_code] = p_hl_val; // 
    }
    if ((src_code != 6) & (dest_code != 6)) {
        *reg_lookup_expected[dest_code] = *reg_lookup_initial[src_code];
    }

    expected_cpu->reg.PC ++;    // 1 Byte. No Flag Change.
}

void unt_ld_tcase(instruction_T local_instrc) {
    printf("---------\nStart LD Unit Test. OPCODE=0x%02X\n", local_instrc.opcode);
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

    int exe_return = execute_test(&working_cpu, local_instrc);
    if (exe_return  != 0) { 
        printf("ERROR [cpu_test], failed to execute test. OPCODE: [0x%02X] \n", ld_test.opcode); 
        logging_log("[FATAL-ERROR], instruction failed to execute. Name:[%s] OPCODE: [0x%02X]\n", ld_test.name, ld_test.opcode);
    }
    else { 
        printf("Execution Completed.\n"); 
    }
    
    if (reg_compare(&working_cpu, &ld_test.expected_cpu)) {
        printf("[PASS] Name:[%s] OPCODE: [0x%02X]\n", ld_test.name, ld_test.opcode);
        logging_log("[PASS] Name:[%s] OPCODE: [0x%02X]\n", ld_test.name, ld_test.opcode);   // Save to log file
    }
    else {
        printf("[FAILED] Test failed. Register did not match expected values.\n");
        logging_log("[FAILED] Name:[%s] OPCODE: [0x%02X]\n", ld_test.name, ld_test.opcode); // Save to log file
    }
    // // CPU Registry Compare.
    // view_regs(&working_cpu, &ld_test.expected_cpu);
    // // Print out pass fail for each AF - HL.
    // reg_compare2(&working_cpu, &ld_test.expected_cpu);
}


void get_expected_8bit_arithmetic(instruction_T instruction, CPU* initial_cpu, CPU* expected_cpu, char* spec_message, uint8_t p_hl_val) {
    uint8_t opcode = instruction.opcode;
    //uint8_t dest_code = (opcode >> 3) & 0x07;          // Dest bits, 5-3  // NOT NEEDED. All instructions use A
    uint8_t src_code = opcode & 0x07;                   // Source Bits 2-0
    size_t opname_max_size = 32;

    const char* reg_names[8] = { "B", "C", "D", "E", "H", "L", "[HL]", "A" };
    
    uint8_t *reg_lookup_initial[8] = {
        &initial_cpu->reg.B, &initial_cpu->reg.C, &initial_cpu->reg.D, &initial_cpu->reg.E, 
        &initial_cpu->reg.H, &initial_cpu->reg.L, NULL, &initial_cpu->reg.A
    };

    uint8_t *reg_lookup_expected[8] = {
        &expected_cpu->reg.B, &expected_cpu->reg.C, &expected_cpu->reg.D, &expected_cpu->reg.E, 
        &expected_cpu->reg.H, &expected_cpu->reg.L, NULL, &expected_cpu->reg.A
    };

    uint8_t orig_cpu_A_val = initial_cpu->reg.A;

    if (opcode >= 0x80 && opcode <= 0x87) {
        // ADD A, x Instruction.
        snprintf(spec_message, opname_max_size, "ADD A, %s", reg_names[src_code]);
        uint8_t r8_src_val = 0x0;

        if (src_code == 6) { r8_src_val = external_read(initial_cpu->reg.HL); }     // ADD A, [HL]
        else { r8_src_val = *reg_lookup_expected[src_code]; }                               // ADD A, r8

        // 16bit for bit overflow:
        uint16_t add_result = (expected_cpu->reg.A + r8_src_val);

        uint8_t final_8bit = (uint8_t)add_result;   // Truncate anything above 8bit

        (final_8bit == 0) ? (expected_cpu->reg.F |= tFLAG_Z) : (expected_cpu->reg.F ^= tFLAG_Z);    // Z Flag
        ((expected_cpu->reg.A & 0x0F) + (r8_src_val & 0x0F) > 0x0F) ? (expected_cpu->reg.F |= tFLAG_H) : (expected_cpu->reg.F ^= tFLAG_H); // H Flag
        (add_result > 0xFF) ? (expected_cpu->reg.F |= tFLAG_C) : (expected_cpu->reg.F ^= tFLAG_C); // C Flag
        (expected_cpu->reg.F ^= tFLAG_N);  // N Flag (Subtraction)

        expected_cpu->reg.A = final_8bit;
        expected_cpu->reg.PC ++;            // 1 Byte.
    }
    if (opcode >= 0x88 && opcode <= 0x8F) {
        // ADC A, x Instruction
        snprintf(spec_message, opname_max_size, "ADC A, %s", reg_names[src_code]);
        uint8_t r8_src_val = 0x0;
        
        if (src_code == 6) { r8_src_val = external_read(initial_cpu->reg.HL); }     // ADC A, [HL]
        else { r8_src_val = *reg_lookup_expected[src_code]; }                               // ADC A, r8
        
        // Read CPU state, 
        uint8_t carry_val = (expected_cpu->reg.F & FLAG_C) ? 1 : 0;

        // Use 16bit for Flag checks. 8bit will truncate results
        uint16_t add_16bit  = (expected_cpu->reg.A + r8_src_val + carry_val);
        uint8_t final_8bit = (uint8_t)add_16bit;

        (final_8bit == 0) ? (expected_cpu->reg.F |= tFLAG_Z) : (expected_cpu->reg.F ^= tFLAG_Z);    // Z Flag
        (((expected_cpu->reg.A & 0x0F) + (r8_src_val & 0x0F) + carry_val) > 0x0F) ? (expected_cpu->reg.F |= tFLAG_H) : (expected_cpu->reg.F ^= tFLAG_H); // H Flag
        (add_16bit > 0xFF) ? (expected_cpu->reg.F |= tFLAG_C) : (expected_cpu->reg.F ^= tFLAG_C); // C Flag
        (expected_cpu->reg.F ^= tFLAG_N);  // N Flag (Subtraction)

        expected_cpu->reg.A = final_8bit;
        expected_cpu->reg.PC ++;    // Only 1 Byte.
    }
    if (opcode >= 0x90 && opcode <= 0x97) {
        // SUB A, x Instruction
        snprintf(spec_message, opname_max_size, "SUB A, %s", reg_names[src_code]);
        uint8_t r8_src_val = 0x0;
        
        if (src_code == 6) { r8_src_val = external_read(initial_cpu->reg.HL); }     // SUB A, [HL]
        else { r8_src_val = *reg_lookup_expected[src_code]; }                               // SUB A, r8

        uint16_t sub_16bit = (expected_cpu->reg.A - r8_src_val);
        uint8_t final_8bit = (uint8_t)sub_16bit;

        (final_8bit == 0) ? (expected_cpu->reg.F |= tFLAG_Z) : (expected_cpu->reg.F ^= tFLAG_Z);    // Z Flag
        ((expected_cpu->reg.A & 0x0F) < (r8_src_val & 0x0F)) ? (expected_cpu->reg.F |= tFLAG_H) : (expected_cpu->reg.F ^= tFLAG_H); // H Flag
        (expected_cpu->reg.A < r8_src_val) ? (expected_cpu->reg.F |= tFLAG_C) : (expected_cpu->reg.F ^= tFLAG_C); // C Flag
        (expected_cpu->reg.F |= tFLAG_N);;  // N Flag (Subtraction) Always SET on SUB/SBC

        expected_cpu->reg.A = final_8bit;
        expected_cpu->reg.PC ++;
    }
    if (opcode >= 0x98 && opcode <= 0x9F) {
        // SBC A, x Instruction
        snprintf(spec_message, opname_max_size, "SBC A, %s", reg_names[src_code]);
        uint8_t r8_src_val = 0x0;
        
        if (src_code == 6) { r8_src_val = external_read(initial_cpu->reg.HL); }     // SBC A, [HL]
        else { r8_src_val = *reg_lookup_expected[src_code]; }                               // SBC A, r8

        uint8_t carry_val = (initial_cpu->reg.F & FLAG_C) ? 1 : 0;

        // Use 16bit for Flag checks. 8bit will truncate results
        uint16_t sub_16bit  = (expected_cpu->reg.A - r8_src_val - carry_val);
        uint8_t final_8bit = (uint8_t)sub_16bit;


        (final_8bit == 0) ? (expected_cpu->reg.F |= tFLAG_Z) : (expected_cpu->reg.F ^= tFLAG_Z);    // Z Flag
        (((expected_cpu->reg.A & 0x0F) - (r8_src_val & 0x0F) - carry_val) < 0) ? (expected_cpu->reg.F |= tFLAG_H) : (expected_cpu->reg.F ^= tFLAG_H); // H Flag
        (expected_cpu->reg.A < (r8_src_val + carry_val)) ? (expected_cpu->reg.F |= tFLAG_C) : (expected_cpu->reg.F ^= tFLAG_C); // C Flag
        (expected_cpu->reg.F |= tFLAG_N);  // N Flag (Subtraction) Always SET on SUB/SBC

        expected_cpu->reg.A = final_8bit;
        expected_cpu->reg.PC ++;
    }



    // Not sure I want to use a switch case...
            // switch (opcode) {
    //     case 0x80 ... 0x87:             
    //         break;
    //     default: 
    //         printf("[ERR], 8bit arithmetic failure. Opcode doesn't match\n");
    // };
}

void get_expected_logic_operations(instruction_T instruction, CPU* initial_cpu, CPU* expected_cpu, char* spec_message, uint8_t p_hl_val) {
    uint8_t opcode = instruction.opcode;
    //uint8_t dest_code = (opcode >> 3) & 0x07;          // Dest bits, 5-3  // NOT NEEDED. All instructions use A
    uint8_t src_code = opcode & 0x07;                   // Source Bits 2-0
    size_t opname_max_size = 32;

    const char* reg_names[8] = { "B", "C", "D", "E", "H", "L", "[HL]", "A" };
    
    uint8_t *reg_lookup_expected[8] = {
        &expected_cpu->reg.B, &expected_cpu->reg.C, &expected_cpu->reg.D, &expected_cpu->reg.E, 
        &expected_cpu->reg.H, &expected_cpu->reg.L, NULL, &expected_cpu->reg.A
    };

    if (opcode >= 0xA0 && opcode <= 0xA7) {
        // AND A, X
        uint8_t reg_val = 0x00;
        
        if (src_code == 6) { reg_val = external_read(initial_cpu->reg.HL); }     // AND A, [HL]
        else { reg_val = *reg_lookup_expected[src_code]; }                               // AND A, r8
        
        uint8_t AND_result = (initial_cpu->reg.A & reg_val);
        initial_cpu->reg.A = AND_result;

        (AND_result == 0) ? (expected_cpu->reg.F |= tFLAG_Z) : (expected_cpu->reg.F ^= tFLAG_Z);
        (expected_cpu->reg.F ^= tFLAG_N);  // ALways cleard
        (expected_cpu->reg.F |= tFLAG_H);    // Always set
        (expected_cpu->reg.F ^= tFLAG_C);  // Always cleared.

        expected_cpu->reg.PC ++;
    }
    if (opcode >= 0xA8 && opcode <= 0xAF) {
        // OR A, X
        uint8_t reg_val = 0x00;
        
        if (src_code == 6) { reg_val = external_read(initial_cpu->reg.HL); }     // OR A, [HL]
        else { reg_val = *reg_lookup_expected[src_code]; }                               // OR A, r8
        
        uint8_t OR_result = (initial_cpu->reg.A | reg_val);
        expected_cpu->reg.A = OR_result;

        (OR_result == 0) ? (expected_cpu->reg.F |= tFLAG_Z) : (expected_cpu->reg.F ^= tFLAG_Z);
        (expected_cpu->reg.F ^= tFLAG_N);  // ALways cleared
        (expected_cpu->reg.F ^= tFLAG_H);  // Always cleared
        (expected_cpu->reg.F ^= tFLAG_C);  // Always cleared

        expected_cpu->reg.PC ++;
    }

    }

void unt_tcase_builder(instruction_T local_instrc) {
    printf("---------\nCreating Unit Test. FOR --> OPCODE=0x%02X\n", local_instrc.opcode);
    CPU initial_cpu_state = cpu_reg_simple_tstate;
    CPU expected_cpu_state = cpu_reg_simple_tstate;

    uint8_t p_hl_val = 0xD0;
    char instruc_name_val[32];

    // This is the only part that might be hard.. to be Dynamic to point to the right Function.
    // But..... I likely need to build, a specific function for each Instruction group. 
    get_expected_8bit_arithmetic(
        local_instrc,
        &initial_cpu_state,
        &expected_cpu_state,
        instruc_name_val,
        p_hl_val);
    
    Test_Case_t build_test;
    build_test.name = instruc_name_val;
    build_test.opcode = local_instrc.opcode;
    build_test.initial_cpu = initial_cpu_state;

    build_test.expected_cpu = expected_cpu_state;

    CPU working_cpu = build_test.initial_cpu;
    
    int exe_return = execute_test(&working_cpu, local_instrc);
    if (exe_return  != 0) { 
        printf("ERROR [cpu_test], instruction failed to execute"); 
    }
    else { 
        printf("Execution Complete.\n"); 
    }

    view_regs(&build_test.initial_cpu, &build_test.expected_cpu);
    view_regs(&working_cpu, &build_test.expected_cpu);

    bool rg_return = reg_compare(&working_cpu, &build_test.expected_cpu);
    if (rg_return) {
        printf("[PASS] Name:[%s] OPCODE: [0x%02X]\n", build_test.name, build_test.opcode);
        logging_log("[PASS] Name:[%s] OPCODE: [0x%02X]\n", build_test.name, build_test.opcode);
    }
    else {
        printf("[FAILED] Name:[%s] OPCODE: [0x%02X] Test failed. Registers did not match. \n", build_test.name, build_test.opcode);
        logging_log("[FAILED] Name:[%s] OPCODE: [0x%02X]\n", build_test.name, build_test.opcode);
        // Don't like this... This is a HARD CODED Test. I need to make this modular.
    }


    // CPU Registry Compare.
    // view_regs(&working_cpu, &ld_test.expected_cpu);
    // // Print out pass fail for each AF - HL.
    // reg_compare2(&working_cpu, &ld_test.expected_cpu);

}



void entry_test_case(){
    instruction_T instrc;
    instrc.opcode = 0x5C;           // Placeholder, so it's initialized. (This probably should be NOP)
    instrc.operand1 = 0x00;
    instrc.operand2 = 0x00;

    for (int i = 0x80; i <= 0x9F; i++) {
        instrc.opcode = i;
        //unt_ld_tcase(instrc);
        unt_tcase_builder(instrc);

        if (i == 0x7F) { break; }   // Hit the last line. STOP and close
    }
}







// Full integration test:
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