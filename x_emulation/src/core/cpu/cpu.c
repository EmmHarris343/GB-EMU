#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdint.h>

#include "../gb.h"
#include "cpu.h"
#include "cpu_instructions.h"
#include "../mmu.h"

#include "../../debug/logger.h"

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

/// FETCH:
// Fetch and Decode the OPCODE + any operands.
void fetch_opcode(GB *gb, uint16_t addr_pc) {
    uint8_t op_code = 0x00;
    uint8_t operand1 = 0x00;
    uint8_t operand2 = 0x00;

    // Reads the OpCode + Operands -> mmu -> based on address/ PC.
    op_code = external_read(gb, addr_pc);
    if (opcode_lengths[op_code] >= 2) {
        operand1 = external_read(gb, addr_pc + 1);
    }
    if (opcode_lengths[op_code] == 3) {
        operand2 = external_read(gb, addr_pc + 2);
    }

    gb->instruction.opcode = op_code;
    gb->instruction.operand1 = operand1;
    gb->instruction.operand2 = operand2;
}


// Initalize the CPU, set the defaults for DMG 01 (Original) - Registers, Interupts etc.
int cpu_init(GB *gb) {
    printf(":CPU: Initializing CPU Registers, Configurations. For VER: %s\n", "DMG 01");

    gb->instruction.opcode = 0x00;
    gb->instruction.operand1 = 0x00;
    gb->instruction.operand2 = 0x00;

    gb->step_count = 0;
    gb->cpu.cycle = 0;

    // Sets the cpu.registers to the 'typical' post-boot rom state.
    gb->cpu.reg = cpu_post_bios_state.reg;

    printf("Done.\n");

    return 0;
}

static void ime_delay(GB *gb) {
    if (gb->cpu.state.IME_delay > 0) {
        gb->cpu.state.IME_delay--;

        // Only set IME to 1. If IME_delay was set, then transitioned down to 0.
        if (gb->cpu.state.IME_delay == 0) {
            gb->cpu.state.IME = 1;
            trace_general_line(gb->instruction.opcode, gb->cpu.reg.F,gb->cpu.reg.PC, 0, 0, "IME SET to 1", 14); // tag 14 = interrupt
        }
    }
}

// Step the CPU by 1 instruction. Will return the cycles taken for that instruction.
uint32_t cpu_step(GB *gb) {
    gb->cpu.cycle = 0;  // Reset the t-cycle back to 0 on each Step.

    // Check for interrupts to process first.
    if (cpu_interrupt_handling(gb)) {
        // It processed the interrupt. Set the total CPU cycles for processing interrupt, and return.
        gb->cpu.cycle = 20;
        return gb->cpu.cycle;
    }
    else if (gb->cpu.state.halt) {
        // Didn't process interrupt. Halt is set. Burn 4 additional cpu cycles.
        gb->cpu.cycle = 4;
        return gb->cpu.cycle;
    }

    // Fetch/ Decode.
    fetch_opcode(gb, gb->cpu.reg.PC);

    // Execute.
    if (execute_instruction(gb, &gb->cpu, gb->instruction) != 0) {
        gb->panic = 1;
        return -1;
    }

    // IME (Interupt delay logic)
    ime_delay(gb);

    return gb->cpu.cycle;
}