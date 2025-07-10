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
    if (tcase.initial_flags) {
        printf("Yes, Specialy Initial Flag.... Set flags accordingly..\n");
        set_flag_byval(rcpu, tcase.initial_flags);
    }

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
        // -> <instruciton> A, A
        set_reg_a(rcpu, tcase.dbl_A_from);
        set_reg_a(ecpu, tcase.dbl_A_expected);

        set_flag_byval(ecpu, tcase.dbl_A_expected_flags);
    }
    if (regv != 7) {
        // set expected flags
        set_flag_byval(ecpu, tcase.expected_flags);
    }

    ecpu->reg.PC ++; // Advance the PC.
}


/// ADD: Tests

void add_zcase(alu8_test_case *z_case) {
    z_case->sub_tname = "zero";
    z_case->initial_A = 0x00;
    z_case->expected_A = 0x00;
    z_case->from_val = 0x00;
    z_case->expected_flags = 0x80;   // 1 0 0 0 (Z, N, H, C)
    z_case->dbl_A_expected = 0x00;
    z_case->dbl_A_from = 0x00;
    z_case->dbl_A_expected_flags = 0x80;
}

void add_hcase(alu8_test_case *h_case) {
    h_case->sub_tname = "half-carry";
    h_case->initial_A = 0x08;
    h_case->expected_A = 0x10;
    h_case->from_val = 0x08;
    h_case->expected_flags = 0x20;   // 0 0 1 0 (Z, N, H, C)
    h_case->dbl_A_from = 0x08;
    h_case->dbl_A_expected = 0x10;
    h_case->dbl_A_expected_flags = 0x20;
}

void add_ccase(alu8_test_case *c_case) {
    c_case->sub_tname = "carry";
    c_case->initial_A = 0xF0;
    c_case->expected_A = 0xC0;
    c_case->from_val = 0xD0;
    c_case->expected_flags = 0x10;   // 0 0 0 1 (Z, N, H, C)
    c_case->dbl_A_from = 0xE0;
    c_case->dbl_A_expected = 0xC0;
    c_case->dbl_A_expected_flags = 0x10;

}

void add_rlvr_case(alu8_test_case *rv_case) {
    rv_case->sub_tname = "rollover";
    rv_case->initial_A = 0xFD;
    rv_case->expected_A = 0x34;
    rv_case->from_val = 0x37;
    rv_case->expected_flags = 0x30;   // 1 0 1 1 (Z, N, H, C)
    rv_case->dbl_A_from = 0x9A;
    rv_case->dbl_A_expected = 0x34;
    rv_case->dbl_A_expected_flags = 0x30;
}

void build_add8(instruction_T inst) {
    CPU ecpu;
    CPU rcpu;
    alu8_test_case z_case = {0};
    add_zcase(&z_case);

    alu8_test_case c_case = {0};
    add_ccase(&c_case);

    alu8_test_case h_case = {0};
    add_hcase(&h_case);

    alu8_test_case rlvr_case = {0};
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
    z_case->from_val = 0x00;
    z_case->carry_state = 0;
    z_case->expected_flags = 0x80;   // 1 0 0 0 (Z, N, H, C)
    z_case->dbl_A_from = 0x00;
    z_case->dbl_A_expected = 0x00;
    z_case->dbl_A_expected_flags = 0x80;
}

void adc_h_c0_case(alu8_test_case *h_case) {
    h_case->sub_tname = "half-carry_c0";
    h_case->initial_A = 0x0E;
    h_case->expected_A = 0x20;
    h_case->from_val = 0x12;
    h_case->carry_state = 0;
    h_case->expected_flags = 0x20;  // 0 0 1 0
    h_case->dbl_A_from = 0x0B;
    h_case->dbl_A_expected = 0x16;
    h_case->dbl_A_expected_flags = 0x20;
}

void adc_c_c0_case(alu8_test_case *c_case) {
    c_case->sub_tname = "carry_c0";
    c_case->initial_A = 0xF0;
    c_case->expected_A = 0x40;
    c_case->from_val = 0x50;
    c_case->carry_state = 0;
    c_case->expected_flags = 0x10;  // 0 0 0 1
    c_case->dbl_A_from = 0xB0;
    c_case->dbl_A_expected = 0x60;
    c_case->dbl_A_expected_flags = 0x10;
}

void adc_rlvr_c0_case(alu8_test_case *rv_case) {
    rv_case->sub_tname = "rollover_c0";
    rv_case->initial_A = 0xFE;
    rv_case->expected_A = 0x09;
    rv_case->from_val = 0x0B;
    rv_case->carry_state = 0;
    rv_case->expected_flags = 0x30;  // 0 0 1 1
    rv_case->dbl_A_from = 0x98;
    rv_case->dbl_A_expected = 0x30;
    rv_case->dbl_A_expected_flags = 0x30;
}

void adc_h_c1_case(alu8_test_case *h_case) {
    h_case->sub_tname = "half-carry_c1";
    h_case->initial_flags = 0x10;   // 0 0 0 1 (Set carry flag to true)
    h_case->initial_A = 0x0E;
    h_case->expected_A = 0x1A;
    h_case->from_val = 0x0B;
    h_case->carry_state = 1;
    h_case->expected_flags = 0x20;  // 0 0 1 0
    h_case->dbl_A_from = 0x08;
    h_case->dbl_A_expected = 0x11;
    h_case->dbl_A_expected_flags = 0x20;
}

void adc_c_c1_case(alu8_test_case *c_case) {
    c_case->sub_tname = "carry_c1";
    c_case->initial_flags = 0x10;   // 0 0 0 1 (Set carry flag to true)
    c_case->initial_A = 0xF2;
    c_case->expected_A = 0x2B;
    c_case->from_val = 0x38;    // F2 + 38 = 2A (with rollover) + 1 (carry flag) = 0x2B.
    c_case->carry_state = 1;
    c_case->expected_flags = 0x10;  // 0 0 0 1
    c_case->dbl_A_from = 0x90;
    c_case->dbl_A_expected = 0x21;
    c_case->dbl_A_expected_flags = 0x10;
}

void adc_rlvr_c1_case(alu8_test_case *rv_case) {
    rv_case->sub_tname = "rollover_c1";
    rv_case->initial_flags = 0x10;   // 0 0 0 1 (Set carry flag to true)
    rv_case->initial_A = 0xFE;
    rv_case->expected_A = 0xA1;
    rv_case->from_val = 0xA2;    // FE + 1 + 0B = 0A
    rv_case->carry_state = 1;//// Do I need this value?
    rv_case->expected_flags = 0x30;  // 0 0 1 1
    rv_case->dbl_A_from = 0x98;
    rv_case->dbl_A_expected = 0x31;
    rv_case->dbl_A_expected_flags = 0x30;
}

void build_adc8(instruction_T inst) {
    CPU ecpu;
    CPU rcpu;
    alu8_test_case z_case = {0};
    adc_zcase(&z_case);

    alu8_test_case c_case = {0};
    adc_c_c0_case(&c_case);

    alu8_test_case h_case = {0};
    adc_h_c0_case(&h_case);

    alu8_test_case rlvr_case = {0};
    adc_rlvr_c0_case(&rlvr_case);

    alu8_test_case c_c1_case = {0};
    adc_c_c1_case(&c_c1_case);

    alu8_test_case h_c1_case = {0};
    adc_h_c1_case(&h_c1_case);

    alu8_test_case rlvr_c1_case = {0};
    adc_rlvr_c1_case(&rlvr_c1_case);

    /// TODO: Clean this up because it's really messy.
    details_T deets;
    deets.inst.opcode =  inst.opcode;
    char spec_name[100];
    deets.mnemonic = "ADC";
    uint8_t rgv = inst.opcode & 0x07;
    deets.from_name = reg_names[rgv];
    deets.subname = "0";
    
    size_t sz = 50;

    alu8_test_case *tc_v[7] = {
        &z_case, &h_case, &c_case, &rlvr_case,
        &h_c1_case, &c_c1_case, &rlvr_c1_case
    };
    for (int i = 0; i < 7; i++) {
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

void sub_zcase(alu8_test_case *z_case) {
    z_case->sub_tname = "zero";
    z_case->initial_A = 0x00;
    z_case->expected_A = 0x00;
    z_case->from_val = 0x00;    
    z_case->expected_flags = 0xC0;  // 1 1 0 0
    z_case->dbl_A_from = 0xB0;
    z_case->dbl_A_expected = 0x00;
    z_case->dbl_A_expected_flags = 0xC0;

}

void sub_hcase(alu8_test_case *h_case) {
    h_case->sub_tname = "half-carry";
    h_case->initial_A = 0xB0;
    h_case->expected_A = 0xAF;
    h_case->from_val = 0x01;
    h_case->expected_flags = 0x60;  // 0 1 1 0
    h_case->dbl_A_from = 0xB0;
    h_case->dbl_A_expected = 0x00;
    h_case->dbl_A_expected_flags = 0xC0;

}

void sub_ccase(alu8_test_case *c_case) {
    c_case->sub_tname = "carry";
    c_case->initial_A = 0x40;
    c_case->expected_A = 0xF0;
    c_case->from_val = 0x50;
    c_case->expected_flags = 0x50;  // 0 1 0 1
    c_case->dbl_A_from = 0xB0;
    c_case->dbl_A_expected = 0x00;
    c_case->dbl_A_expected_flags = 0xC0;
}

void sub_rlvr_case(alu8_test_case *rv_case) {
    rv_case->sub_tname = "rollover";
    rv_case->initial_A = 0x34;
    rv_case->expected_A = 0x37;
    rv_case->from_val = 0xFD;
    rv_case->expected_flags = 0x70;  // 0 1 1 1
    rv_case->dbl_A_from = 0x34;
    rv_case->dbl_A_expected = 0x00;
    rv_case->dbl_A_expected_flags = 0xC0;
}


void build_sub8(instruction_T inst) {
    CPU ecpu;
    CPU rcpu;
    alu8_test_case z_case = {0};
    sub_zcase(&z_case);

    alu8_test_case c_case = {0};
    sub_ccase(&c_case);

    alu8_test_case h_case = {0};
    sub_hcase(&h_case);

    alu8_test_case rlvr_case = {0};
    sub_rlvr_case(&rlvr_case);

    /// TODO: Clean this up because it's really messy.
    details_T deets;
    char spec_name[100];
    uint8_t rgv = inst.opcode & 0x07;
    deets.inst.opcode =  inst.opcode;
    deets.mnemonic = "SUB";
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
    z_case->initial_A = 0x00;
    z_case->expected_A = 0x00;
    z_case->from_val = 0x00;
    z_case->carry_state = 0;
    z_case->expected_flags = 0x80;   // 1 0 0 0 (Z, N, H, C)
    z_case->dbl_A_from = 0x00;
    z_case->dbl_A_expected = 0x00;
    z_case->dbl_A_expected_flags = 0x80;
}

void sbc_h_c0_case(alu8_test_case *h_case) {
    h_case->sub_tname = "half-carry_c0";
    h_case->initial_A = 0x0E;
    h_case->expected_A = 0x12;
    h_case->from_val = 0x30;
    h_case->carry_state = 0;
    h_case->expected_flags = 0x20;  // 0 0 1 0
    h_case->dbl_A_from = 0x00;
    h_case->dbl_A_expected = 0x20;
    h_case->dbl_A_expected_flags = 0x20;
}

void sbc_c_c0_case(alu8_test_case *c_case) {
    c_case->sub_tname = "carry_c0";
    c_case->initial_A = 0xF2;
    c_case->expected_A = 0x2B;
    c_case->from_val = 0x38;
    c_case->carry_state = 0;
    c_case->expected_flags = 0x10;  // 0 0 0 1
    c_case->dbl_A_from = 0x00;
    c_case->dbl_A_expected = 0x00;
    c_case->dbl_A_expected_flags = 0x10;
}

void sbc_rlvr_c0_case(alu8_test_case *rv_case) {
    rv_case->sub_tname = "rollover_c0";
    rv_case->initial_A = 0xFE;
    rv_case->expected_A = 0x00;
    rv_case->from_val = 0x01;    // FE + 1 + 1 = 100 (or 00)
    rv_case->carry_state = 1;
    rv_case->expected_flags = 0x10;  // 1 0 1 1
    rv_case->dbl_A_from = 0x00;
    rv_case->dbl_A_expected = 0x00;
    rv_case->dbl_A_expected_flags = 0x10;
}

void sbc_h_c1_case(alu8_test_case *h_case) {
    h_case->sub_tname = "half-carry_c1";
    h_case->initial_A = 0x0E;
    h_case->expected_A = 0x12;    
    h_case->from_val = 0x30;
    h_case->carry_state = 1;
    h_case->expected_flags = 0x20;  // 0 0 1 0
    h_case->dbl_A_from = 0x00;
    h_case->dbl_A_expected = 0x20;
    h_case->dbl_A_expected_flags = 0x20;
}

void sbc_c_c1_case(alu8_test_case *c_case) {
    c_case->sub_tname = "carry_c1";
    c_case->initial_A = 0xF2;
    c_case->expected_A = 0x2B;
    c_case->from_val = 0x38;    // F2 + 38 = 2A (with rollover) + 1 (carry flag) = 0x2B.
    c_case->carry_state = 1;
    c_case->expected_flags = 0x10;  // 0 0 0 1
    c_case->dbl_A_from = 0x00;
    c_case->dbl_A_expected = 0x00;
    c_case->dbl_A_expected_flags = 0x10;
}

void sbc_rlvr_c1_case(alu8_test_case *rv_case) {
    rv_case->sub_tname = "rollover_c1";
    rv_case->initial_A = 0xFE;
    rv_case->expected_A = 0x00;
    rv_case->from_val = 0x01;    // FE + 1 + 1 = 100 (or 00)
    rv_case->carry_state = 1;
    rv_case->expected_flags = 0x10;  // 1 0 1 1
    rv_case->dbl_A_from = 0x00;
    rv_case->dbl_A_expected = 0x00;
    rv_case->dbl_A_expected_flags = 0x10;
}



void build_sbc8(instruction_T inst) {
    CPU ecpu;
    CPU rcpu;
    alu8_test_case z_case = {0};
    sbc_zcase(&z_case);

    alu8_test_case c_case = {0};
    sbc_c_c0_case(&c_case);

    alu8_test_case h_case = {0};
    sbc_h_c0_case(&h_case);

    alu8_test_case rlvr_case = {0};
    sbc_rlvr_c0_case(&rlvr_case);

    alu8_test_case c_c1_case = {0};
    sbc_c_c1_case(&c_c1_case);

    alu8_test_case h_c1_case = {0};
    sbc_h_c1_case(&h_c1_case);

    alu8_test_case rlvr_c1_case = {0};
    sbc_rlvr_c1_case(&rlvr_c1_case);

    alu8_test_case *tc_v[7] = {
        &z_case, &h_case, &c_case, &rlvr_case,
        &h_c1_case, &c_c1_case, &rlvr_c1_case
    };
    for (int i = 0; i < 7; i++) {
        rcpu = cpu_reg_simple_tstate;
        ecpu = cpu_reg_simple_tstate;

        assign_cpu_tstate(&rcpu, &ecpu, inst, *tc_v[i]);

        execute_test(&rcpu, inst);
        cpar_reg_check(&ecpu, &rcpu);
        cpar_reg_long(&ecpu, &rcpu);
    }

}