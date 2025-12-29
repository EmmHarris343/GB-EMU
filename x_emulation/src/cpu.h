#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "logger.h"

#define NANOSECONDS_IN_MS 1000000

typedef struct {
    uint8_t opcode;
    uint8_t operand1;
    uint8_t operand2;
} instruction_T;

typedef struct {
    int step_count;
    uint8_t opcode;
    uint16_t PC;
} debug_state;

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
} CPU_Registers;

typedef struct {        
    uint8_t halt;
    uint8_t stop;
    uint8_t pause;
    uint8_t IME;    // Interrupt Master Enable, flag
    uint16_t IE;     // Interupt
    uint16_t IF;     // Interupt Flag
    uint8_t panic;  // My way to detect major failure, and abort.
} CPU_State;

typedef struct {
    CPU_Registers reg;  // CPU Registers
    CPU_State state;    // CPU States
} CPU;


typedef enum {
    INSTR_UNDF,
    INSTR_NOP,
    INSTR_ALU,
    INSTR_LD,
    INSTR_LD16,
    INSTR_LDH,
    INSTR_LDSP,
    INSTR_JUMP,
    INSTR_CALL,
    INSTR_POP,
    INSTR_PUSH,
    INSTR_RL_A,
    INSTR_RR_A,
    INSTR_RET,
    INSTR_RST,
    INSTR_MISC,
    INSTR_CB,
    INSTR_UNKNOWN,
    INSTR_TYPE_COUNT
} instr_type_T;



// External to CPU Instruction Commands
void set_flag(int cpu_flag);
void clear_flag(int cpu_flag);
uint16_t cnvrt_lil_endian(uint8_t LOW, uint8_t HIGH);
uint8_t external_read(uint16_t addr_pc);
void external_write(uint16_t addr, uint8_t write_val);



// CPU Run section:

void cpu_init();

// Main cpu LOOP:
void run_cpu(int max_steps);

// CPU Loop (By time):
void run_cpu_bytime(uint64_t max_time_ms);


// TEST Section:
void run_cpu_test(uint8_t test_op_code);





// The AF, Specifically the F CPU Register. - This is each individual flag
// NOTE, excluding flag N (Subtract flag) Yes/No 1/0.

// The others Z, H, C, I believe can be set to specific values!!
/// TODO: This needs to be redone. This is wrong :/

#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

// Old method, bad. doesn't let me know what each flag is.
// #define FLAG_Z  (1 << 7)
// #define FLAG_N  (1 << 6)
// #define FLAG_H  (1 << 5)
// #define FLAG_C  (1 << 4)


#endif