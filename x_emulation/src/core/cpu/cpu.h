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
    uint8_t panic;  // My way to detect major failure, and abort.
} CPU_State;

typedef struct {
    CPU_Registers reg;  // CPU Registers
    CPU_State state;    // CPU States
    uint32_t cycle;     // The cycle for this cpu step. (Should reset each step)
} CPU;

typedef struct {
    uint8_t opcode;
    uint8_t operand1;
    uint8_t operand2;
} instruction_T;




#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CPU_TRACE_CAPACITY 100

typedef struct {
    uint64_t step;
    uint16_t pc;
    uint8_t opcode;
    uint32_t cycles;
    uint16_t sp;

    uint8_t a;
    uint8_t f;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;

    uint8_t ime;
    //uint8_t ie;
    uint8_t iflag;
} CPUTraceEntry;

typedef struct {
    CPUTraceEntry entries[CPU_TRACE_CAPACITY];
    uint32_t head;
    uint32_t count;
} CPUTraceBuffer;




uint16_t cnvrt_lil_endian(uint8_t LOW, uint8_t HIGH);


// Read/ Write functions -> MMU
uint8_t external_read(GB *gb, uint16_t addr_pc);
void external_write(GB *gb, uint16_t addr, uint8_t write_val);



/*
    CPU: Init|Run|Reset
*/


// Initialize the CPU (Sets the Register values, Flags etc)
int cpu_init(GB *gb);


// Step the CPU by 1 instruction. Will return the cycles taken for that instruction.
uint32_t cpu_step(GB *gb);


// Reset the CPU completely.
void cpu_reset(GB *gb, CPU *cpu);


// POST run report:
void print_instruction_counts();

// DEBUG snapshot states:
typedef struct {
    uint8_t a;
    uint8_t f;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t pc;
    uint16_t sp;
    uint8_t mem_hl;

    uint8_t mem_sp;
    uint8_t mem_sp_plus_1;
} CpuSnapshot;



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