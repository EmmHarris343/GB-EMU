#include "cpu_test.h"
#include <stddef.h>
//#include <system_error>
#ifdef ENABLE_TESTS
// This is for The build flag.

#include <stdio.h>
#include "../src/cpu.h"

#endif


#include <stdio.h>
#include "../src/cpu.h"

//#include "../src/logger.h"

#include "0_alu/alu_test.h"


extern CPU cpu_reg_simple_tstate;

//char* reg_names[8] = { "B", "C", "D", "E", "H", "L", "[HL]", "A" };



void bld_arith8bit_tests() {
    instruction_T inst;
    inst.opcode = 0x00; // Will default to NOP

    // Special ADD, SUB, (n8)
    uint8_t arith8_spec[] = { 0xC6, 0xCE, 0xD6, 0xDE };   // ADD, ADC, SUB, SBC (n8).

    for (int i = 0x80; i <= 0x87; i++) {        // 80 - 9F (IS all ADD, ADC, SUB, SBC.... A, r8 & A, [HL] instructions)
        inst.opcode = i;
        build_add8(inst);
    }
    for (int i = 0x88; i <= 0x8F; i++) {        // 80 - 9F (IS all ADD, ADC, SUB, SBC.... A, r8 & A, [HL] instructions)
        inst.opcode = i;
        build_adc8(inst);
    }

    for (int i = 0x90; i <= 0x97; i++) {        // 80 - 9F (IS all ADD, ADC, SUB, SBC.... A, r8 & A, [HL] instructions)
        inst.opcode = i;
        build_sub8(inst);
    }


    // for (int i = 0x80; i <= 0x9F; i++) {        // 80 - 9F (IS all ADD, ADC, SUB, SBC.... A, r8 & A, [HL] instructions)
    //     inst.opcode = i;
    //     build_add8(inst);
    // }

    // for (int i = 0x0; i <= 0x03; i++) {
    //     inst.opcode = arith8_spec[i];    // ADD and SUB
    //     build_add8(inst);
    // }
}

// void bld_logic_ops_tests() {
//     instruction_T inst;
//     inst.opcode = 0x00; // Will default to NOP

//     // Special Logic Ops:
//     uint8_t logic_ops[4] = { 0xE6, 0xEE, 0xF6, 0xFE };   // AND, XOR, OR, CP.

//     for (int i = 0xA0; i <= 0xBF; i++) {        // 80 - 9F (Is all AND, XOR, OR, CP.... A, r8 instructions)
//         inst.opcode = i;

//     }

//     for (int i = 0x0; i <= 0x03; i++) {
//         inst.opcode = logic_ops[i];    // AND, OR, XOR...
//         //unt_tcase_logic(inst);
//     }
// }

// void bld_arth16_test() {
//     uint8_t arith16bit[] = {
//         0x03, 0x13, 0x23, 0x33,
//         0x09, 0x19, 0x29, 0x39,
//         0x0B, 0x1B, 0x2B, 0x3B
//     };
// }


void entry_test_case(){
    printf("Start Test Case \n");
    printf("forcing a rebuild...\n");
    bld_arith8bit_tests();

    // Legit putting this here cause it makes me laugh..
    uint8_t concentraaaatee[] = { 0x00 };
    
    //logging_log("[PASS] Name:[%s] OPCODE: [0x%02X]\n", ld_test.name, ld_test.opcode);   // Save to log file
}