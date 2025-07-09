#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "../../src/cpu.h"


#define tFLAG_Z 0x80  // 1000 0000
#define tFLAG_N 0x40  // 0100 0000
#define tFLAG_H 0x20  // 0010 0000
#define tFLAG_C 0x10  // 0001 0000


#define FZ 0x80  // 1000 0000
#define FN 0x40  // 0100 0000
#define FH 0x20  // 0010 0000
#define FC 0x10  // 0001 0000


void check_flags(CPU* inital_cpu, CPU* expected_cpu);
void cpar_reg_long(CPU* b4, CPU* l8);
void cpar_reg_check(CPU *b4, CPU *l8);
bool check_reg(CPU *rcpu, CPU *ecpu);
void check_toLog(CPU *rcpu, CPU *ecpu, instruction_T inst, char *name);
void set_flag_byval(CPU *cpu, uint8_t f);
void set_reg_val(CPU *cpu, uint8_t op, uint8_t rval);
void set_reg_a(CPU *cpu, uint8_t rval);
