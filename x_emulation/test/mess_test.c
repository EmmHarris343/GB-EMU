#include "cpu_test.h"
//#include <system_error>
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




// void unt_tcase_builder(instruction_T local_instrc) {
//     printf("---------\nCreating Unit Test. FOR --> OPCODE=0x%02X\n", local_instrc.opcode);
//     CPU initial_cpu_state = cpu_reg_simple_tstate;
//     CPU expected_cpu_state = cpu_reg_simple_tstate;

//     uint8_t p_hl_val = 0xD0;
//     external_write(initial_cpu_state.reg.HL, p_hl_val);
//     char instruc_name_val[32];

//     // This is the only part that might be hard.. to be Dynamic to point to the right Function.
//     // But..... I likely need to build, a specific function for each Instruction group.
//     if ((local_instrc.opcode >= 0x80) && (local_instrc.opcode <= 0x9F)) {
//         get_expected_8bit_arithmetic(
//         local_instrc,
//         &initial_cpu_state,
//         &expected_cpu_state,
//         instruc_name_val,
//         p_hl_val);
//     }
//     if (local_instrc.opcode == 0xC6 || local_instrc.opcode == 0xCE || local_instrc.opcode == 0xD6 || local_instrc.opcode == 0xDE) {
//         get_expected_8bit_arithmetic(
//         local_instrc,
//         &initial_cpu_state,
//         &expected_cpu_state,
//         instruc_name_val,
//         p_hl_val);
//     }
//     if ((local_instrc.opcode & 0xC7) == 0x04 || (local_instrc.opcode & 0xC7) == 0x05) {
//         get_expected_8bit_arithmetic(
//         local_instrc,
//         &initial_cpu_state,
//         &expected_cpu_state,
//         instruc_name_val,
//         p_hl_val);
//     }
//     if ((local_instrc.opcode >= 0xA0) && (local_instrc.opcode <= 0xBF)) {
//         get_expected_logic_operations(
//         local_instrc,
//         &initial_cpu_state,
//         &expected_cpu_state,
//         instruc_name_val,
//         p_hl_val);
//     }
//     if (local_instrc.opcode == 0xE6 || local_instrc.opcode == 0xEE || local_instrc.opcode == 0xF6 || local_instrc.opcode == 0xFE) {
//         get_expected_logic_operations(
//         local_instrc,
//         &initial_cpu_state,
//         &expected_cpu_state,
//         instruc_name_val,
//         p_hl_val);
//     }

//     if ((local_instrc.opcode & 0xCF) == 0x03  || (local_instrc.opcode & 0xCF) == 0x0B || (local_instrc.opcode & 0xCF) == 0x09) {
//         get_expected_16bit_arithmetic(
//         local_instrc,
//         &initial_cpu_state,
//         &expected_cpu_state,
//         instruc_name_val,
//         p_hl_val);  
//         /// TODO: Remove p_hl_val. As no 16bit arithmatic uses [HL] pointed values.
//     }

//     Test_Case_t build_test;
//     build_test.name = instruc_name_val;
//     build_test.opcode = local_instrc.opcode;
//     if (local_instrc.operand1 > 0) {
//         build_test.operand1 = local_instrc.operand1;
//     }
//     if (local_instrc.operand2 > 0) {
//         build_test.operand2 = local_instrc.operand2;
//     }    
//     build_test.initial_cpu = initial_cpu_state;
//     build_test.expected_cpu = expected_cpu_state;

//     CPU working_cpu = build_test.initial_cpu;
//     // Write the default value of [HL] again to ram.
//     // Because my "test" of what is in ram, actually writes it to ram.. Sooo.
//     external_write(working_cpu.reg.HL, p_hl_val);
    

//         printf("ERROR [cpu_test], instruction failed to execute"); 
//     }
//     else { 
//         printf("Execution Complete.\n"); 
//     }

//     printf("View regs: initial cpu, expected cpu\n");
//     view_regs(&build_test.initial_cpu, &build_test.expected_cpu);
//     printf("View regs: working cpu, expected cpu\n");
//     view_regs(&working_cpu, &build_test.expected_cpu);

//     reg_compare2(&working_cpu, &build_test.expected_cpu);

//     bool rg_return = reg_compare(&working_cpu, &build_test.expected_cpu);

//     else {
//         printf("[FAILED] Name:[%s] OPCODE: [0x%02X] Test failed. Registers did not match. \n", build_test.name, build_test.opcode);
//         logging_log("[FAILED] Name:[%s] OPCODE: [0x%02X]\n", build_test.name, build_test.opcode);
//         // Don't like this... This is a HARD CODED Test. I need to make this modular.
//     }
// }




void entry_test_case(){
    instruction_T instrc;
    instrc.opcode = 0x5C;           // Placeholder, so it's initialized. (This probably should be NOP)
    instrc.operand1 = 0x00;
    instrc.operand2 = 0x00;

    // IE: "Run PUSH DE, Run 5-6 LDs, Run POP DE etc"
    uint8_t concentraaaatee[] = { 0xA0, 0xA1, 0xA2, /* ......... etc etc */ };

    // Special 8bit Arithmatic:
    uint8_t arith8bit[] = { 0xC6, 0xCE, 0xD6, 0xDE };   // ADD, ADC, SUB, SBC.
    uint8_t arith8bit_2[] = {
        0x04, 0x14, 0x24, 0x34,     // INC B, D, H, [HL]
        0x0C, 0x1C, 0x2C, 0x3C,     // INC C, E, L, A
        0x05, 0x15, 0x25, 0x35,   // DEC B, D, H, [HL]
        0x0D, 0x1D, 0x2D, 0x3D  // DEC C, E, L, A
        };

    uint8_t arith16bit[] = {
        0x03, 0x13, 0x23, 0x33,
        0x09, 0x19, 0x29, 0x39,
        0x0B, 0x1B, 0x2B, 0x3B
    };
    // Special Logic Ops:
    uint8_t logic_ops[] = { 0xE6, 0xEE, 0xF6, 0xFE };   // AND, XOR, OR, CP.

    // instrc.operand1 = 0x84;
    // instrc.operand2 = 0x00;
    // for (int i = 0x0; i <= 0x03; i++) {
    //     instrc.opcode = arith8bit[i];    // ADD and SUB
    //     unt_tcase_builder(instrc);
    // }
    // for (int i = 0x0; i <= 0x03; i++) {
    //     instrc.opcode = logic_ops[i];    // AND, OR, XOR...
    //     unt_tcase_builder(instrc);
    // }
    // for (int i = 0x0; i <= 0x0F; i++) {
    //     instrc.opcode = arith8bit_2[i]; // INC / DEC r8
    //     unt_tcase_builder(instrc);
    // }
    // for (int i = 0x0; i <= 0x0B; i++) {
    //     instrc.opcode = arith16bit[i];    // INC r16 | DEC r16 | ADD r16
    //     unt_tcase_builder(instrc);
    // }
}
