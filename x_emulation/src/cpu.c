#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
// #include <stdlib.h>
#include <stdint.h>

#include "cpu.h"
#include "cpu_instructions.h"

#include "mmu_interface.h"

uint8_t local_rom_entry[3];

CPU local_cpu;

instruction_T op_instruction;

CPU_STATE cpu_status;



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

/// TODO: Flag system works, but isn't the most intuitive, as flags have "0 = Z, 1 = N, 2 = 3....." 
void set_flag(int cpu_flag) {   
    switch (cpu_flag) {
        case 0: // Z Flag
            //printf("Set z Flag\n");
            local_cpu.F |= FLAG_Z;
            break;
        case 1: // N Flag
            //printf("Set N Flag\n");
            local_cpu.F |= FLAG_N;
            break;
        case 2: // H Flag
            //printf("Set H Flag\n");
            local_cpu.F |= FLAG_H;
            break;
        case 3: // C Flag
            //printf("Set C Flag\n");
            local_cpu.F |= FLAG_C;
            break;
    }
}

void clear_flag(int cpu_flag) {
    switch (cpu_flag) {
        case 0: // Z Flag
            local_cpu.F &= ~FLAG_Z;
            break;
        case 1: // N Flag
            local_cpu.F &= ~FLAG_N;
            break;
        case 2: // H Flag
            local_cpu.F &= ~FLAG_H;
            break;
        case 3: // C Flag
            local_cpu.F &= ~FLAG_C;
            break;
    }
}

void cpu_init(uint8_t *rom_entry) {         // Initialize this to the DMG   (Original)

    printf(":CPU: Initialization, Setting registers and Settings. For VER: %s\n", "DMG 01");
    // Make Entry point CPU.C Local Variable:
    for (int i = 0; i <= 3; i++ ){
        local_rom_entry[i] = rom_entry[i];
    }

    // Set the operands:
    op_instruction.opcode = 0;
    op_instruction.operand1 = 0;
    op_instruction.operand2 = 0;

    // Set Flag Registers   (This is actually Registers F)
    set_flag(0);    // Z
    clear_flag(1);  // N
    clear_flag(2);  // H
    clear_flag(3);  // C

    // Set the Registers initial state (After Bootrom Pass) -- I think this is what DMG01 looks like after bootup
    local_cpu.A = 0x01;
    local_cpu.B = 0x00;
    local_cpu.C = 0x13;
    local_cpu.D = 0x00;
    local_cpu.E = 0xD8;
    local_cpu.H = 0x01;
    local_cpu.L = 0x4D;

    local_cpu.PC = 0x0100;
    local_cpu.SP = 0xFFFE;
    printf("Finished init for DMG 01\n");
}

void check_registers() {
    printf("\n:CPU: === Registers === \n");
    (local_cpu.F & FLAG_Z) ? printf("  Z Flag set | ") : printf("  Z Flag NOT set | ");
    (local_cpu.F & FLAG_N) ? printf("  N Flag set | ") : printf("  N Flag NOT set | ");
    (local_cpu.F & FLAG_H) ? printf("  H Flag set | ") : printf("  H Flag NOT set | ");
    (local_cpu.F & FLAG_C) ? printf("  C Flag set") : printf("  C Flag NOT set");
    printf("\n");
    printf("  A: 0x%02X\n", local_cpu.A);
    printf("  B: 0x%02X, C: 0x%02X\n", local_cpu.B, local_cpu.C);
    printf("  D: 0x%02X, E: 0x%02X\n", local_cpu.D, local_cpu.E);
    printf("  H: 0x%02X, L: 0x%02X\n", local_cpu.H, local_cpu.L);
    printf("\n");
}


uint16_t cnvrt_lil_endian(uint8_t LOW, uint8_t HIGH) {
    uint16_t cvrt_byte = (HIGH << 8) | LOW;
    //printf("Cnvrt FROM LIL Endian => LOW: %02X | HIGH %02X Little Endian Output: %04X\n", LOW, HIGH, cvrt_byte);
    return cvrt_byte;
}

void external_write(uint16_t addr, uint8_t write_val) {
    mmu_write(addr, write_val);
}

uint8_t external_read(uint16_t addr_pc) {
    uint8_t int8_val = 0;
    int8_val = mmu_read(addr_pc);

    return int8_val;
}


/// DECODE:
// Reads the OPP Code, 
void extract_opcode(uint16_t addr_pc) {
    uint8_t op_code = 0;
    uint8_t operand1 = 0;
    uint8_t operand2 = 0;   
    
    op_code = mmu_read(addr_pc);
    printf("OP_CODE (Read from Ram)%04X\n", op_code);
    if (opcode_lengths[op_code] >= 2) {
        // printf(":CPU: 8Bit Operand ");
        operand1 = mmu_read(addr_pc + 1);
    }
    if (opcode_lengths[op_code] == 3) {
        // printf(":CPU: Second 8Bit Operand ");
        operand2 = mmu_read(addr_pc + 2);
    }

    op_instruction.opcode = op_code;
    op_instruction.operand1 = operand1;
    op_instruction.operand2 = operand2;
}

// Might change later, but this mostly just Executes the CPU Instruction
void step_cpu(int step_count) {
    if (execute_instruction(&local_cpu, op_instruction, step_count) != 0) {
        printf(":CPU: Error Executing CPU instruction!\n");
    }
}

/// START:
// Startup and RUN CPU. (Note with a limit of Max Steps)
void run_cpu(int max_steps) {
    printf(":CPU: RUN_CPU Started. Running test run: MAX STEPS: %0X\n", max_steps);
    for (int i = 0; i < max_steps; i++) {
        printf("\n[STEP %03d]", i);
        extract_opcode(local_cpu.PC);
        step_cpu(i);
        //check_registers();
        //if (cpu_status.halt == 1) i = max_steps;
        if (cpu_status.panic == 1) {
            printf("::CPU:: PANIC HALT detected. Breaking CPU loop.\n"); 
            break;
        }
    }
    printf("::CPU:: Reached CPU Step Limit, STOPPING\n");
}



// *----
/// TEST:
// *----

void tstate_set_opcode(uint8_t test_opcode, uint8_t op1, uint8_t op2) {
        // Set the opcode and operands:
        op_instruction.opcode = test_opcode;
        op_instruction.operand1 = op1;
        op_instruction.operand2 = op2;
}

void tstate_set_registers() {
        // Set the Registers initial state (After Bootrom Pass) -- I think this is what DMG01 looks like after bootup
        local_cpu.A = 0x01;
        local_cpu.B = 0x00;
        local_cpu.C = 0x13;
        local_cpu.D = 0x00;
        local_cpu.E = 0xD8;
        local_cpu.H = 0x01;
        local_cpu.L = 0x4D;
    
        local_cpu.PC = 0x0100;
        local_cpu.SP = 0xFFFE;
}

void tstate_set_flag(){
    // Certain instructions will not run unless specific flags set.
    // IE: Subtraction (N)
    // IE: CC (condition)... Such as: JP NZ (Will only jump if Z not set!)

    // Set Flag Registers   (This is actually Registers F)
    set_flag(0);    // Z
    clear_flag(1);  // N
    clear_flag(2);  // H
    clear_flag(3);  // C

}

void tstate_set_ram() {
    // Write to a specific area of RAM, 
    // So when it loads from that area. It can be verified to be something other than giberish or, all 0s

}


// This ensures everything is done, then finally calls the Execute Instruction
void run_cpu_test() {
    int step_count = 0;             // Will I ever use this for tests?
    uint8_t special_op_code = 0x5C; // 5C = LD E, H
    
    tstate_set_opcode(special_op_code, 0, 0);            // The Opcode (& the Operands), to pass to CPU Instruction
    
    tstate_set_registers();
    tstate_set_flag();              // How do I know which flag Needs to be set?
    tstate_set_ram();

    printf(":CPU: Initial CPU state set. Ready for Execution of Test\n");

    printf("Showing Registers first, making sure working..\n\n");
    check_registers();


    if (execute_test(&local_cpu, op_instruction) != 0) {
        printf(":CPU: Error Executing CPU instruction!\n");
    }

    printf("Post Execution Registers... \n");
    check_registers();
}