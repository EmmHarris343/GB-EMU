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


char* reg_names[8] = { "B", "C", "D", "E", "H", "L", "[HL]", "A" };

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

// Checks the Register states, if any fail will return fail.
bool check_reg(CPU *rcpu, CPU *ecpu) {
    return \
    rcpu->reg.AF == ecpu->reg.AF &&
    rcpu->reg.BC == ecpu->reg.BC &&
    rcpu->reg.DE == ecpu->reg.DE &&
    rcpu->reg.HL == ecpu->reg.HL &&
    rcpu->reg.SP == ecpu->reg.SP &&
    rcpu->reg.PC == ecpu->reg.PC;
}


void check_toLog(CPU *rcpu, CPU *ecpu, instruction_T inst, char *name) {
    bool rg_return = check_reg(rcpu, ecpu);
    if (rg_return) {
        printf("[PASS] Name:[%s] OPCODE: [0x%02X]\n", name, inst.opcode);
        logging_log("[PASS] Name:[%s] OPCODE: [0x%02X]\n", name, inst.opcode);
    }
    else {
        printf("[FAILED] Name:[%s] OPCODE: [0x%02X] Test failed. Registers did not match. \n", name, inst.opcode);
        logging_log("[FAILED] Name:[%s] OPCODE: [0x%02X]\n", name, inst.opcode);
        // Don't like this... This is a HARD CODED Test. I need to make this modular.
    }
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
    z_case->sub_tname = "zero";
    z_case->initial_A = 0x00;
    z_case->expected_A = 0x00;
    z_case->double_A_A = 0x00;  // The ADD A, A. Case
    z_case->from_val = 0x00;    // actual register is calculated by opcode
    z_case->expected_flags = 0x80;   // 1 0 0 0 (Z, N, H, C)
}

void add_hcase(add8_test_case *h_case) {
    h_case->sub_tname = "half-carry";
    h_case->initial_A = 0x08;
    h_case->expected_A = 0x10;
    h_case->double_A_A = 0x8;   // The ADD A, A. Case
    h_case->from_val = 0x08;    // actual register is calculated by opcode
    h_case->expected_flags = 0x20;   // 0 0 1 0 (Z, N, H, C)
}

void add_ccase(add8_test_case *c_case) {
    c_case->sub_tname = "carry";
    c_case->initial_A = 0xF0;
    c_case->expected_A = 0xC0;
    c_case->double_A_A = 0xE0;   // The ADD A, A. Case
    c_case->from_val = 0xD0;     // actual register is calculated by opcode
    c_case->expected_flags = 0x10;   // 0 0 0 1 (Z, N, H, C)
}

void add_rlvr_case(add8_test_case *rv_case) {
    rv_case->sub_tname = "rollover";
    rv_case->initial_A = 0xFD;
    rv_case->expected_A = 0x34;
    rv_case->double_A_A = 0x9A;   // The ADD A, A. Case
    rv_case->from_val = 0x37;     // actual register is calculated by opcode
    rv_case->expected_flags = 0x30;   // 1 0 1 1 (Z, N, H, C)
}


void add_assign_cpu_tstate(CPU *rcpu, CPU *ecpu, instruction_T inst, add8_test_case tcase) {
    if ((inst.opcode & 0xC0) == 0x80) {             // [inst.opcode & 0xC0] ensures opcode between 0x80 - 0xBF
        uint8_t sc = (inst.opcode >>3) & 0x07;
        uint8_t regv = inst.opcode & 0x07;

        if (regv < 6) {
            set_reg_val(rcpu, inst.opcode, tcase.from_val);
            set_reg_val(ecpu, inst.opcode, tcase.from_val);

            // A Registers
            set_reg_a(rcpu, tcase.initial_A);
            set_reg_a(ecpu, tcase.expected_A);
        }
        if (regv == 6) {
            // HL
            external_write(ecpu->reg.HL, tcase.from_val);

            // A Registers
            set_reg_a(rcpu, tcase.initial_A);
            set_reg_a(ecpu, tcase.expected_A);
        }
        if (regv == 7) {
            // ADD A, A
            set_reg_a(rcpu, tcase.double_A_A);
            set_reg_a(ecpu, tcase.expected_A);
        }
        // set expected flags
        set_flag_byval(ecpu, tcase.expected_flags);

        ecpu->reg.PC ++; // Advance the PC.
    }
    // n8 ADD, ADC, ... AND, XOR, CP etc
    if ((inst.opcode & 0xC7) == 0xC6) { }   // The special ADD A, n8
    // r8 Registers first (Incase it lands on ADD A, A. So [expected A] won't get over written by the [from val])

}

void build_add8(instruction_T inst) {
    CPU ecpu;
    CPU rcpu;

    add8_test_case z_case;
    add_zcase(&z_case);

    add8_test_case c_case;
    add_ccase(&c_case);

    add8_test_case h_case;
    add_hcase(&h_case);

    add8_test_case rlvr_case;
    add_rlvr_case(&rlvr_case);

    details_T deets;
    
    deets.inst.opcode = inst.opcode;
    char spec_name[100];
    deets.mnemonic = "ADD";
    uint8_t rgv = inst.opcode & 0x07;
    deets.from_name = reg_names[rgv];
    deets.subname = "0";
    
    size_t sz = 50;

    add8_test_case *tc_v[4] = {
        &z_case, &h_case, &c_case, &rlvr_case
    };
    for (int i = 0; i < 4; i++) {
        rcpu = cpu_reg_simple_tstate;
        ecpu = cpu_reg_simple_tstate;

        deets.subname = tc_v[i]->sub_tname;

        add_assign_cpu_tstate(&rcpu, &ecpu, inst, *tc_v[i]);
        snprintf(spec_name, sz, "%s A, %s-%s", deets.mnemonic, deets.from_name, deets.subname);

        printf("Get subname? :: %s\n", deets.subname);

        execute_test(&rcpu, inst);
        cpar_reg_check(&ecpu, &rcpu);
        check_toLog(&rcpu, &ecpu, inst, spec_name);
        cpar_reg_long(&ecpu, &rcpu);
    }
}


// ADC is a bit different.. cause... 
// It rolls the state of C into the value. 

// uint8_t carry_val = (cpu->reg.F & FLAG_C) ? 1 : 0;
// (cpu->reg.A + op_r8 + carry_val);

void adc_zcase(adc8_test_case *z_case) {
    z_case->sub_tname = "zero";
    z_case->initial_A = 0xF0;
    z_case->expected_A = 0x20;
    z_case->carry_state = 0;
    z_case->from_val = 0x30;
    z_case->expected_flags = 0x90;  // 1 0 0 1
}

void adc_hcase(adc8_test_case *h_case) {
    h_case->sub_tname = "half-carry";
    h_case->initial_A = 0x0E;
    h_case->expected_A = 0x12;
    h_case->carry_state = 1;
    h_case->from_val = 0x30;
    h_case->expected_flags = 0x20;  // 0 0 1 0
}

void adc_ccase(adc8_test_case *c_case) {
    c_case->sub_tname = "carry";
    c_case->initial_A = 0xF2;
    c_case->expected_A = 0x2B;
    c_case->carry_state = 1;
    c_case->from_val = 0x38;    // F2 + 38 = 2A (with rollover) + 1 (carry flag) = 0x2B.
    c_case->expected_flags = 0x10;  // 0 0 0 1
}

void adc_rlvr_case(adc8_test_case *rv_case) {
    rv_case->sub_tname = "rollover";
    rv_case->initial_A = 0xFE;
    rv_case->expected_A = 0x00;
    rv_case->carry_state = 1;
    rv_case->from_val = 0x01;    // FE + 1 + 1 = 100 (or 00)
    rv_case->expected_flags = 0x10;  // 1 0 1 1
}

void adc_assign_cpu_tstate(CPU *rcpu, CPU *ecpu, instruction_T inst, adc8_test_case tcase) {
    // r8 Registers first (Incase it lands on ADD A, A. So [expected A] won't get over written by the [from val])
    set_reg_val(rcpu, inst.opcode, tcase.from_val);
    set_reg_val(ecpu, inst.opcode, tcase.from_val);

    // A Registers
    set_reg_a(rcpu, tcase.initial_A);
    set_reg_a(ecpu, tcase.expected_A);

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
            external_write(ecpu->reg.HL, tcase.from_val);
        }
    }
}

void build_adc8(instruction_T inst) {
    CPU ecpu;
    CPU rcpu;
    adc8_test_case z_case;
    adc_zcase(&z_case);

    adc8_test_case c_case;
    adc_ccase(&c_case);

    adc8_test_case h_case;
    adc_hcase(&h_case);

    adc8_test_case rlvr_case;
    adc_rlvr_case(&rlvr_case);

    adc8_test_case *tc_v[4] = {
        &z_case, &c_case, &h_case, &rlvr_case
    };
    for (int i = 0; i < 4; i++) {
        rcpu = cpu_reg_simple_tstate;
        ecpu = cpu_reg_simple_tstate;

        adc_assign_cpu_tstate(&rcpu, &ecpu, inst, *tc_v[i]);

        execute_test(&rcpu, inst);
        cpar_reg_check(&ecpu, &rcpu);
        cpar_reg_long(&ecpu, &rcpu);
    }

}

void bld_arith8bit_tests() {
    instruction_T inst;
    inst.opcode = 0x00; // Will default to NOP

    // Special ADD, SUB, (n8)
    uint8_t arith8_spec[] = { 0xC6, 0xCE, 0xD6, 0xDE };   // ADD, ADC, SUB, SBC (n8).

    for (int i = 0x80; i <= 0x87; i++) {        // 80 - 9F (IS all ADD, ADC, SUB, SBC.... A, r8 & A, [HL] instructions)
        inst.opcode = i;
        build_add8(inst);
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

void bld_logic_ops_tests() {
    instruction_T inst;
    inst.opcode = 0x00; // Will default to NOP

    // Special Logic Ops:
    uint8_t logic_ops[4] = { 0xE6, 0xEE, 0xF6, 0xFE };   // AND, XOR, OR, CP.

    for (int i = 0xA0; i <= 0xBF; i++) {        // 80 - 9F (Is all AND, XOR, OR, CP.... A, r8 instructions)
        inst.opcode = i;

    }

    for (int i = 0x0; i <= 0x03; i++) {
        inst.opcode = logic_ops[i];    // AND, OR, XOR...
        //unt_tcase_logic(inst);
    }
}

void bld_arth16_test() {
    uint8_t arith16bit[] = {
        0x03, 0x13, 0x23, 0x33,
        0x09, 0x19, 0x29, 0x39,
        0x0B, 0x1B, 0x2B, 0x3B
    };
}


void entry_test_case(){
    printf("Start Test Case \n");
    printf("forcing a rebuild...\n");
    bld_arith8bit_tests();

    // Legit putting this here cause it makes me laugh..
    uint8_t concentraaaatee[] = { 0x00 };
    
    //logging_log("[PASS] Name:[%s] OPCODE: [0x%02X]\n", ld_test.name, ld_test.opcode);   // Save to log file
}