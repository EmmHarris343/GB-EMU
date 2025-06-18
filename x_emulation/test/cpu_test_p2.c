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


/*
Let your assertions only focus on verifying effects and flags, not re-running or re-implementing instruction logic. 
Let the emulator do the execution your job is to feed it inputs, and predict the outputs independently.
*/

#define FZ 0x80  // 1000 0000
#define FN 0x40  // 0100 0000
#define FH 0x20  // 0010 0000
#define FC 0x10  // 0001 0000


// Condensed flag instructions:
void s_f(CPU* c, uint8_t fl) {
    c->reg.F |= fl;
}
void c_f(CPU* c, uint8_t fl) {
    c->reg.F ^= fl;
}


void prediction_add8(uint8_t a, uint8_t b, CPU *cpu) {
    uint16_t add_16bit = a + b;
    uint8_t low = (a & 0xF) + (b & 0xF);

    // J'aime pas là
    // cpu->reg.F = 0;
    // if ((add_16bit & 0xFF) == 0) cpu->reg.F |= FZ;
    // if (low > 0xF) cpu->reg.F |= FH;
    // if (add_16bit > 0xFF) cpu->reg.F |= FC;

    c_f(cpu, FN);  // N (sub) Always cleared
    (add_16bit & 0xFF)  == 0
        ? s_f(cpu, FZ) : c_f(cpu, FZ);  // Z Flag
    (a & 0x0F) + (b & 0x0F) > 0x0F
        ? s_f(cpu, FH) : c_f(cpu, FH);  // H Flag
    (add_16bit > 0xFF)
        ? s_f(cpu, FC) : c_f(cpu, FC);  // C Flag
}

void prediction_adc8(uint8_t a, uint8_t b, CPU *cpu) {
    uint8_t carry_in = (cpu->reg.F & FC) ? 1 : 0;
    uint16_t add_16bit = (a + b + carry_in);

    c_f(cpu, FN);  // N (sub) Always cleared
    (add_16bit & 0xFF) == 0 
        ? s_f(cpu, FZ) : c_f(cpu, FZ);  // Z Flag
    ((a & 0x0F) + (b & 0x0F) + carry_in) > 0x0F 
        ? s_f(cpu, FH) : c_f(cpu, FH);  // H Flag
    (add_16bit > 0xFF)
        ? s_f(cpu, FC) : c_f(cpu, FC);  // C Flag
}

void prediction_sub(uint8_t a, uint8_t b, CPU *cpu) {
    uint16_t sub_16bit = (a - b);

    s_f(cpu, FN);   // N (sub) Always set
    (sub_16bit & 0xFF) == 0
        ? s_f(cpu, FZ) : c_f(cpu, FZ);  // Z Flag
    ((a & 0x0F) < (b & 0x0F))
        ? s_f(cpu, FH) : c_f(cpu, FH);  // H Flag
    (a < b)
        ? s_f(cpu, FC) : c_f(cpu, FC);  // C Flag
}

void prediction_sbc(uint8_t a, uint8_t b, CPU *cpu) {
    uint8_t carry_in  = (cpu->reg.F & FC) ? 1 : 0;
    uint16_t sub_16bit = (a - b - carry_in);

    s_f(cpu, FN);   // N (sub) Always set
    (sub_16bit & 0xFF) == 0
        ? s_f(cpu, FZ) : c_f(cpu, FZ);  // Z Flag
    ((a & 0x0F) - (b & 0x0F) - carry_in) < 0
        ? s_f(cpu, FH) : c_f(cpu, FH);  // H Flag
    (a < (b + carry_in))
        ? s_f(cpu, FC) : c_f(cpu, FC);  // C Flag
}

void prediction_inc8(uint8_t val, CPU *cpu) {
    ((val & 0x0F) == 0x0F)   // H Flag before INC
    ? s_f(cpu, FH) : c_f(cpu, FH);
    val ++;
    (val == 0x0)             // Z Flag after INC
        ? s_f(cpu, FZ) : c_f(cpu, FZ);
    c_f(cpu, FN);     // N (sub) Always cleared
}

void prediction_dec8(uint8_t val, CPU *cpu) {
        ((val & 0x0F) == 0x00)
            ? s_f(cpu, FH) : c_f(cpu, FH);  // H Flag
        val --;
        ((val & 0xFF) == 0x00)
            ? s_f(cpu, FZ) : c_f(cpu, FZ);  // Z Flag
        s_f(cpu, FN);   // N (sub) Always Set
}

void prediction_and(uint8_t val, CPU *cpu) {

}

void prediction_xor(uint8_t val, CPU *cpu) {

}

void prediction_or(uint8_t val, CPU *cpu) {

}

void prediction_cp(uint8_t val, CPU *cpu) {

}

// 16bit.
void prediction_inc16(uint8_t val, CPU *cpu) {
    // No flags set.
    // Probably delete this

}

void prediction_dec16(uint8_t val, CPU *cpu) {
    // No flags set.
    // Probably detete this

}

void prediction_add16(uint16_t a, uint16_t b, CPU *cpu) {
    uint32_t add_result = (a + b);
    
    // Z FLAG UNCHANGED, Do not set, or clear.
    c_f(cpu, FN);  // ALways cleared
    ((a & 0x0FFF) + (b & 0x0FFF) > 0x0FFF) // The H Flag.
        ? s_f(cpu, FH) : c_f(cpu, FH);
    (add_result > 0xFFFF)
        ? s_f(cpu, FC) : c_f(cpu, FC); // C Flag
}



const CPU spec_2 = {
    .reg.AF = 0x0CB0,       // B0 = 1011 (IE Z set, N not set, H set, C set)
    .reg.BC = 0x0C0A,
    .reg.DE = 0x0C1B,
    .reg.HL = 0x0C2D,       // This points to WRAM Work-RAM. (FOR Test Writes/ Reads.)
    .reg.SP = 0xFFFE,
    .reg.PC = 0x0979,
    .state.IME = 0,         // Interupt
    .state.halt = 0,
    .state.pause = 0,
    .state.stop = 0,
    .state.panic = 0        // Mine.... if this is set. It means instruction likely wasn't made yet. IE Hard abort
};


typedef struct {

} spec_cpu_reg;

void calculate_output() {



}


void experiment_add_deux(instruction_T instruction, CPU* e_cpu) {
    // Simple?..

    CPU initial_cpu_state = spec_2;
    CPU expected_cpu_state = spec_2;


    CPU et_cpu = spec_2;

    // 0x0F = Decimal 15.
    // So, 15 + 15. Or, 0x0F + 0x0F.
    // = 30 (decimal)
    // = 0x1E (hex)

    // This would, overflow 0x0F. The half carry flag. 
    // So this would set the flags:
    // Z=0, N=0, H=1, C=0

    et_cpu.reg.BC = 0x0F0F; // Pre-state?
    et_cpu.reg.BC = 0x1E0F; // Post-state?

    et_cpu.reg.AF = 0x0C20; // 0x20 = 0010 (H flag set but nothing else)




    // e_cpu = experiment_cpu_state;

    // Opcode for ADD?

    // pre val
    // post val



}