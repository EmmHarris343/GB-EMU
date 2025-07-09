#include "utile.h"
#include "../../src/logger.h"


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