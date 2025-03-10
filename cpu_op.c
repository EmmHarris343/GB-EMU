#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define HEADER_START        0x0100
#define HEADER_OFFSET       0x0100      // Same as Start, just easier to understand
#define HEADER_SIZE         0x50        // Header END point is: 0x014f (50 Bytes Total)


#define ENTRY_POINT_START   0x0100
#define ENTRY_POINT_END     0x0103

#define OP_Extra_size         0x50      // Lets just... load a bunch of data for funny




typedef void opcode_t(uint8_t *val, uint8_t opcode);        // Placeholder to make code work


/*
CPU Follows individual instructions 1 by 1, by the rom Entry point.


For example, in instruction for LD (load data) HL & DE
H = High, L = Low
D = High, E = Low

A   (Accumulator)	Stores math/logical operation results
F   (Flags)	        Stores condition flags (Zero, Carry, etc.)

BC	                Counter, data storage, or address
DE	                Address pointer for data movement
HL	                Memory pointer (common for VRAM transfers)
SP	                Stack location (used for function calls)
PC	                Holds the next instruction’s memory address

Further:
HL as a memory pointer
DE as a destination address
BC as a counter

Example "Code"
-------
LD HL, $4000   ; ROM address of tile data
LD DE, $9000   ; VRAM address for tiles
LD C, $10      ; Copy 16 bytes (1 tile)

Loop:
    LD A, (HL)  ; Load byte from ROM
    LD (DE), A  ; Store byte in VRAM
    INC HL      ; Move to next ROM byte
    INC DE      ; Move to next VRAM byte
    DEC C       ; Decrease counter
    JR NZ, Loop ; Repeat until all bytes copied
-------






THE RAW BITS Of OP Codes:
LD HL $1234
00100001 00110100 00010010

LD:
(LD HL, nn) => 00100001 → Opcode 0x21
00110100 00010010 → Operand 0x1234 (Little-endian)



SOME OP CODES FOR LD:
Instruction	    Opcode	    Bytes	    Description                         What it does:
LD HL, nn	    0x21	    3	        Load 16-bit immediate into HL       (MEMORY POINTER)
LD DE, nn	    0x11	    3	        Load 16-bit immediate into DE       (DESTINATION Address)

LD C, n	        0x0E	    2	        Load 8-bit immediate into C
LD A, (HL)	    0x7E	    1	        Load A from memory at HL            (Used to copy byte to HL) - Loop
LD (HL), A	    0x77	    1	        Store A at memory HL                (Copy from HL to DE Destination) - Loop
LD (nnnn), A	0xEA nn nn	3	        Store A at memory address nnnn




SOOOOOOOOOOO.....................

If looking for a Execution to load data into the HL, then Load it into DE/ "VRAM".
I would be looking for in binary:

LD:
0010 0001
0x21 (Or in Hex)

DE:
0001 0001
0x11 (Or in Hex)


*/ 


/*


BLOCK 0: (MAybe not right... or at least confusing..)

                7	6	5	        4	3	2	1	0
ld r16, imm16	0	0	Dest (r16)	    0	0	0	1
ld [r16mem], a	0	0	Dest (r16mem)	0	0	1	0
ld a, [r16mem]	0	0	Source (r16mem)	1	0	1	0
ld [imm16], sp	0	0	0	    0       1	0	0	0

BLOCK 3: (Less confusing... I guess)
	            7	6	5	4	    3	2	1	0
ldh [c], a	    1	1	1	0	    0	0	1	0
ldh [imm8], a	1	1	1	0	    0	0	0	0
ld [imm16], a	1	1	1	0	    1	0	1	0
ldh a, [c]	    1	1	1	1	    0	0	1	0
ldh a, [imm8]	1	1	1	1	    0	0	0	0
ld a, [imm16]	1	1	1	1	    1	0	1	0
*/

// OP CODE EXAMPLE:

// typedef struct {
//     const char *mnemonic;
//     uint8_t length;
//     uint8_t cycles;
//     void (*execute)();
// } Opcode;

// Opcode opcode_table[256] = {
//     [0x00] = {"NOP", 1, 4, "op_NOP"},               //    These are not usually in Quotes
//     [0xC3] = {"JP nn", 3, 16, "op_JP_nn"},
//     [0xFE] = {"CP A, n8", 2, 8, "op_CP_A_n8"},
//     // More opcodes...
// };

// void execute_opcode(uint8_t opcode) {
//     if (opcode_table[opcode].execute) {
//         opcode_table[opcode].execute();
//     } else {
//         printf("Unknown opcode: %02X\n", opcode);
//     }
// }







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



    // Next.. FE 
    // CP A, n8
    // Z 1 H C      // Flags (Always in this Order Z 1 H C To say on or off.. I guess . 
    
    // Example:
    // AND A, B
    // Z010


    // Z = Zero Flag.       This bit is set if and only if the result of an operation is zero. -- Used by conditional jumps
    // 1 = Flag is Set      The Subtraction Flag    (BCD)
    // H = Half Carry Flag  **H indicates carry for the lower 4 bits of the result (BCD)
    // C = Carry Flag.      Complicated... See documentations, lots of conditions.


    static void nop(uint8_t *gb, uint8_t opcode);               // uint8_t is soemthing else

    static void ld_rr_d16(uint8_t *gb, uint8_t opcode);    // uint8_t is soemthing else


    static opcode_t *opcodes[256] = {
            nop,        ld_rr_d16, 
    };





void cpu_read_codes(const char *filename, uint8_t *entry_point) {
    FILE *file = fopen(filename, "rb");     // rb = Read bytes of the file.
    if (!file) {
        perror("Error reading ROM file\n");
        return;
    }

    printf("Entry Points: (0)%02X (1)%02X (2)%02X\n", entry_point[0], entry_point[1], entry_point[2]);
    // First Val is C3 (Jump).
    // On Graph: 
    // JP a16       - Jump Point, a16 : Means little-endian 16-bit Address.

    // So the next values are:
    // BE, 00. Little Endian means. Least significant Byte first.

    // SO. :: JUMP TO 0x00BE ::

    // THIS IS NOT EFFICIENT. Reading the phsyical Rom reach time...



    uint8_t opcode;
    uint16_t jump_address;

    jump_address = 0x00BE;      // 190

    uint8_t next_op[OP_Extra_size] = {0};


    fseek(file, jump_address, SEEK_SET);
    fread(next_op, 1, 10, file);
    fclose(file);



    for (int op_= 0; op_ < 10; op_++) {
        printf("Next Op code? %02X\n", next_op[op_]);
    }
    // Gives FE 11
    // FE = CP A,r8     (ComPare)
    
    // This Operation says:
    // Subtract the value in r8 from A and set flags accordingly, but don't store the result. This is useful for ComParing values.
    // Z = Set if Result is 0
    // N = 1
    // H = Set if borrow from bit is 4
    // C = Set if borrow (IE: if r8 > A)


}








int main() {
    printf("Startup.. Bla bla\n");
    const char *rom_file = "rom/pkmn_red.gb";
    printf("NOTE: Using rom file: %s\n\n", rom_file);



    uint8_t *entry_point = get_entry_point(rom_file);

    cpu_read_codes(rom_file, entry_point);




    return 0;
}