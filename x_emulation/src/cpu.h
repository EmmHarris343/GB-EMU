#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#define NANOSECONDS_IN_MS 1000000

// Include the gb_s struct from gb.h, this avoids importing gb.h into this header.
typedef struct gb_s GB;

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
    int IME_delay;  // Delay the interupt until next execution.
    uint8_t IE;     // Interupt
    uint8_t IF;     // Interupt Flag
    uint8_t panic;  // My way to detect major failure, and abort.
} CPU_State;

typedef struct {
    CPU_Registers reg;  // CPU Registers
    CPU_State state;    // CPU States
} CPU;

typedef struct {
    uint8_t opcode;
    uint8_t operand1;
    uint8_t operand2;
} instruction_T;





// External to CPU Instruction Commands
// void set_flag(int cpu_flag);
// void clear_flag(int cpu_flag);
uint16_t cnvrt_lil_endian(uint8_t LOW, uint8_t HIGH);


// Read/ Write functions -> MMU
uint8_t external_read(GB *gb, uint16_t addr_pc);
void external_write(GB *gb, uint16_t addr, uint8_t write_val);



/*
    CPU: Init|Run|Reset
*/


// Initialize the CPU (Sets the Register values, Flags etc)
void cpu_init(GB *gb);

// Run CPU (By Step Limit):
void run_cpu(GB *gb, int max_steps);
// Run CPU Loop (By Time Limit):
void run_cpu_bytime(GB *gb, uint64_t max_time_ms);
// Run CPU (Test Mode):
void run_cpu_test(GB *gb, uint8_t test_op_code);    // Note this using the test_cpu modules and files!

// Step the CPU....... this is meant for tracking the cycles.
// Sooooo... things might change for how this actually works.
uint32_t cpu_step(GB *gb);


// Completely Reset the CPU.
void cpu_reset(GB *gb, CPU *cpu);





// Makes setting flags easier to read in code
#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10


// For debug stuff:
typedef struct {
    int step_count;
    uint8_t opcode;
    uint16_t PC;
} debug_state;

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

#endif