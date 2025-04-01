#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cpu.h"
#include "cpu_instructions.h"

#include "mmu_interface.h"


uint8_t local_rom_entry[3];

CPU_Registers registers;        // Enable (Makes global as well) -- Struct in cpu.h file






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

void lookup_opcode_len(uint8_t opcode) {

}



void decode_opcode(uint8_t curr_addr_val) {

}


// Each CPU_STEP is called ..

void cpu_step(void) {
    //uint8_t opcode = mmu_read(cpu_state.pc);
    //decode_and_execute(opcode);
}





/// PROBLEM: I thought registers were 0/ 1. But they can hold values. 
/// DO: That means entire Flag system needs to be overhauled

void set_flag(int cpu_flag) {
    
    switch (cpu_flag) {
        case 0: // Z Flag
            //printf("Set z Flag\n");
            registers.f |= FLAG_Z;
        case 1: // N Flag
            //printf("Set N Flag\n");
            registers.f |= FLAG_N;
        case 2: // H Flag
            //printf("Set H Flag\n");
            registers.f |= FLAG_H;
        case 3: // C Flag
            //printf("Set C Flag\n");
    }
}

void clear_flag(int cpu_flag) {
    switch (cpu_flag) {
        case 0: // Z Flag
            registers.f &= ~FLAG_Z;
        case 1: // N Flag
            registers.f &= ~FLAG_N;
        case 2: // H Flag
            registers.f &= ~FLAG_H;
        case 3: // C Flag
            registers.f &= ~FLAG_C;
    }
}


// REMINDER:
// CPU LOOP IS : Fetch-Decode-Execute
// 1. Fetch the opcode from memory at the current PC (program counter).
// 2. Decode the opcode to determine what it does and how many additional bytes it needs.
// 3. Execute the instruction and update PC accordingly.
// 4..... Repeat until Emulation stops.







void cpu_init(uint8_t *rom_entry) {         // Initialize this to the DMG   (Original)

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



    // Set Flag Registers   (This is actually Registers F)
    set_flag(0);    // Z
    clear_flag(1);  // N
    clear_flag(2);  // H
    clear_flag(3);  // C

    // Set the Registers initial state (After Bootrom Pass)
    registers.a = 0x01;
    registers.b = 0x00;
    registers.c = 0x13;
    registers.d = 0x00;
    registers.e = 0xD8;
    registers.h = 0x01;
    registers.l = 0x4D;

    registers.pc = 0x0100;
    registers.sp = 0xFFFE;
}

void check_registers() {
    printf("Registers:\n");
    (registers.f & FLAG_Z) ? printf(" - Zero Flag set\n") : printf(" - Zero Flag NOT set\n");
    (registers.f & FLAG_N) ? printf(" - N Flag set\n") : printf(" - N Flag NOT set\n");
    (registers.f & FLAG_H) ? printf(" - H Flag set\n") : printf(" - H Flag NOT set\n");
    (registers.f & FLAG_C) ? printf(" - C Flag set\n") : printf(" - C Flag NOT set\n");
    printf("A: 0x%X\n", registers.a);
    printf("B: 0x%X, C: 0x%X\n", registers.b, registers.c);
    printf("D: 0x%X, E: 0x%X\n", registers.b, registers.c);
    printf("H: 0x%X, L: 0x%X\n", registers.h, registers.l);
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
    printf("Cnvrt FROM LIL Endian => LOW: %02X | HIGH %02X Little Endian Output: %04X\n", LOW, HIGH, cvrt_byte);
    return cvrt_byte;
}


void read_opcode_val(uint16_t addr_pc) {
    int len = 2;            // OP code is included in 3 bit lengths.
    uint8_t value[3];       // Each value will be split into 8bit Values regardless..

    printf("Fetching %d bytes from PC: 0x%04X\n", len, addr_pc);
    // value[1] = mmu_read(addr_pc + 1);
    // printf("value 1, %02X\n", value[1]);

    //mmu_debugger(addr_pc);

    for (int i = 0; i <= len; i++ ) {
        value[i] = mmu_read(addr_pc + i);
        printf("Byte %d @ 0x%04X: %02X\n", i, addr_pc + i, value[i]);
    }

    uint8_t cpu_opcode = value[0];
    printf("Next Op_Code: %02X\n", cpu_opcode);

}


void test_step_instruction() {
    
    // GB Startup, Jumps to $0100 (Entry point in header)
    registers.pc = 0x0100;

    uint8_t init_op = local_rom_entry[0];

    // Usually this Entry point is a NOP, Followed by a Jump instruction. Lets find out.
    printf("Rom Entry point, First OP_CODE %02x\n", init_op);
    
    // First OP Code is:
    // C3 = JP a16 (3 byte, including OP Code)

    // C3 50 01    ; JP 0x0150
    // C3 = 1 Byte, 50 = 1 Byte, 01 = 1 Byte.

    printf("What is the Entry point values.. [1] %02X, [2] %02X\n", local_rom_entry[1], local_rom_entry[2]);   
    uint16_t a16_byte = cnvrt_lil_endian(local_rom_entry[1], local_rom_entry[2]);
    
    printf("The 16bit 2Byte Value? %04X\n", a16_byte);

    // Command: Jump to 16 Bit address.
    registers.pc = a16_byte;
    printf("Jumping to Address: 0x%04X\n", a16_byte);

    /// NEXT:
    // Read the memory at the new PC location.

    read_opcode_val(registers.pc);


    
    // Don't need to read the PC 
    //uint8_t opcode = mmu_read(registers.pc);

    // Rename this to CPU State?
    //uint8_t opcode = mmu_read(cpu_state.pc);
}



// Manually read from Rom + the Roms OP_CODES.
void manual_read_CPU(const char *filename, uint8_t *entry_point) {
    printf("Entry Points: (0)%02X (1)%02X (2)%02X\n", entry_point[0], entry_point[1], entry_point[2]);
    // BE, 00. Little Endian means. Least significant Byte first.


    
    // Gives FE 11
    // FE = CP A,r8     (ComPare)       => OP CODE: Subtract the value in r8 from A and set flags accordingly, but don't store the result. This is useful for ComParing values.

}