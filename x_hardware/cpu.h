#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef struct {
    uint8_t A, F;   // Accumulator & Flags
    uint8_t B, C;
    uint8_t D, E;
    uint8_t H, L;
    uint16_t SP;    // 
    uint16_t PC;    // Pointer Counter
    uint8_t memory[0x10000]; // Simplified, 64KB
} CPU;


typedef struct {
    union {
        struct {
            uint8_t f;  // Flag Register (Yes the ZNHC)
            uint8_t a;  // The Accumulator
        };
        uint16_t af;
    };
    union {
        struct {
            uint8_t c;
            uint8_t b;
        };
        uint16_t bc;
    };
    union {
        struct {
            uint8_t e;
            uint8_t d;
        };
        uint16_t de;
    };
    union {
        struct {
            uint8_t l;
            uint8_t h;
        };
        uint16_t hl;
    };

    uint16_t sp;    // Stack pointer
    uint16_t pc;    // Program Counter

} CPU_Registers;




// SOME OTHER THINGY
// void cpu_init(CPU *cpu);
// void cpu_step(CPU *cpu);

void cpu_step(void);
void cpu_init(uint8_t *rom_entry);

void test_step_instruction();


// The AF, Specifically the F CPU Register. - This is each individual flag
// NOTE, excluding flag N (Subtract flag) Yes/No 1/0.

// The others Z, H, C, I believe can be set to specific values!!
/// TODO: This needs to be redone. This is wrong :/
#define FLAG_Z  (1 << 7)
#define FLAG_N  (1 << 6)
#define FLAG_H  (1 << 5)
#define FLAG_C  (1 << 4)


#endif