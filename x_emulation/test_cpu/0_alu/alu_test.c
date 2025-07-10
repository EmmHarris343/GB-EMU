//#include <system_error>
#ifdef ENABLE_TESTS
// This is for The build flag.

#include <stdio.h>
#include "../src/cpu.h"

#endif

#include "alu_test.h"       // This includes all other needed includes



extern CPU cpu_reg_simple_tstate;
extern char* reg_names[8];

//char* reg_names[8] = { "B", "C", "D", "E", "H", "L", "[HL]", "A" };



void assign_cpu_tstate(CPU *rcpu, CPU *ecpu, instruction_T inst, alu8_test_case tcase) {
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
        // (Self ADD) -> ADD A, A
        set_reg_a(rcpu, tcase.double_A_A);
        set_reg_a(ecpu, tcase.expected_A);
    }
    // set expected flags
    set_flag_byval(ecpu, tcase.expected_flags);

    ecpu->reg.PC ++; // Advance the PC.
}


/// ADD: Tests

void add_zcase(alu8_test_case *z_case) {
    z_case->sub_tname = "zero";
    z_case->initial_A = 0x00;
    z_case->expected_A = 0x00;
    z_case->double_A_A = 0x00;  // The ADD A, A. Case
    z_case->from_val = 0x00;    // actual register is calculated by opcode
    z_case->expected_flags = 0x80;   // 1 0 0 0 (Z, N, H, C)
}

void add_hcase(alu8_test_case *h_case) {
    h_case->sub_tname = "half-carry";
    h_case->initial_A = 0x08;
    h_case->expected_A = 0x10;
    h_case->double_A_A = 0x8;   // The ADD A, A. Case
    h_case->from_val = 0x08;    // actual register is calculated by opcode
    h_case->expected_flags = 0x20;   // 0 0 1 0 (Z, N, H, C)
}

void add_ccase(alu8_test_case *c_case) {
    c_case->sub_tname = "carry";
    c_case->initial_A = 0xF0;
    c_case->expected_A = 0xC0;
    c_case->double_A_A = 0xE0;   // The ADD A, A. Case
    c_case->from_val = 0xD0;     // actual register is calculated by opcode
    c_case->expected_flags = 0x10;   // 0 0 0 1 (Z, N, H, C)
}

void add_rlvr_case(alu8_test_case *rv_case) {
    rv_case->sub_tname = "rollover";
    rv_case->initial_A = 0xFD;
    rv_case->expected_A = 0x34;
    rv_case->double_A_A = 0x9A;   // The ADD A, A. Case
    rv_case->from_val = 0x37;     // actual register is calculated by opcode
    rv_case->expected_flags = 0x30;   // 1 0 1 1 (Z, N, H, C)
}

void build_add8(instruction_T inst) {
    CPU ecpu;
    CPU rcpu;
    alu8_test_case z_case;
    add_zcase(&z_case);

    alu8_test_case c_case;
    add_ccase(&c_case);

    alu8_test_case h_case;
    add_hcase(&h_case);

    alu8_test_case rlvr_case;
    add_rlvr_case(&rlvr_case);    

    /// TODO: Clean this up because it's really messy.
    details_T deets;
    deets.inst.opcode =  inst.opcode;
    char spec_name[100];
    deets.mnemonic = "ADD";
    uint8_t rgv = inst.opcode & 0x07;
    deets.from_name = reg_names[rgv];
    deets.subname = "0";
    
    size_t sz = 50;

    alu8_test_case *tc_v[4] = {
        &z_case, &h_case, &c_case, &rlvr_case
    };
    for (int i = 0; i < 4; i++) {
        rcpu = cpu_reg_simple_tstate;
        ecpu = cpu_reg_simple_tstate;

        deets.subname = tc_v[i]->sub_tname;

        assign_cpu_tstate(&rcpu, &ecpu, inst, *tc_v[i]);
        snprintf(spec_name, sz, "%s A, %s-%s", deets.mnemonic, deets.from_name, deets.subname);

        printf("Get subname? :: %s\n", deets.subname);

        execute_test(&rcpu, inst);
        cpar_reg_check(&ecpu, &rcpu);
        check_toLog(&rcpu, &ecpu, inst, spec_name);
        cpar_reg_long(&ecpu, &rcpu);
    }
}

void adc_zcase(alu8_test_case *z_case) {
    z_case->sub_tname = "zero";
    z_case->initial_A = 0x00;
    z_case->expected_A = 0x00;
    z_case->double_A_A = 0x00;
    z_case->from_val = 0x00;
    z_case->carry_state = 0;
    z_case->expected_flags = 0x80;   // 1 0 0 0 (Z, N, H, C)
}

void adc_hcase(alu8_test_case *h_case) {
    h_case->sub_tname = "half-carry";
    h_case->initial_A = 0x0E;
    h_case->expected_A = 0x12;
    h_case->double_A_A = 0x00;
    h_case->carry_state = 1;
    h_case->from_val = 0x30;
    h_case->expected_flags = 0x20;  // 0 0 1 0
}

void adc_ccase(alu8_test_case *c_case) {
    c_case->sub_tname = "carry";
    c_case->initial_A = 0xF2;
    c_case->expected_A = 0x2B;
    c_case->carry_state = 1;
    c_case->double_A_A = 0x00;
    c_case->from_val = 0x38;    // F2 + 38 = 2A (with rollover) + 1 (carry flag) = 0x2B.
    c_case->expected_flags = 0x10;  // 0 0 0 1
}

void adc_rlvr_case(alu8_test_case *rv_case) {
    rv_case->sub_tname = "rollover";
    rv_case->initial_A = 0xFE;
    rv_case->expected_A = 0x00;
    rv_case->carry_state = 1;
    rv_case->double_A_A = 0x00;
    rv_case->from_val = 0x01;    // FE + 1 + 1 = 100 (or 00)
    rv_case->expected_flags = 0x10;  // 1 0 1 1
}

void build_adc8(instruction_T inst) {
    CPU ecpu;
    CPU rcpu;
    alu8_test_case z_case;
    adc_zcase(&z_case);

    alu8_test_case c_case;
    adc_ccase(&c_case);

    alu8_test_case h_case;
    adc_hcase(&h_case);

    alu8_test_case rlvr_case;
    adc_rlvr_case(&rlvr_case);

    /// TODO: Clean this up because it's really messy.
    details_T deets;
    deets.inst.opcode =  inst.opcode;
    char spec_name[100];
    deets.mnemonic = "ADC";
    uint8_t rgv = inst.opcode & 0x07;
    deets.from_name = reg_names[rgv];
    deets.subname = "0";
    
    size_t sz = 50;

    alu8_test_case *tc_v[4] = {
        &z_case, &h_case, &c_case, &rlvr_case
    };
    for (int i = 0; i < 4; i++) {
        rcpu = cpu_reg_simple_tstate;
        ecpu = cpu_reg_simple_tstate;

        deets.subname = tc_v[i]->sub_tname;

        assign_cpu_tstate(&rcpu, &ecpu, inst, *tc_v[i]);
        snprintf(spec_name, sz, "%s A, %s-%s", deets.mnemonic, deets.from_name, deets.subname);

        printf("Get subname? :: %s\n", deets.subname);

        execute_test(&rcpu, inst);
        cpar_reg_check(&ecpu, &rcpu);
        check_toLog(&rcpu, &ecpu, inst, spec_name);
        cpar_reg_long(&ecpu, &rcpu);
    }
}



//-------------------------------------------
/// SUB: Tests

//////// FAKE, Placeholders

void sub_zcase(alu8_test_case *z_case) {
    z_case->sub_tname = "zero";
    z_case->initial_A = 0x00;
    z_case->expected_A = 0x00;
    z_case->double_A_A = 0x00;
    z_case->from_val = 0x00;    
    z_case->expected_flags = 0xC0;  // 1 1 0 0
}

void sub_hcase(alu8_test_case *h_case) {
    h_case->sub_tname = "half-carry";
    h_case->initial_A = 0x10;
    h_case->expected_A = 0x0F;
    h_case->double_A_A = 0x00;
    h_case->from_val = 0x30;
    h_case->expected_flags = 0x60;  // 0 1 1 0
}

void sub_ccase(alu8_test_case *c_case) {
    c_case->sub_tname = "carry";
    c_case->initial_A = 0x40;
    c_case->from_val = 0x50;
    c_case->expected_A = 0xF0;
    c_case->double_A_A = 0x00;
    c_case->expected_flags = 0x50;  // 0 1 0 1
}

void sub_rlvr_case(alu8_test_case *rv_case) {
    rv_case->sub_tname = "rollover";
    rv_case->initial_A = 0x34;
    rv_case->from_val = 0xFD;
    rv_case->expected_A = 0x37;
    rv_case->double_A_A = 0x00;
    rv_case->expected_flags = 0x70;  // 0 1 1 1
}


void build_sub8(instruction_T inst) {
    CPU ecpu;
    CPU rcpu;
    alu8_test_case z_case;
    sub_zcase(&z_case);

    alu8_test_case c_case;
    sub_ccase(&c_case);

    alu8_test_case h_case;
    sub_hcase(&h_case);

    alu8_test_case rlvr_case;
    sub_rlvr_case(&rlvr_case);

    /// TODO: Clean this up because it's really messy.
    details_T deets;
    deets.inst.opcode =  inst.opcode;
    char spec_name[100];
    deets.mnemonic = "SUB";
    uint8_t rgv = inst.opcode & 0x07;
    deets.from_name = reg_names[rgv];
    deets.subname = "0";
    
    size_t sz = 50;

    alu8_test_case *tc_v[4] = {
        &z_case, &h_case, &c_case, &rlvr_case
    };
    for (int i = 0; i < 4; i++) {
        rcpu = cpu_reg_simple_tstate;
        ecpu = cpu_reg_simple_tstate;

        deets.subname = tc_v[i]->sub_tname;

        assign_cpu_tstate(&rcpu, &ecpu, inst, *tc_v[i]);
        snprintf(spec_name, sz, "%s A, %s-%s", deets.mnemonic, deets.from_name, deets.subname);

        printf("Get subname? :: %s\n", deets.subname);

        execute_test(&rcpu, inst);
        cpar_reg_check(&ecpu, &rcpu);
        check_toLog(&rcpu, &ecpu, inst, spec_name);
        cpar_reg_long(&ecpu, &rcpu);
    }
}







////// FAKE, placeholders.

void sbc_zcase(alu8_test_case *z_case) {
    z_case->sub_tname = "zero";
    z_case->initial_A = 0xF0;
    z_case->expected_A = 0x20;
    z_case->carry_state = 0;
    z_case->from_val = 0x30;
    z_case->expected_flags = 0x90;  // 1 0 0 1
}

void sbc_hcase(alu8_test_case *h_case) {
    h_case->sub_tname = "half-carry";
    h_case->initial_A = 0x0E;
    h_case->expected_A = 0x12;
    h_case->carry_state = 1;
    h_case->from_val = 0x30;
    h_case->expected_flags = 0x20;  // 0 0 1 0
}

void sbc_ccase(alu8_test_case *c_case) {
    c_case->sub_tname = "carry";
    c_case->initial_A = 0xF2;
    c_case->expected_A = 0x2B;
    c_case->carry_state = 1;
    c_case->from_val = 0x38;    // F2 + 38 = 2A (with rollover) + 1 (carry flag) = 0x2B.
    c_case->expected_flags = 0x10;  // 0 0 0 1
}

void sbc_rlvr_case(alu8_test_case *rv_case) {
    rv_case->sub_tname = "rollover";
    rv_case->initial_A = 0xFE;
    rv_case->expected_A = 0x00;
    rv_case->carry_state = 1;
    rv_case->from_val = 0x01;    // FE + 1 + 1 = 100 (or 00)
    rv_case->expected_flags = 0x10;  // 1 0 1 1
}


void sbc_assign_cpu_tstate(CPU *rcpu, CPU *ecpu, instruction_T inst, sbc8_test_case tcase) {
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
        //uint8_t sc = (inst.opcode >>3) & 0x07;
        uint8_t regv = inst.opcode & 0x07;
        if (regv == 6) {
            // HL
            external_write(ecpu->reg.HL, tcase.from_val);
        }
    }
}

void build_sbc8(instruction_T inst) {
    CPU ecpu;
    CPU rcpu;
    alu8_test_case z_case;
    sbc_zcase(&z_case);

    alu8_test_case c_case;
    sbc_ccase(&c_case);

    alu8_test_case h_case;
    sbc_hcase(&h_case);

    alu8_test_case rlvr_case;
    sbc_rlvr_case(&rlvr_case);

    alu8_test_case *tc_v[4] = {
        &z_case, &c_case, &h_case, &rlvr_case
    };
    for (int i = 0; i < 4; i++) {
        rcpu = cpu_reg_simple_tstate;
        ecpu = cpu_reg_simple_tstate;

        assign_cpu_tstate(&rcpu, &ecpu, inst, *tc_v[i]);

        execute_test(&rcpu, inst);
        cpar_reg_check(&ecpu, &rcpu);
        cpar_reg_long(&ecpu, &rcpu);
    }

}