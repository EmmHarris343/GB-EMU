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

#include "cpu_test_p2.h"


extern CPU cpu_reg_simple_tstate;


#define tFLAG_Z 0x80  // 1000 0000
#define tFLAG_N 0x40  // 0100 0000
#define tFLAG_H 0x20  // 0010 0000
#define tFLAG_C 0x10  // 0001 0000


#define FZ 0x80  // 1000 0000
#define FN 0x40  // 0100 0000
#define FH 0x20  // 0010 0000
#define FC 0x10  // 0001 0000


const char* reg_names[8] = { "B", "C", "D", "E", "H", "L", "[HL]", "A" };

////// TODO:
//  MOVE THESE TO cpu_test_p2??
void check_flags(CPU* inital_cpu, CPU* expected_cpu) {
    printf("\n:CPU: === Registers === \n");
    (inital_cpu->reg.F & FLAG_Z) ? printf("  Z Flag set | ") : printf("  Z Flag NOT set | ");
    (inital_cpu->reg.F & FLAG_N) ? printf("  N Flag set | ") : printf("  N Flag NOT set | ");
    (inital_cpu->reg.F & FLAG_H) ? printf("  H Flag set | ") : printf("  H Flag NOT set | ");
    (inital_cpu->reg.F & FLAG_C) ? printf("  C Flag set") : printf("  C Flag NOT set");
}

void cpar_reg_long(CPU* b4, CPU* l8) {
    printf("\n:CPU: === Registers === \n");
    printf("  X: [B4/L8]\n");
    printf("  A: [0x%02X/ 0x%02X]\n", b4->reg.A, l8->reg.A);
    printf("  B: [0x%02X/ 0x%02X] | C: [0x%02X/ 0x%02X]\n", b4->reg.B, l8->reg.B, b4->reg.C, l8->reg.C);
    printf("  D: [0x%02X/ 0x%02X] | E: [0x%02X/ 0x%02X]\n", b4->reg.D, l8->reg.D, b4->reg.E, l8->reg.E);
    printf("  H: [0x%02X/ 0x%02X] | L: [0x%02X/ 0x%02X]\n", b4->reg.H, l8->reg.H, b4->reg.L, l8->reg.L);
    //printf("\n");
    printf("--------\nRaw Hex of FLAGS: [0x%02X/ 0x%02X]\n--------\n", b4->reg.F, l8->reg.F);
}

void cpar_reg_check(CPU *b4, CPU *l8) {
    (b4->reg.AF == l8->reg.AF) ? printf("[PASS]=AF :: ") : printf("[FAIL] AF\n");
    (b4->reg.BC == l8->reg.BC) ? printf("[PASS]=BC :: ") : printf("[FAIL] BC\n");
    (b4->reg.DE == l8->reg.DE) ? printf("[PASS]=DE :: ") : printf("[FAIL] DE\n");
    (b4->reg.HL == l8->reg.HL) ? printf("[PASS]=HL :: ") : printf("[FAIL] HL\n");
    (b4->reg.PC == l8->reg.PC) ? printf("[PASS]=PC :: ") : printf("[FAIL] PC\n");
    (b4->reg.SP == l8->reg.SP) ? printf("[PASS]=SP\n") : printf("[FAIL] SP\n");
    //(working->reg. == expected->reg.AF) ? printf("[PASS] AF\n") : printf("[FAIL] AF\n");
}


// void test_arith8(CPU *ecpu, CPU *icpu, instruction_T inst) {
//     uint8_t op = inst.opcode;
//     uint8_t phl_val = 0xD0;

//     uint8_t sc = op & 0x07;           // r8
//     uint8_t spl = (op >> 3) & 0x07;   // * A, n8
    
//     uint8_t *rgl[8] = {
//         &ecpu->reg.B, &ecpu->reg.C, &ecpu->reg.D, &ecpu->reg.E, 
//         &ecpu->reg.H, &ecpu->reg.L, &phl_val, &ecpu->reg.A
//     };

    
//     if ((op & 0xFE) == 0x80){   // ADD
//         prediction_add8(*rgl[7], *rgl[sc], ecpu);
//         // prediction doesn't actually give me expected Register values
//     }
//     if ((op & 0xC7) == 0x04){   // INC
//         prediction_inc8(*rgl[sc], ecpu);
//     }
// }

// void to_test() {
//     Test_Case_t base;
//     instruction_T create_inst;
//     create_inst.opcode = 0x82;

//     base.initial_cpu = cpu_reg_simple_tstate;
//     base.expected_cpu = cpu_reg_simple_tstate;

//     test_arith8(&base.expected_cpu, &base.expected_cpu, create_inst);
// }

// void add8_suite(CPU *ecpu, CPU *icpu) {
//     // Make Add test for each case.

//     uint8_t op = 0; // placeholder
//     uint8_t phl_val = 0xD0;
//     uint8_t sc = op & 0x07;

//     uint8_t *rgl[8] = {
//         &ecpu->reg.B, &ecpu->reg.C, &ecpu->reg.D, &ecpu->reg.E, 
//         &ecpu->reg.H, &ecpu->reg.L, &phl_val, &ecpu->reg.A
//     };

//     // ADD A, r8

//     // For ADD A, B:
//     // Half flag:
//         // Set A = 0x4E
//         // Set B = 0x02
//         // A = A0
//         // Will set Half Flag. (Will not set 0)
//     // Carry Flag:
//         // Set A = 0xF4
//         // Set B = 0x34
//         // Will equal A = (128 16bit) 28
//         // Will rollover setting C, but not H. 
//     // Z + Carry:
//         // Set A = 0xF9
//         // Set B = 0x07
//         // A = 0 (100)
//         // Expected: will rollover (H + C),  and set z flag

// }


void run_add8(add8_test_case *tc) {

}

void set_flag_byval(CPU *cpu, uint8_t f) {
    cpu->reg.F = f;
}

void set_reg_val(CPU *cpu, uint8_t op, uint8_t rval) {
    uint8_t sc = (op & 0x07);
    uint8_t phl_val = 0;

    uint8_t *rgl[8] = {
        &cpu->reg.B, &cpu->reg.C, &cpu->reg.D, &cpu->reg.E, 
        &cpu->reg.H, &cpu->reg.L, &phl_val, &cpu->reg.A
    };

    *rgl[sc] = rval;
}

void set_reg_a(CPU *cpu, uint8_t rval) {
    cpu->reg.A = rval;
}

void add_zcase(add8_test_case *z_case) {
    z_case->initial_A = 0x00;
    z_case->expected_A = 0x00;
    z_case->from_reg_val = 0x00;     // actual register is calculated by opcode
    z_case->operand = 0x00;
    z_case->memory_val = 0x00;
    z_case->expected_flags = 0x80;   // 1 0 0 0 (Z, N, H, C)
}

void add_hcase(add8_test_case *h_case) {
    h_case->initial_A = 0x0D;
    h_case->expected_A = 0x10;
    h_case->from_reg_val = 0x03;     // actual register is calculated by opcode
    h_case->operand = 0x03;
    h_case->memory_val = 0x03;
    h_case->expected_flags = 0x20;   // 0 0 1 0 (Z, N, H, C)
}

void add_ccase(add8_test_case *c_case) {
    c_case->initial_A = 0xFD;
    c_case->expected_A = 0xCD;
    c_case->from_reg_val = 0xD0;     // actual register is calculated by opcode
    c_case->operand = 0xD0;
    c_case->memory_val = 0xD0;
    c_case->expected_flags = 0x10;   // 0 0 0 1 (Z, N, H, C)
}

void add_rlvr_case(add8_test_case *rv_case) {
    rv_case->initial_A = 0xFD;
    rv_case->expected_A = 0x00;
    rv_case->from_reg_val = 0x03;     // actual register is calculated by opcode
    rv_case->operand = 0x03;
    rv_case->memory_val = 0x03;
    rv_case->expected_flags = 0xB0;   // 1 0 1 1 (Z, N, H, C)
}


void add_assign_cpu_tstate(CPU *rcpu, CPU *ecpu, instruction_T inst, add8_test_case tcase) {
    // A Registers
    set_reg_a(rcpu, tcase.initial_A);
    set_reg_a(ecpu, tcase.expected_A);

    // r8 Registers
    set_reg_val(rcpu, inst.opcode, tcase.from_reg_val);
    set_reg_val(ecpu, inst.opcode, tcase.from_reg_val);

    // set expected flags
    set_flag_byval(ecpu, tcase.expected_flags);

    ecpu->reg.PC ++; // Advance the PC.

    // n8 ADD, ADC, ... AND, XOR, CP etc
    if ((inst.opcode & 0xC7) == 0xC6) { }

    // reg / [hl]:
    if ((inst.opcode & 0xC0) == 0x80) {
        uint8_t sc = (inst.opcode >>3) & 0x07;
        uint8_t regv = inst.opcode & 0x07;
        if (regv == 6) {
            // HL
            external_write(ecpu->reg.HL, tcase.memory_val);
        }
        else {
            // r8 registers
        }
    }
}

void build_add8() {
    CPU ecpu;
    CPU rcpu;

    ecpu = cpu_reg_simple_tstate;
    rcpu = cpu_reg_simple_tstate;

    //Test_Case_t base;
    instruction_T inst;
    inst.opcode = 0x82;

    /// TODO:
    // I need 4 "tests" for each ADD Opcode
    // Z set test. H set test, C set test, all but N set test.
    // (With HL and n8 options too)

    add8_test_case z_case;
    add_zcase(&z_case);

    add8_test_case c_case;
    add_ccase(&c_case);

    add8_test_case h_case;
    add_hcase(&h_case);

    add8_test_case rlvr_case;
    add_rlvr_case(&rlvr_case);


    add8_test_case *c_v[4] = {
        &z_case, &c_case, &h_case, &rlvr_case
    };



    // PUT THIS IN A LOOP:

    // builld z_case test
    for (int i = 0; i < 4; i++) {
        printf("Running each test case for OPCODE\n");
        rcpu = cpu_reg_simple_tstate;
        ecpu = cpu_reg_simple_tstate;

        add_assign_cpu_tstate(&rcpu, &ecpu, inst, *c_v[i]);

        execute_test(&rcpu, inst);
        cpar_reg_check(&ecpu, &rcpu);
        cpar_reg_long(&ecpu, &rcpu);
    }
}





void entry_test_case(){
    printf("Start Test Case \n");
    printf("forcing a rebuild...\n");
    build_add8();
    
    //logging_log("[PASS] Name:[%s] OPCODE: [0x%02X]\n", ld_test.name, ld_test.opcode);   // Save to log file
}