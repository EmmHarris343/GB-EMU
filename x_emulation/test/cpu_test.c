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


#define tFLAG_Z 0x80  // 1000 0000
#define tFLAG_N 0x40  // 0100 0000
#define tFLAG_H 0x20  // 0010 0000
#define tFLAG_C 0x10  // 0001 0000


#define FZ 0x80  // 1000 0000
#define FN 0x40  // 0100 0000
#define FH 0x20  // 0010 0000
#define FC 0x10  // 0001 0000


// For some quick and easy opcodes?
#define IS_ADD_R(op) ((op & 0xF8) == 0x80)
#define IS_ADC_R(op) ((op & 0xF8) == 0x88)
#define IS_SUB_R(op) ((op & 0xF8) == 0x90)
#define IS_SBC_R(op) ((op & 0xF8) == 0x98)
#define IS_AND_R(op) ((op & 0xF8) == 0xA0)
#define IS_XOR_R(op) ((op & 0xF8) == 0xA8)
#define IS_OR_R(op)  ((op & 0xF8) == 0xB0)
#define IS_CP_R(op)  ((op & 0xF8) == 0xB8)

// These ones might not work right...... 
#define IS_AND_n8(op)  ((op & 0xC7) == 0xC6)
#define IS_XOR_n8(op)  ((op & 0xC7) == 0xB8)
#define IS_OR_n8(op)  ((op & 0xC7) == 0xB8)
#define IS_CP_n8(op)  ((op & 0xC7) == 0xB8)

#define GET_REG(op)  ((op) & 0x07)




const char* reg_names[8] = { "B", "C", "D", "E", "H", "L", "[HL]", "A" };


// Condensed flag instructions:
void s_f(CPU* c, uint8_t fl) {
    c->reg.F |= fl;
}
void c_f(CPU* c, uint8_t fl) {
    c->reg.F ^= fl;
}


// Condensed checks
uint8_t ck_cf(CPU* c) {
    return (c->reg.F & FLAG_C) ? 1 : 0;
}

uint8_t ck_sbc(CPU* c, uint8_t r, uint8_t cy) {
    return ((c->reg.A & 0x0F) - (r & 0x0F) - cy);
}



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
    (working->reg.HL == expected->reg.HL) ? printf("[PASS]=HL :: ") : printf("[FAIL] HL\n");
    (working->reg.PC == expected->reg.PC) ? printf("[PASS]=PC :: ") : printf("[FAIL] PC\n");
    (working->reg.SP == expected->reg.SP) ? printf("[PASS]=SP\n") : printf("[FAIL] SP\n");
    //(working->reg. == expected->reg.AF) ? printf("[PASS] AF\n") : printf("[FAIL] AF\n");
}

void reg_whfail(CPU *working, CPU *expected) {
    // Add each failure point. 
    if (working->reg.A != expected->reg.A) {
        // Failed at Register A. ---- Add to "failure mismatch list".
    }

    
    printf("Failure at Register point(s): \n" /* the thingy */);
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

    size_t max_length = 32;
    snprintf(spec_message, max_length, "LD %s, %s", reg_names[dest_code], reg_names[src_code]);
    
    uint8_t *reg_lookup_expected[8] = {
        &expected_cpu->reg.B, &expected_cpu->reg.C, &expected_cpu->reg.D, &expected_cpu->reg.E, 
        &expected_cpu->reg.H, &expected_cpu->reg.L, NULL, &expected_cpu->reg.A
    };
    
    if (dest_code == 6) {   // LD [HL], x         
        external_write(initial_cpu->reg.HL, *reg_lookup_expected[src_code]);
    }
    if (src_code == 6) {    // LD x, [HL]        
        external_write(initial_cpu->reg.HL, p_hl_val);
        *reg_lookup_expected[dest_code] = p_hl_val; // 
    }
    if ((src_code != 6) & (dest_code != 6)) {
        *reg_lookup_expected[dest_code] = *reg_lookup_expected[src_code];
    }

    expected_cpu->reg.PC ++;    // 1 Byte
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


void get_expected_8bit_arithmetic(instruction_T instruction, CPU* initial_cpu, CPU* exp_cpu, char* spec_message, uint8_t p_hl_val) {
    uint8_t opcode = instruction.opcode;
    uint8_t op1 = instruction.operand1;
    uint8_t src_code = opcode & 0x07;
    uint8_t src_spcl = (opcode >> 3) & 0x07;
    size_t sz = 32;
    char* op_mnemonic;
    
    uint8_t *rg_lkp[8] = {
        &exp_cpu->reg.B, &exp_cpu->reg.C, &exp_cpu->reg.D, &exp_cpu->reg.E,
        &exp_cpu->reg.H, &exp_cpu->reg.L, NULL, &exp_cpu->reg.A
    };

    uint8_t reg_val = 0x0;
    if ((src_code == 6) & (opcode < 0xC0))          // (INSTR) A, [HL]
        { reg_val = external_read(initial_cpu->reg.HL); }
    if ((src_code != 6) & (opcode < 0xC0))          // (INSTR) A, r8
        { reg_val = *rg_lkp[src_code]; }
    if ((src_code == 6) & (opcode >= 0xC0))         // (INSTR) A, n8    (This sets the value to a n8 operand 1)
        { if (op1) reg_val = op1; }

    // INC / DEC does not have an n8 value it uses. It has 16 byte, but that will be under 16 byte arithmatic
    if ((opcode & 0xC7) == 0x04){
        op_mnemonic = "INC";    // INC A, r8
        if (src_spcl == 6)
            { reg_val = external_read(initial_cpu->reg.HL); }
        else
            { reg_val = *rg_lkp[src_spcl];}

        uint8_t hl_val = reg_val;
        printf("INC ----- OPCODE | SRC_SPCL_OPCODE -> 0x%02X | 0x%02X\n\n", opcode, src_spcl);

        ((hl_val & 0x0F) == 0x0F)   // H Flag before INC
            ? s_f(exp_cpu, FH) : c_f(exp_cpu, FH);
        hl_val ++;
        (hl_val == 0x0)             // Z Flag after INC
            ? s_f(exp_cpu, FZ) : c_f(exp_cpu, FZ);
        c_f(exp_cpu, FN);     // N (sub) Always cleared

        if (src_spcl == 6) {
            // Do not ACTUALLY write to ram, this is just a test!
        }
        else {
            *rg_lkp[src_spcl] = hl_val;
        }
    }
    if ((opcode & 0xC7) == 0x05) {
        op_mnemonic = "DEC";    // DEC A, r8
        printf("Execute DEC\n\n\n");

        if (src_spcl == 6)
            { reg_val = external_read(initial_cpu->reg.HL); }
        else
            { reg_val = *rg_lkp[src_spcl];}

        // uint8_t pre_dec = (*rg_lkp[src_code]);
        // uint8_t dec_final = (*rg_lkp[src_code] --);

        printf("==--TEST DEBUG--== WHAT IS VALUE in [HL] :: 0x%02X\n", external_read(initial_cpu->reg.HL));

        printf("DEC ----- OPCODE | SRC_SPCL_OPCODE -> 0x%02X | 0x%02X\n\n", opcode, src_spcl);
        uint8_t hl_val = reg_val;
        
        ((hl_val & 0x0F) == 0x00)   // H Flag before DEC
            ? s_f(exp_cpu, FH) : c_f(exp_cpu, FH);  // H Flag
        hl_val --;
        ((hl_val & 0xFF) == 0x00)   // Z Flag after DEC
            ? s_f(exp_cpu, FZ) : c_f(exp_cpu, FZ);  // Z Flag
        s_f(exp_cpu, FN);   // N (sub) Always Set
        
        if (src_spcl == 6) {
            // Do not ACTUALLY write to ram, this is just a test!
        }
        else {
            *rg_lkp[src_spcl] = hl_val;
        }
    }

    // Do instead?
    //if (((opcode & 0xFE) == 0x80) || (opcode = 0xC6))
    //if ((IS_ADD_R(opcode)) || (opcode = 0xC6))
    if ((opcode >= 0x80 && opcode <= 0x87) || (opcode == 0xC6)) {
        op_mnemonic = "ADD";    // ADD A, X  | ADD A, n8
        
        uint16_t add_result = (exp_cpu->reg.A + reg_val);
        uint8_t final_8bit = (uint8_t)add_result;
        uint8_t a_reg = exp_cpu->reg.A;

        c_f(exp_cpu, FN);   // N (sub) Always cleared
        (add_result & 0xFF)  == 0 
            ? s_f(exp_cpu, FZ) : c_f(exp_cpu, FZ);  // Z Flag
        (a_reg & 0x0F) + (reg_val & 0x0F) > 0x0F 
            ? s_f(exp_cpu, FH) : c_f(exp_cpu, FH);  // H Flag
        (add_result > 0xFF)
            ? s_f(exp_cpu, FC) : c_f(exp_cpu, FC);  // C Flag
       
        exp_cpu->reg.A = final_8bit;
        if (opcode == 0xC6) { exp_cpu->reg.PC ++; }     // (Advance it once more, for the n8 instruciton.)
    }
    if ((opcode >= 0x88 && opcode <= 0x8F) || (opcode == 0xCE)){
        op_mnemonic = "ADC";   // ADC A, X || ADC A, n8

        uint8_t carry_in = (exp_cpu->reg.F & FC) ? 1 : 0;
        uint16_t add_16bit = (exp_cpu->reg.A + reg_val + carry_in);
        uint8_t final_8bit = (uint8_t)add_16bit;
        uint8_t a_reg = exp_cpu->reg.A;

        c_f(exp_cpu, FN);  // N (sub) Always cleared
        (add_16bit & 0xFF) == 0 
            ? s_f(exp_cpu, FZ) : c_f(exp_cpu, FZ);  // Z Flag
        ((a_reg & 0x0F) + (reg_val & 0x0F) + carry_in) > 0x0F 
            ? s_f(exp_cpu, FH) : c_f(exp_cpu, FH);  // H Flag
        (add_16bit > 0xFF)
            ? s_f(exp_cpu, FC) : c_f(exp_cpu, FC);  // C Flag

        exp_cpu->reg.A = final_8bit;
        if (opcode == 0xCE) { exp_cpu->reg.PC ++; }     // (Advance it once more, for the n8 instruciton.)
    }
    if ((opcode >= 0x90 && opcode <= 0x97) || (opcode == 0xD6)) {
        op_mnemonic = "SUB";   // SUB A, X || SUB A, n8

        uint16_t sub_16bit = (exp_cpu->reg.A - reg_val);
        uint8_t final_8bit = (uint8_t)sub_16bit;
        uint8_t a_reg = exp_cpu->reg.A;

        s_f(exp_cpu, FN);   // N (sub) Always set
        (sub_16bit & 0xFF) == 0
            ? s_f(exp_cpu, FZ) : c_f(exp_cpu, FZ);  // Z Flag
        ((a_reg & 0x0F) < (reg_val & 0x0F))
            ? s_f(exp_cpu, FH) : c_f(exp_cpu, FH);  // H Flag
        (a_reg < reg_val)
            ? s_f(exp_cpu, FC) : c_f(exp_cpu, FC);  // C Flag

        exp_cpu->reg.A = final_8bit;
        if (opcode == 0xD6) { exp_cpu->reg.PC ++; }     // (Advance it once more, for the n8 instruciton.)
    }
    if ((opcode >= 0x98 && opcode <= 0x9F) || (opcode == 0xDE)){
        op_mnemonic = "SBC";   // SBC A, X || SBC A, X
        
        uint8_t carry_in  = (exp_cpu->reg.F & FC) ? 1 : 0;
        uint16_t sub_16bit = (exp_cpu->reg.A - reg_val - carry_in);
        uint8_t final_8bit = (uint8_t)sub_16bit;
        uint8_t a_reg = exp_cpu->reg.A;

        s_f(exp_cpu, FN);   // N (sub) Always set
        (sub_16bit & 0xFF) == 0
            ? s_f(exp_cpu, FZ) : c_f(exp_cpu, FZ);  // Z Flag
        ((a_reg & 0x0F) - (reg_val & 0x0F) - carry_in) < 0
            ? s_f(exp_cpu, FH) : c_f(exp_cpu, FH);  // H Flag
        (a_reg < (reg_val + carry_in))
            ? s_f(exp_cpu, FC) : c_f(exp_cpu, FC);  // C Flag

        exp_cpu->reg.A = final_8bit;
        if (opcode == 0xDE) { exp_cpu->reg.PC ++; }     // (Advance it once more, for the n8 instruciton.)
    }
    exp_cpu->reg.PC ++; // Bytes = 1
    if (opcode >= 0xC6 & opcode <= 0xFF) 
        { snprintf(spec_message, sz, "%s A, n8 (%2X)", op_mnemonic, op1); }
    if ((opcode >= 0x80) && (opcode <= 0xBF)) 
        { snprintf(spec_message, sz, "%s A, %s", op_mnemonic, reg_names[src_spcl]); }
    if ((opcode & 0xC7) == 04 || (opcode & 0xC7) == 05) {
        snprintf(spec_message, sz, "%s %s", op_mnemonic, reg_names[src_spcl]);
    }
}

void get_expected_logic_operations(instruction_T instruction, CPU* initial_cpu, CPU* exp_cpu, char* spec_message, uint8_t p_hl_val) {
    uint8_t opcode = instruction.opcode;
    uint8_t op1 = instruction.operand1;
    //uint8_t dest_code = (opcode >> 3) & 0x07;
    uint8_t src_code = opcode & 0x07;                   // Source Bits 2-0
    size_t sz = 32;
    char* op_mnemonic;

    uint8_t *rg_lkp[8] = {
        &exp_cpu->reg.B, &exp_cpu->reg.C, &exp_cpu->reg.D, &exp_cpu->reg.E, 
        &exp_cpu->reg.H, &exp_cpu->reg.L, NULL, &exp_cpu->reg.A
    };

    uint8_t reg_val = 0x00;
    if ((src_code == 6) && (opcode < 0xC0))     // (INSTR) A, [HL]
        { reg_val = external_read(initial_cpu->reg.HL); }
    if ((src_code != 6) && (opcode < 0xC0))     // (INSTR) A, r8
        { reg_val = *rg_lkp[src_code]; }
    if ((src_code == 6) && (opcode >= 0xC0))    // (INSTR) A, n8
        { if (op1) reg_val = op1; }

    if ((opcode >= 0xA0 && opcode <= 0xA7) || (opcode == 0xE6)) {
        op_mnemonic = "AND";   // AND A, X || AND A, n8.

        uint8_t AND_result = (initial_cpu->reg.A & reg_val);
        exp_cpu->reg.A = AND_result;

        (AND_result == 0) ? s_f(exp_cpu, FZ) : c_f(exp_cpu, FZ);
        c_f(exp_cpu, FN);  // ALways cleared
        s_f(exp_cpu, FH);  // Always set
        c_f(exp_cpu, FC);  // Always cleared
        if (opcode == 0xE6) { exp_cpu->reg.PC ++; }     // (Advance it once more, for the n8 instruciton.)
    }
    if ((opcode >= 0xA8 && opcode <= 0xAF) || (opcode == 0xEE)) {
        op_mnemonic = "XOR";   // XOR A, X || XOR A, n8
        
        uint8_t XOR_result = (exp_cpu->reg.A ^ reg_val);
        exp_cpu->reg.A = XOR_result;
        (XOR_result == 0) ? s_f(exp_cpu, FZ) : c_f(exp_cpu, FZ);
        c_f(exp_cpu, FN);  // ALways cleared
        c_f(exp_cpu, FH);  // Always cleared
        c_f(exp_cpu, FC);  // Always cleared
        if (opcode == 0xEE) { exp_cpu->reg.PC ++; }     // (Advance it once more, for the n8 instruciton.)
    }
    if ((opcode >= 0xB0 && opcode <= 0xB7) || (opcode == 0xF6)) {
        op_mnemonic = "OR";     // OR A, X || OR A, n8
        
        uint8_t OR_result = (initial_cpu->reg.A | reg_val);
        exp_cpu->reg.A = OR_result;
        (OR_result == 0) ? s_f(exp_cpu, FZ) : c_f(exp_cpu, FZ);
        c_f(exp_cpu, FN);  // ALways cleared
        c_f(exp_cpu, FH);  // Always cleared
        c_f(exp_cpu, FC);  // Always cleared
        if (opcode == 0xF6) { exp_cpu->reg.PC ++; }     // (Advance it once more, for the n8 instruciton.)
    }
    if ((opcode >= 0xB8 && opcode <= 0xBF) || (opcode == 0xFE)) {
        op_mnemonic = "CP";     // CP A, X || CP A, n8

        uint8_t hold_result = (exp_cpu->reg.A - reg_val);
        uint8_t a_reg = exp_cpu->reg.A;

        (hold_result == 0) ? s_f(exp_cpu, FZ) : c_f(exp_cpu, FZ);
        s_f(exp_cpu, FN);  // ALways Set

        (a_reg & 0x0F) < (reg_val & 0x0F)
            ? s_f(exp_cpu, FH) : c_f(exp_cpu, FH); // H-Carry
        (a_reg < reg_val)  
            ? s_f(exp_cpu, FC) : c_f(exp_cpu, FC); // Carry
        if (opcode == 0xFE) { exp_cpu->reg.PC ++; }     // (Advance it once more, for the n8 instruciton.)
    }
    exp_cpu->reg.PC ++; // Bytes = 1
    if (opcode >= 0xC6 & opcode <= 0xFF) {
        snprintf(spec_message, sz, "%s A, n8 (%2X)", op_mnemonic, op1);
    }
    else {
        snprintf(spec_message, sz, "%s A, %s", op_mnemonic, reg_names[src_code]);
    }
    
    }

void get_expected_16bit_arithmetic(instruction_T instruction, CPU* initial_cpu, CPU* exp_cpu, char* spec_message, uint8_t p_hl_val) {   
    // These are tests outputs for INC BC, DEC BC. Plus for simplicity sake, ADD r16, r16

    uint8_t opcode = instruction.opcode;
    uint8_t src_opcode = (opcode >> 4) & 0x03;  // All the DEC, INC, and ADD r16. Encodes the Register in a different bit area. So shift it over more!
    size_t sz = 32;
    char* op_mnemonic;
    
    uint16_t *rg_lkp[4] = {
        &exp_cpu->reg.BC, &exp_cpu->reg.DE, &exp_cpu->reg.HL, &exp_cpu->reg.SP
    };
    const char* reg_names_arith16bit[4] = { "BC", "DE", "HL", "SP" };

    if ((opcode & 0xCF) == 0x03) {      // Will awlays match 0x03, for 0x03 - 0x33.
        // The INC BC, DE, HL, SP
        op_mnemonic = "INC";
        uint16_t inc_value = *rg_lkp[src_opcode];
        inc_value ++;
        *rg_lkp[src_opcode] = inc_value;
    }
    if ((opcode & 0xCF) == 0x0B) {      // Will always match to 0x0B, for 0x0B to 0x3B.
        // DEC BC, DE, HL, SP
        op_mnemonic = "DEC";
        uint16_t dec_value = *rg_lkp[src_opcode];
        dec_value --;
        *rg_lkp[src_opcode] = dec_value;
    }
    if ((opcode & 0xCF) == 0x09) {      // Will always match to 0x0B, for 0x0B to 0x3B.
        // ADD HL, r16
        // This one is more complicated, because it wants to track Flags.
        op_mnemonic = "ADD";
        uint16_t pre_hl = initial_cpu->reg.HL;
        uint16_t pre_r16 = *rg_lkp[src_opcode];
        uint32_t add_result = (pre_hl + pre_r16);
        uint16_t final_result = (uint16_t)add_result;   // Convert back to 16bit, will truncate anything higher than 0xFF.
        
        // Z FLAG UNCHANGED, Do not set, or clear.
        c_f(exp_cpu, FN);  // ALways cleared
        ((pre_hl & 0x0FFF) + (pre_r16 & 0x0FFF) > 0x0FFF) // The H Flag.
            ? s_f(exp_cpu, FH) : c_f(exp_cpu, FH);
        (add_result > 0xFFFF)
            ? s_f(exp_cpu, FC) : c_f(exp_cpu, FC); // C Flag
        
        
        exp_cpu->reg.HL = final_result;
    }

    if ((opcode & 0xCF) == 0x09) 
        { snprintf(spec_message, sz, "%s HL, %s", op_mnemonic, reg_names_arith16bit[src_opcode]); }
    else
        { snprintf(spec_message, sz, "%s %s", op_mnemonic, reg_names_arith16bit[src_opcode]); }

    exp_cpu->reg.PC ++;
}

void unt_tcase_builder(instruction_T local_instrc) {
    printf("---------\nCreating Unit Test. FOR --> OPCODE=0x%02X\n", local_instrc.opcode);
    CPU initial_cpu_state = cpu_reg_simple_tstate;
    CPU expected_cpu_state = cpu_reg_simple_tstate;

    uint8_t p_hl_val = 0xD0;
    external_write(initial_cpu_state.reg.HL, p_hl_val);
    char instruc_name_val[32];

    // This is the only part that might be hard.. to be Dynamic to point to the right Function.
    // But..... I likely need to build, a specific function for each Instruction group.
    if ((local_instrc.opcode >= 0x80) && (local_instrc.opcode <= 0x9F)) {
        get_expected_8bit_arithmetic(
        local_instrc,
        &initial_cpu_state,
        &expected_cpu_state,
        instruc_name_val,
        p_hl_val);
    }
    if (local_instrc.opcode == 0xC6 || local_instrc.opcode == 0xCE || local_instrc.opcode == 0xD6 || local_instrc.opcode == 0xDE) {
        get_expected_8bit_arithmetic(
        local_instrc,
        &initial_cpu_state,
        &expected_cpu_state,
        instruc_name_val,
        p_hl_val);
    }
    if ((local_instrc.opcode & 0xC7) == 0x04 || (local_instrc.opcode & 0xC7) == 0x05) {
        get_expected_8bit_arithmetic(
        local_instrc,
        &initial_cpu_state,
        &expected_cpu_state,
        instruc_name_val,
        p_hl_val);
    }
    if ((local_instrc.opcode >= 0xA0) && (local_instrc.opcode <= 0xBF)) {
        get_expected_logic_operations(
        local_instrc,
        &initial_cpu_state,
        &expected_cpu_state,
        instruc_name_val,
        p_hl_val);
    }
    if (local_instrc.opcode == 0xE6 || local_instrc.opcode == 0xEE || local_instrc.opcode == 0xF6 || local_instrc.opcode == 0xFE) {
        get_expected_logic_operations(
        local_instrc,
        &initial_cpu_state,
        &expected_cpu_state,
        instruc_name_val,
        p_hl_val);
    }

    if ((local_instrc.opcode & 0xCF) == 0x03  || (local_instrc.opcode & 0xCF) == 0x0B || (local_instrc.opcode & 0xCF) == 0x09) {
        get_expected_16bit_arithmetic(
        local_instrc,
        &initial_cpu_state,
        &expected_cpu_state,
        instruc_name_val,
        p_hl_val);  
        /// TODO: Remove p_hl_val. As no 16bit arithmatic uses [HL] pointed values.
    }

    Test_Case_t build_test;
    build_test.name = instruc_name_val;
    build_test.opcode = local_instrc.opcode;
    if (local_instrc.operand1 > 0) {
        build_test.operand1 = local_instrc.operand1;
    }
    if (local_instrc.operand2 > 0) {
        build_test.operand2 = local_instrc.operand2;
    }    
    build_test.initial_cpu = initial_cpu_state;
    build_test.expected_cpu = expected_cpu_state;

    CPU working_cpu = build_test.initial_cpu;
    // Write the default value of [HL] again to ram.
    // Because my "test" of what is in ram, actually writes it to ram.. Sooo.
    external_write(working_cpu.reg.HL, p_hl_val);
    
    int exe_return = execute_test(&working_cpu, local_instrc);
    if (exe_return  != 0) { 
        printf("ERROR [cpu_test], instruction failed to execute"); 
    }
    else { 
        printf("Execution Complete.\n"); 
    }

    printf("View regs: initial cpu, expected cpu\n");
    view_regs(&build_test.initial_cpu, &build_test.expected_cpu);
    printf("View regs: working cpu, expected cpu\n");
    view_regs(&working_cpu, &build_test.expected_cpu);

    reg_compare2(&working_cpu, &build_test.expected_cpu);

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
}



void entry_test_case(){
    instruction_T instrc;
    instrc.opcode = 0x5C;           // Placeholder, so it's initialized. (This probably should be NOP)
    instrc.operand1 = 0x00;
    instrc.operand2 = 0x00;

    for (int i = 0x80; i <= 0xBF; i++) {
        instrc.opcode = i;
        //unt_ld_tcase(instrc);
        unt_tcase_builder(instrc);

        if (i == 0x7F) { break; }   // Hit the last line. STOP and close
    }


    // // List of opcodes. To run (In sequence).
    // uint8_t test_opcodes[] = {
    //     0x80, 0x81, 0x82, // ADD A, B/C/D
    //     0x88, 0x89,           // ADC A, B/C
    //     0xC6,             // ADD A, n8
    //     0xCE,             // ADC A, n8
    //     // ...and so on
    // };

    // maybe do, to split into groups
    // Or ... to arrange "steps" for a instruction
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

    instrc.operand1 = 0x84;
    instrc.operand2 = 0x00;
    for (int i = 0x0; i <= 0x03; i++) {
        instrc.opcode = arith8bit[i];    // ADD and SUB
        unt_tcase_builder(instrc);
    }    
    for (int i = 0x0; i <= 0x03; i++) {
        instrc.opcode = logic_ops[i];    // AQND, OR, XOR...
        unt_tcase_builder(instrc);
    }
    for (int i = 0x0; i <= 0x0F; i++) {
        instrc.opcode = arith8bit_2[i]; // INC / DEC r8
        unt_tcase_builder(instrc);
    }
    for (int i = 0x0; i <= 0x0B; i++) {
        instrc.opcode = arith16bit[i];    // INC r16 | DEC r16 | ADD r16
        unt_tcase_builder(instrc);
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