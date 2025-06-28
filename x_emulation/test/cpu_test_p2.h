
#ifndef CPU_TEST_P2_H
#define CPU_TEST_P2_H

#include <stdint.h>
#include <stdbool.h>
//#include "cpu_test_p2.c"
#include "../src/cpu.h"

//#include "../src/cpu.h"
//#include "../src/mmu.h"

#endif

void prediction_add8(uint8_t a, uint8_t b, CPU *cpu);
void prediction_adc8(uint8_t a, uint8_t b, CPU *cpu);
void prediction_sub8(uint8_t a, uint8_t b, CPU *cpu);
void prediction_sbc8(uint8_t a, uint8_t b, CPU *cpu);
void prediction_inc8(uint8_t rg, CPU *cpu);
void prediction_dec8(uint8_t rg, CPU *cpu);