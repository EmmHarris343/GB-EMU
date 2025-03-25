#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cpu.h"
#include "cpu_instructions.h"





#define HEADER_START        0x0100
#define HEADER_OFFSET       0x0100      // Same as Start, just easier to understand
#define HEADER_SIZE         0x50        // Header END point is: 0x014f (50 Bytes Total)


#define ENTRY_POINT_START   0x0100
#define ENTRY_POINT_END     0x0103

#define OP_Extra_size         0x50      // Lets just... load a bunch of data for fun






// Total WORK Ram       = 8 KiB
// Total VIDEO RAM      = 8 KiB
// CPU SPEED            = 4.194304 MHz

CPU_Registers registers;        // Enable (Makes global as well) -- Struct in cpu.h file







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




// What I actually need to do:
/*

The hex codes (like below) translates to a specific op_code
    [0x00] = op_NOP,
    [0xC3] = op_JP_nn,
    [0xFE] = op_CP_A_n8,


I need a way to be provided the current op_code. (In Hex). 
Have a table that specifies what each OP_CODE is based on the hex (So it can be pointed to the Function Pointer Table.)
Be given details about the OP_CODE, such as how many bytes it needs, 1, 2, 3 etc
    So I can know: 
    1. The data stored in memory, to complete the instruction (Add, subtract, copy, compare, load/ LD, jump address).
    2. Where the NEXT 8-BIT OP_CODE is located.
    3. So I *ultimately* know how far to advance the **PC.. 


After all that... I can just print out the op_codes and the data for now. 
But, I will need to implement JUMP codes at a bare-min, to properly jumpto / calculate the next **PC


*/






void do_opcode(uint8_t special_val) {
    // uint8_t opcode = cycle_read(gb, gb->pc++);      // Get the current op_code
    // opcodes[opcode](gb, opcode);                    // Run the op_code that's inside the Function pointer. (List of functions)

}

void cpu_initialize() {
    // GB_VER = 0x01 (DMG), only one I am programming for now..

    /*
    DMG Register values:
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



    // Set the Flag Registers, and the Registers.
    // Note, this is the intial state of the GB ver: DMG
    // AFTER the Bootrom would have run, and passed
    // (Pass meaning, ready to start reading OP_Code instructions from ROM)

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

// Gets Entry Point from ROM.
uint8_t hold_entry_point[3] = {0};   // Declare here, so the address doesn't disappear once the Function closes

uint8_t *get_entry_point(const char *filename) {

    FILE *file = fopen(filename, "rb");     // rb = Read bytes of the file.
    if (!file) {
        perror("Error reading ROM file\n");
        return 0;
    }

    // Read entire Header:

    uint8_t header[HEADER_SIZE];
    fseek(file, HEADER_START, SEEK_SET);
    fread(header, 1, HEADER_SIZE, file);
    fclose(file);

    printf(" Read Entry point: ");
    for (int i = (ENTRY_POINT_START - HEADER_START);
             i < (ENTRY_POINT_END - HEADER_START);
             i++)
    {
        printf("%02X (i: %d) ", header[i],i);
        hold_entry_point[i] = header[i];
    }
    printf("\n");

    
    return hold_entry_point;
}



// Manually read from Rom + the Roms OP_CODES.
void manual_read_CPU(const char *filename, uint8_t *entry_point) {
    FILE *file = fopen(filename, "rb");     // rb = Read bytes of the file.
    if (!file) {
        perror("Error reading ROM file\n");
        return;
    }

    printf("Entry Points: (0)%02X (1)%02X (2)%02X\n", entry_point[0], entry_point[1], entry_point[2]);
    // BE, 00. Little Endian means. Least significant Byte first.

    uint16_t jump_address;

    // Entry point value said: 0xC3 (JUMP) to BE OO (Reversed cause little endian)
    jump_address = 0x00BE;

    uint8_t next_op[OP_Extra_size] = {0};


    fseek(file, jump_address, SEEK_SET);
    fread(next_op, 1, 10, file);            // Read up to 10 values, and store into next_op[x]
    fclose(file);



    for (int op_= 0; op_ < 10; op_++) {     // Grab 10, 8bit places .. for giggles...
        printf("Next Op code? %02X\n", next_op[op_]);
    }
    
    // Gives FE 11
    // FE = CP A,r8     (ComPare)       => OP CODE: Subtract the value in r8 from A and set flags accordingly, but don't store the result. This is useful for ComParing values.

}




// Startup, get entry point from rom. BLABLA
int main_task() {
    printf("Startup.. Bla bla\n");
    const char *rom_file = "rom/pkmn_red.gb";
    printf("NOTE: Using rom file: %s\n\n", rom_file);

    cpu_initialize();
    check_registers();

    printf("Begin CPU OP Instructions.. \n ");

    uint8_t *entry_point = get_entry_point(rom_file);

    //manual_read_CPU(rom_file, entry_point);

    



    return 0;
}