#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
// #include <stdlib.h>
#include <stdint.h>

#include "cpu.h"
#include "cpu_instructions.h"

#include "mmu_interface.h"

uint8_t local_rom_entry[3];

CPU loc_cpu;

instruction_T op_instruction;

CPU_STATE cpu_status;




/// LOOP: 
/*
    main loop is as follows:
    PC -> Read PC memory location
    Decode OP Code
    Determin OP Code + byte Length
    Execute OP Code and Pass Bytes
    Update PC based on byte length
    -- IF JUMP OP Code, Update PC to the new Location.
    
*/

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


/// PROBLEM: I thought registers were 0/ 1. But they can hold values. 
/// DO: That means entire Flag system needs to be overhauled

void set_flag(int cpu_flag) {
    
    switch (cpu_flag) {
        case 0: // Z Flag
            //printf("Set z Flag\n");
            loc_cpu.F |= FLAG_Z;
            break;
        case 1: // N Flag
            //printf("Set N Flag\n");
            loc_cpu.F |= FLAG_N;
            break;
        case 2: // H Flag
            //printf("Set H Flag\n");
            loc_cpu.F |= FLAG_H;
            break;
        case 3: // C Flag
            //printf("Set C Flag\n");
            loc_cpu.F |= FLAG_C;
            break;
    }
}

void clear_flag(int cpu_flag) {
    switch (cpu_flag) {
        case 0: // Z Flag
            loc_cpu.F &= ~FLAG_Z;
            break;
        case 1: // N Flag
            loc_cpu.F &= ~FLAG_N;
            break;
        case 2: // H Flag
            loc_cpu.F &= ~FLAG_H;
            break;
        case 3: // C Flag
            loc_cpu.F &= ~FLAG_C;
            break;
    }
}


// REMINDER:
// CPU LOOP IS : Fetch-Decode-Execute
// 1. Fetch the opcode from memory at the current PC (program counter).
// 2. Decode the opcode to determine what it does and how many additional bytes it needs.
// 3. Execute the instruction and update PC accordingly.
// 4..... Repeat until Emulation stops.







void cpu_init(uint8_t *rom_entry) {         // Initialize this to the DMG   (Original)

    printf(":CPU: Initialization, Setting registers and Settings. For VER: %s\n", "DMG 01");
    // Make Entry point CPU.C Local Variable:
    for (int i = 0; i <= 3; i++ ){
        local_rom_entry[i] = rom_entry[i];
    }

    // printf(":cpu.c: -> CPU_INIT Rom entry point print\n");
    // for (int i = 0; i <= 3; i++)
    // {
    //     printf("%02X ", local_rom_entry[i]);
    // }
    // printf("\n");
    

    /*
    DMG *Original Register values:
    A	$01	
    F	Z=1 N=0 H=? C=?
    B	$00	
    C	$13
    D	$00
    E	$D8
    H	$01
    L	$4D
    PC	$0100
    SP	$FFFE

    NOTE: 
    IF header checksum is $00.. Then:
    Carry and half-carry flags are clear (FOR DMG); otherwise, they are both set.
    */

    // Set the operands:
    op_instruction.opcode = 0;
    op_instruction.operand1 = 0;
    op_instruction.operand2 = 0;


    // Set Flag Registers   (This is actually Registers F)
    set_flag(0);    // Z
    clear_flag(1);  // N
    clear_flag(2);  // H
    clear_flag(3);  // C

    // Set the Registers initial state (After Bootrom Pass)
    
    loc_cpu.A = 0x01;
    loc_cpu.B = 0x00;
    loc_cpu.C = 0x13;
    loc_cpu.D = 0x00;
    loc_cpu.E = 0xD8;
    loc_cpu.H = 0x01;
    loc_cpu.L = 0x4D;

    loc_cpu.PC = 0x0100;
    loc_cpu.SP = 0xFFFE;
    printf("Finished init for DMG 01\n");
}

void check_registers() {
    printf("\n:CPU: === Registers === \n");
    (loc_cpu.F & FLAG_Z) ? printf("  Z Flag set | ") : printf("  Z Flag NOT set | ");
    (loc_cpu.F & FLAG_N) ? printf("  N Flag set | ") : printf("  N Flag NOT set | ");
    (loc_cpu.F & FLAG_H) ? printf("  H Flag set | ") : printf("  H Flag NOT set | ");
    (loc_cpu.F & FLAG_C) ? printf("  C Flag set") : printf("  C Flag NOT set");
    printf("\n");
    printf("  A: 0x%02X\n", loc_cpu.A);
    printf("  B: 0x%02X, C: 0x%02X\n", loc_cpu.B, loc_cpu.C);
    printf("  D: 0x%02X, E: 0x%02X\n", loc_cpu.D, loc_cpu.E);
    printf("  H: 0x%02X, L: 0x%02X\n", loc_cpu.H, loc_cpu.L);
    printf("\n");
}

/// NOTICE: CC Condition codes. 
/*
cc = A condition code:
    Z    Execute if Z is set.
    NZ   Execute if Z is not set.
    C    Execute if C is set.
    NC   Execute if C is not set. 

*/


uint16_t cnvrt_lil_endian(uint8_t LOW, uint8_t HIGH) {
    uint16_t cvrt_byte = (HIGH << 8) | LOW;
    //printf("Cnvrt FROM LIL Endian => LOW: %02X | HIGH %02X Little Endian Output: %04X\n", LOW, HIGH, cvrt_byte);
    return cvrt_byte;
}


uint8_t get_op_len(uint8_t opcode) {
    //printf(":CPU: ---== Amount of Bytes? ==--- %01X\n\n", opcode_lengths[opcode]);
    return opcode_lengths[opcode];
}

void external_write(uint16_t addr, uint8_t write_val) {
    mmu_write(addr, write_val);
}

uint8_t external_read(uint16_t addr_pc) {
    uint8_t int8_val = 0;
    int8_val = mmu_read(addr_pc);

    return int8_val;
}

void step_cpu(uint16_t addr_pc, int step_count) {
    //printf("Entered Step CPU\n");
    uint8_t op_code = 0;
    uint8_t operand1 = 0;
    uint8_t operand2 = 0;
    
    uint8_t op_code_length = 0;

    
    // Get OP_CODE From the PC_ADDR
    //printf(":CPU: Step CPU, OP, Length, Operands\n");
    op_code = mmu_read(addr_pc);
    //mmu_debugger(addr_pc);
    op_code_length = get_op_len(op_code);
    // printf(":CPU: OPCODE LEN: %02X ", op_code_length);
    // printf(":CPU: SP Addr: 0x%02X ", loc_cpu.SP);
    // printf(":CPU: PC Addr: 0x%02X ", addr_pc);
    
    if (op_code_length >= 2) {
        // printf(":CPU: 8Bit Operand ");
        operand1 = mmu_read(addr_pc + 1);
    }
    if (op_code_length == 3) {
        // printf(":CPU: Second 8Bit Operand ");
        operand2 = mmu_read(addr_pc + 2);
    }
    // printf("\n");

    op_instruction.opcode = op_code;
    op_instruction.operand1 = operand1;
    op_instruction.operand2 = operand2;
    
    //printf(":CPU: Stepping... Opcode: %02X, Op1: %02X, Op2: %02X\n", op_instruction.opcode, op_instruction.operand1, op_instruction.operand2);

    //printf(":CPU: Stepping... \n  PC: %04X\n  OPCODE: %02X\n  OP_Length: %02X\n", addr_pc, op_instruction.opcode, op_code_length);
    //printf(":CPU: OP1: %02X OP2: %02X\n", operand1, operand2);

    if (execute_instruction(&loc_cpu, op_instruction, step_count) != 0) {
        printf(":CPU: Error Executing CPU instruction!\n");
    }
    //printf(":CPU: Finished CPU Step\n");
}


// Basic test of CPU. Max steps makes it so it only runs a few CPU steps at a time. To test output / functionality.
void run_cpu(int max_steps) {
    printf(":CPU: RUN_CPU Started. Running test run: MAX STEPS: %0X\n", max_steps);
    for (int i = 0; i < max_steps; i++) {
        printf("\n[STEP %03d]", i);
        step_cpu(loc_cpu.PC, i);
        //check_registers();
        //if (cpu_status.halt == 1) i = max_steps;
        if (cpu_status.panic == 1) {
            printf("::CPU:: PANIC HALT detected. Breaking CPU loop.\n"); 
            break;
        }
    }
    printf("::CPU:: Reached CPU Step Limit, STOPPING\n");   
}


        //printf("\n\n===================\n:CPU: Step Count:(%d / %d) \n", i, max_steps);
        //check_registers();
        //printf(":CPU: PC Counter: 0x%04X\n", loc_cpu.PC);
