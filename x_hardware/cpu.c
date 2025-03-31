#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cpu.h"
#include "cpu_instructions.h"

#include "mmu.h"


uint8_t local_rom_entry[3];

CPU_Registers registers;        // Enable (Makes global as well) -- Struct in cpu.h file






/*

THIS IS LIKELY HOW I WILL CALL THE CPU LOOP / Interface with Memory etc




*/

void cpu_step(void) {
    //uint8_t opcode = mmu_read(cpu_state.pc);
    //decode_and_execute(opcode);
}







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



// Manually read from Rom + the Roms OP_CODES.
void manual_read_CPU(const char *filename, uint8_t *entry_point) {
    printf("Entry Points: (0)%02X (1)%02X (2)%02X\n", entry_point[0], entry_point[1], entry_point[2]);
    // BE, 00. Little Endian means. Least significant Byte first.


    
    // Gives FE 11
    // FE = CP A,r8     (ComPare)       => OP CODE: Subtract the value in r8 from A and set flags accordingly, but don't store the result. This is useful for ComParing values.

}