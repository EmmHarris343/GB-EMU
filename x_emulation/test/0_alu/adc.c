//#include <system_error>
#ifdef ENABLE_TESTS
// This is for The build flag.

#include <stdio.h>
#include "../src/cpu.h"

#endif

#include "../cpu_test.h"

#include "../common/utile.h"
#include "../../src/cpu_instructions.h"

extern CPU cpu_reg_simple_tstate;
extern char* reg_names[8];

char* reg_names[8] = { "B", "C", "D", "E", "H", "L", "[HL]", "A" };

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