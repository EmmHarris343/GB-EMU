#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdint.h>

#include "../gb.h"
#include "cpu.h"
#include "cpu_instructions.h"
#include "../mmu.h"

//#include "logger.h"

uint64_t instr_count[INSTR_TYPE_COUNT] = {0};
uint64_t which_op[256] = {0};       // 64 bit, so it can store a huge amount of numbers. (Realistically, 16 bit likely fine for what I'm doing)
const char* optype_names[] = {
    "UDEF", "NOP", "ALU", "LD", "LD16", "LDH", "LDSP", "JUMP", "CALL", "POP", "PUSH", "RL_A", "RR_A", "RET", "RST", "MISC", "CB", "UNKNOWN"
};

// DMG-01
const CPU cpu_post_bios_state = {
    .reg.AF = 0x01B0,
    .reg.BC = 0x0013,
    .reg.DE = 0x00D8,
    .reg.HL = 0x014D,
    .reg.SP = 0xFFFE,
    .reg.PC = 0x0100,
    .state.IME = 0,       // Master Interupt
    .state.IME_delay = 0, // IME state after next instruction.
    .state.halt = 0,
    .state.pause = 0,
    .state.stop = 0,
    .state.panic = 0      // Mine.... if this is set. It means instruction likely wasn't made yet. IE Hard abort
};

// Was meant for the GBC, but it's a little more complicated based on headers of ROM.
const CPU cpu_reg_simple_gbc_tstate = {
    .reg.AF = 0x0100,     // B0 = 1011 (IE Z set, N not set, H set, C set)
    .reg.BC = 0x021B,
    .reg.DE = 0x032D,
    .reg.HL = 0xC100,     // This points to WRAM Work-RAM. (FOR Test Writes/ Reads.)
    .reg.SP = 0xFFFE,
    .reg.PC = 0x0779,
    .state.IME = 0,       // Master Interupt
    .state.IME_delay = 0, // IME state after next instruction.
    .state.halt = 0,
    .state.pause = 0,
    .state.stop = 0,
    .state.panic = 0      // Mine.... if this is set. It means instruction likely wasn't made yet. IE Hard abort
};


static const uint8_t opcode_lengths[256] = {
    1,3,1,1, 1,1,2,1, 3,1,1,1, 1,1,2,1,     // 0x00 - 0x0F
    2,3,1,1, 1,1,2,1, 2,1,1,1, 1,1,2,1,     // 0x10 - 0x1F
    2,3,1,1, 1,1,2,1, 2,1,1,1, 1,1,2,1,     // 0x20 - 0x2F
    2,3,1,1, 1,1,2,1, 2,1,1,1, 1,1,2,1,     // 0x30 - 0x3F
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,     // 0x40 - 0x4F
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,     // 0x50 - 0x5F
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,     // 0x60 - 0x6F
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,     // 0x70 - 0x7F
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,     // 0x80 - 0x8F
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,     // 0x90 - 0x9F
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,     // 0xA0 - 0xAF
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,     // 0xB0 - 0xBF
    1,1,3,3, 3,1,2,1, 1,1,3,1, 3,3,2,1,     // 0xC0 - 0xCF
    1,1,3,0, 3,1,2,1, 1,1,3,0, 3,0,2,1,     // 0xD0 - 0xDF
    2,1,1,0, 0,1,2,1, 2,1,3,0, 0,0,2,1,     // 0xE0 - 0xEF
    2,1,1,1, 0,1,2,1, 2,1,3,1, 0,0,2,1,     // 0xF0 - 0xFF
    // ... Fill in the rest
};



// The problem with this....
// Each time there is an empty value or field. It will default to returning 0. (IE, NOP).
// So every instruction I haven't plotted out, will default to NOP.
static const uint8_t opcode_types[256] = {
    [0x01] = INSTR_LD16,            // LD r16, n16
    [0x11] = INSTR_LD16,
    [0x21] = INSTR_LD16,
    [0x31] = INSTR_LD16,
    [0x02] = INSTR_LD,              // LD [r16], A
    [0x12] = INSTR_LD,
    [0x22] = INSTR_LD,              // LD [HL+], A
    [0x32] = INSTR_LD,              // LD [HL-], A
    [0x06] = INSTR_LD,              // LD r8, n8
    [0x16] = INSTR_LD,
    [0x26] = INSTR_LD,
    [0x36] = INSTR_LD,
    [0x0A] = INSTR_LD,              // LD r8, [r16]
    [0x1A] = INSTR_LD,
    [0x2A] = INSTR_LD,              // LD A, [HL+]
    [0x3A] = INSTR_LD,              // LD A, [HL-]
    [0x0E] = INSTR_LD,              // LD r8, n8
    [0x1E] = INSTR_LD,
    [0x2E] = INSTR_LD,
    [0x3E] = INSTR_LD,
    [0x10] = INSTR_MISC,            // STOP
    [0x08] = INSTR_LD16,            // LD [a16], SP
    [0x07] = INSTR_RL_A,
    [0x17] = INSTR_RL_A,
    [0x0F] = INSTR_RR_A,
    [0x1F] = INSTR_RR_A,
    [0x20] = INSTR_JUMP,
    [0x30] = INSTR_JUMP,
    [0x18] = INSTR_JUMP,            // JR e8
    [0x28] = INSTR_JUMP,            // JR Z, e8
    [0x38] = INSTR_JUMP,            // JR C, e8
    [0x09] = INSTR_ALU,             // ADD r16, r16
    [0x19] = INSTR_ALU,             // ADD r16, r16
    [0x29] = INSTR_ALU,             // ADD r16, r16
    [0x39] = INSTR_ALU,             // ADD r16, r16
    [0x03 ... 0x05] = INSTR_ALU,    // INC r16, INC r8, DEC r8 etc..
    [0x13 ... 0x15] = INSTR_ALU,
    [0x23 ... 0x25] = INSTR_ALU,
    [0x33 ... 0x35] = INSTR_ALU,
    [0x0B ... 0x0D] = INSTR_ALU,    // DEC r16, INC r8, DEC r8 etc...
    [0x1B ... 0x1D] = INSTR_ALU,
    [0x2B ... 0x2D] = INSTR_ALU,
    [0x3B ... 0x3D] = INSTR_ALU,
    [0x27] = INSTR_MISC,            // DAA
    [0x37] = INSTR_MISC,            // SCF
    [0x2F] = INSTR_MISC,            // CPL
    [0x3F] = INSTR_MISC,            // CCF
    [0x40 ... 0x75] = INSTR_LD,     // LD r8 r8
    [0x76] = INSTR_MISC,            // HALT (middle of LD instructions)
    [0x77 ... 0x7F] = INSTR_LD,     // LD r8 r8
    [0x80 ... 0xBF] = INSTR_ALU,    // BULK ADD, SUB, XOR, OR, CP etc..
    [0xCB] = INSTR_CB,              // Prefixed Instructions (CB)
    [0xC2] = INSTR_JUMP,
    [0xC3] = INSTR_JUMP,
    [0xCA] = INSTR_JUMP,
    [0xD2] = INSTR_JUMP,
    [0xDA] = INSTR_JUMP,
    [0xE9] = INSTR_JUMP,
    [0xE0] = INSTR_LDH,              // LDH [a8], A
    [0xF0] = INSTR_LDH,              // LDH A, [a8]
    [0xE2] = INSTR_LDH,
    [0xF2] = INSTR_LDH,
    [0xEA] = INSTR_LD,              // LD [a16], A
    [0xFA] = INSTR_LD,              // LD A, [a16]
    [0xF8] = INSTR_LDSP,            // LD HL, SP + e8
    [0xF9] = INSTR_LDSP,            // LD SP, HL
    [0xC1] = INSTR_POP,             // Pop, Push Call,
    [0xD1] = INSTR_POP,
    [0xE1] = INSTR_POP,
    [0xF1] = INSTR_POP,
    [0xC5] = INSTR_PUSH,
    [0xD5] = INSTR_PUSH,
    [0xE5] = INSTR_PUSH,
    [0xF5] = INSTR_PUSH,
    [0xC4] = INSTR_CALL,
    [0xD4] = INSTR_CALL,
    [0xCC] = INSTR_CALL,
    [0xDC] = INSTR_CALL,
    [0xCD] = INSTR_CALL,            // CALL a16
    [0xC6] = INSTR_ALU,             // ALU A, n8..
    [0xD6] = INSTR_ALU,
    [0xE6] = INSTR_ALU,
    [0xF6] = INSTR_ALU,
    [0xCE] = INSTR_ALU,             // ALU A, n8... continued
    [0xDE] = INSTR_ALU,
    [0xEE] = INSTR_ALU,
    [0xFE] = INSTR_ALU,
    [0xC8] = INSTR_RET,             // RET:
    [0xC9] = INSTR_RET,
    [0xD8] = INSTR_RET,
    [0xC7] = INSTR_RST,             // RST:
    [0xD7] = INSTR_RST,
    [0xE7] = INSTR_RST,
    [0xF7] = INSTR_RST,
    [0xCF] = INSTR_RST,
    [0xDF] = INSTR_RST,
    [0xEF] = INSTR_RST,
    [0xFF] = INSTR_RST,
    [0xF3] = INSTR_MISC,            // DI
    [0xFB] = INSTR_MISC             // EI
};







/// TODO: I shouldn't need this function here. Can likely move to cpu_instructions.c
uint16_t cnvrt_lil_endian(uint8_t LOW, uint8_t HIGH) {
    uint16_t cvrt_byte = (HIGH << 8) | LOW;
    return cvrt_byte;
}

// External_write/read used by cpu.c + cpu_instructions.c
void external_write(GB *gb, uint16_t addr, uint8_t write_val) {
    mmu_write(gb, addr, write_val);
}
uint8_t external_read(GB *gb, uint16_t addr_pc) {
    uint8_t read_val = 0;
    read_val = mmu_read(gb, addr_pc);

    return read_val;
}

void print_instruction_counts() {
    for (int i = 0; i < INSTR_TYPE_COUNT; i++ ) {
        printf("[%-8s]: %lu\n", optype_names[i], instr_count[i]);
    }
}

// static void cpu_trace_log_step(GB *gb, uint8_t opcode, uint32_t cycles_taken) {
//     CPU *cpu = &gb->cpu;

//     CPUTraceEntry entry = {
//         .step = gb->step_count,
//         .pc = cpu->reg.PC,
//         .opcode = opcode,
//         .cycles = cycles_taken,
//         .sp = cpu->reg.SP,
//         .a = cpu->reg.A,
//         .f = cpu->reg.F,
//         .b = cpu->reg.B,
//         .c = cpu->reg.C,
//         .d = cpu->reg.D,
//         .e = cpu->reg.E,
//         .h = cpu->reg.H,
//         .l = cpu->reg.L,
//         .ime = cpu->state.IME,
//         .ie = gb->interrupts.IE,
//         .iflag = gb->interrupts.IF
//     };
//     //cpu_trace_buffer_push(&gb->debug.cpu_trace, entry);
// }

// static void cpu_trace_buffer_dump(FILE *file, const CPUTraceBuffer *buffer) {
//     uint32_t start;
//     uint32_t i;

//     if (buffer->count == 0) {
//         fprintf(file, "CPU trace buffer empty.\n");
//         return;
//     }

//     start = (buffer->head + CPU_TRACE_CAPACITY - buffer->count) % CPU_TRACE_CAPACITY;

//     for (i = 0; i < buffer->count; i++) {
//         uint32_t index = (start + i) % CPU_TRACE_CAPACITY;
//         const CPUTraceEntry *entry = &buffer->entries[index];

//         fprintf(file,
//                 "STP=%llu PC=%04X OP=%02X CYC=%u SP=%04X "
//                 "A=%02X F=%02X B=%02X C=%02X D=%02X E=%02X H=%02X L=%02X "
//                 "IME=%u IE=%02X IF=%02X\n",
//                 (unsigned long long) entry->step,
//                 entry->pc,
//                 entry->opcode,
//                 entry->cycles,
//                 entry->sp,
//                 entry->a,
//                 entry->f,
//                 entry->b,
//                 entry->c,
//                 entry->d,
//                 entry->e,
//                 entry->h,
//                 entry->l,
//                 entry->ime,
//                 entry->ie,
//                 entry->iflag);
//     }
// }

// static void cpu_trace_buffer_init(CPUTraceBuffer *buffer) {
//     memset(buffer, 0, sizeof(*buffer));
// }

// static void cpu_trace_buffer_push(CPUTraceBuffer *buffer, CPUTraceEntry entry) {
//     buffer->entries[buffer->head] = entry;
//     buffer->head = (buffer->head + 1) % CPU_TRACE_CAPACITY;

//     if (buffer->count < CPU_TRACE_CAPACITY) {
//         buffer->count++;
//     }
// }

void opcode_tosummary(GB *gb) {
    uint8_t op_code = gb->instruction.opcode;
    //uint8_t op_code = gb->instruction.opcode; // TODO: Enable when removing instruction_T global.

    // Calculate opcode type, and save for high level overview.
    instr_type_T op_type = opcode_types[op_code];
    if (op_code == 0x00) {
        op_type = INSTR_NOP;
    }
    if (op_type >= INSTR_TYPE_COUNT) {
        op_type = INSTR_UNKNOWN;
    }
    if (op_type == 0) {
        // This is falling under UDEF (Undefined.. yet).. So what's the opcodes being called?
        which_op[op_code] ++;
    }
    instr_count[op_type]++ ;
}

/// DECODE:
// Reads the OP Code + Operands
void load_opcode(GB *gb, uint16_t addr_pc) {
    uint8_t op_code = 0x00;
    uint8_t operand1 = 0x00;
    uint8_t operand2 = 0x00;

    // Reads the OpCode + Operands -> mmu -> based on address/ PC.
    op_code = mmu_read(gb, addr_pc);
    if (opcode_lengths[op_code] >= 2) {
        operand1 = mmu_read(gb, addr_pc + 1);
    }
    if (opcode_lengths[op_code] == 3) {
        operand2 = mmu_read(gb, addr_pc + 2);
    }

    gb->instruction.opcode = op_code;
    gb->instruction.operand1 = operand1;
    gb->instruction.operand2 = operand2;
}


// Initalize the CPU, set the defaults for DMG 01 (Original) - Registers, Interupts etc.
int cpu_init(GB *gb) {
    printf(":CPU: Initialization, setting registers and settings. For VER: %s\n", "DMG 01");

    gb->instruction.opcode = 0x00;
    gb->instruction.operand1 = 0x00;
    gb->instruction.operand2 = 0x00;

    gb->step_count = 0;
    gb->cpu.cycle = 0;

    // Sets the cpu.registers to the 'typical' post-boot rom state.
    gb->cpu.reg = cpu_post_bios_state.reg;

    printf("Done. Finished init for DMG 01\n");

    return 0;
}

static void ime_delay(GB *gb)
{
    if (gb->cpu.state.IME_delay > 0) {
        gb->cpu.state.IME_delay--;

        // Only set IME to 1. If IME_delay was set, then transitioned down to 0.
        if (gb->cpu.state.IME_delay == 0) {
            //printf("IME delay reached 0. IME SET! (1)\n");
            gb->cpu.state.IME = 1;
        }
    }
}

// Step the CPU by 1 instruction. Will return the cycles taken for that instruction.
uint32_t cpu_step(GB *gb) {
    gb->cpu.cycle = 0;  // Reset the t-cycle back to 0 on each Step.

    if (cpu_interrupt_handling(gb)) { // Processed interrupt. Return 20 t-cycles
        gb->cpu.cycle = 20;
        return gb->cpu.cycle;
    }
    else if (gb->cpu.state.halt) {    // No interrupt processed, BUT Halt set. Continue in 'low-power' mode.
        gb->cpu.cycle = 4;

        //printf("Burning Halt cycles...\n");
        // This will continue to burn cycles. Until the interrupt has been processed.
        // No instructions will be loaded or executed in this state!

        return gb->cpu.cycle;
    }

    // Only get/ process opcode after any interrupts processed / halt cleared.
    load_opcode(gb, gb->cpu.reg.PC);    // Updates the instruction_t inside gb->instruction.

    if (execute_instruction(gb, &gb->cpu, gb->instruction) != 0) {
        gb->panic = 1;
        return -1;
    }

    // IME (Interupt delay logic)
    ime_delay(gb);

    // Add the opcode that was executed (if any)
    opcode_tosummary(gb);

    return gb->cpu.cycle;   // Should be set after each execution
}