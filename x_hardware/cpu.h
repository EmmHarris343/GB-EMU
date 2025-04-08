#ifndef CPU_H
#define CPU_H

#include <stdint.h>

// typedef struct {
//     uint8_t A, F;   // Accumulator & Flags
//     uint8_t B, C;
//     uint8_t D, E;
//     uint8_t H, L;
//     uint16_t SP;    // 
//     uint16_t PC;    // Pointer Counter
// } CPU;

typedef struct {
    uint8_t opcode;
    uint8_t operand1;
    uint8_t operand2;
} instruction_T;




typedef struct {
    union {
        struct {
            uint8_t F;  // Flag Register (Yes the ZNHC)
            uint8_t A;  // The Accumulator
        };
        uint16_t AF;
    };
    union {
        struct {
            uint8_t C;
            uint8_t B;
        };
        uint16_t BC;
    };
    union {
        struct {
            uint8_t E;
            uint8_t D;
        };
        uint16_t DE;
    };
    union {
        struct {
            uint8_t L;
            uint8_t H;
        };
        uint16_t HL;
    };

    uint16_t SP;    // Stack pointer
    uint16_t PC;    // Program Counter

} CPU;

typedef struct {
    uint8_t halt;
    uint8_t pause;
    uint8_t stop;
} CPU_STATE;



// SOME OTHER THINGY
// void cpu_init(CPU *cpu);
// void cpu_step(CPU *cpu);

void set_flag(int cpu_flag);
void clear_flag(int cpu_flag);
uint16_t cnvrt_lil_endian(uint8_t LOW, uint8_t HIGH);
uint8_t external_read(uint16_t addr_pc);
void external_write(uint16_t addr, uint8_t write_val);


void cpu_init(uint8_t *rom_entry);

void run_cpu(int max_steps);


// The AF, Specifically the F CPU Register. - This is each individual flag
// NOTE, excluding flag N (Subtract flag) Yes/No 1/0.

// The others Z, H, C, I believe can be set to specific values!!
/// TODO: This needs to be redone. This is wrong :/
#define FLAG_Z  (1 << 7)
#define FLAG_N  (1 << 6)
#define FLAG_H  (1 << 5)
#define FLAG_C  (1 << 4)


#endif