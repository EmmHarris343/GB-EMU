typedef void opcode_t(uint8_t *gb_val, uint8_t opcode);        // Placeholder to make code work



/*
    --      CPU FLAGS        --

(Z) Zero flag:
    This flag is set if and only if the result of an operation is zero. Used by conditional jumps.

(C) Carry Flag (C or Cy):
    Flag is set in these cases:

    1. 8-bit Addition is > $FF.
    2. 16-bit Addition is > $FFFF.
    3. When Sub or ComPare lower than zero.
    4. If shift/ rotate shifts out a "1" bit.
* NOTE: this used by conditional jumps and certain instructions, ADC, SBC ..

(N) Subtraction Flag: 
    N indicates whether the previous instruction has been a subtraction
(H) Half Carry Flag:
    H indicates carry for the lower 4 bits of the result
* NOTE: Both N & H flags are ONLY used, when:
    An Arithmatic action is performed on a Value originally in BCD Format (Binary-Coded-Decimal).
    Which the DAA Function will convert back to BCD, depending on the Value in A, the N, H, (and sometimes) Cary flags.
    
|| CPU DAA      : https://rgbds.gbdev.io/docs/v0.9.1/gbz80.7#DAA
|| N & H Flags  : https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-bcd-flags-n-h

*/
struct cpu_flag{
    uint8_t z;      // Zero flag
    uint8_t n;      // Subtraction flag   (BCD / DAA Only)
    uint8_t h;      // Half Carry flag    (BCD / DAA Only)
    uint8_t c;      // Carry flag
};












/*

CPU OP_CODES       ||   Actual Logic and Functionality

*/

static void nop(uint8_t *gb, uint8_t opcode)            // Placeholder..
{
}

static void ld_rr_d16(uint8_t *gb, uint8_t opcode)      // Placeholder..
{
    // Based on Input value of opcode... wait.. why opcode?

    // Take the OP_Code, then taking values from memory, or what ever else is needed.. 
    // Update Register Values, Or Flags. Then Stop.


    // Recommended: Pass in pointer to value, that way value is edited inside the pointer. 
    // Instead of needing to return anything.




    // This is an example (Don't JUST copy. LEARN!)
    // uint8_t register_id;
    // uint16_t value;
    // register_id = (opcode >> 4) + 1;
    // value = cycle_read(gb, gb->pc++);
    // value |= cycle_read(gb, gb->pc++) << 8;
    // gb->registers[register_id] = value;
}

// Other functions..

static opcode_t *opcodes[256] = {
        nop,        ld_rr_d16, 
};

static uint8_t get_cycle(uint8_t *gb_val, uint16_t addr)
{
    // Do.. something..
    // Such as, read the next address point. 

    // Then maybe return a address of what to read next?


    return 0;


    // if (gb->pending_cycles) {
    //     GB_advance_cycles(gb, gb->pending_cycles);
    // }
    // gb->address_bus = addr;
    // uint8_t ret = GB_read_memory(gb, addr);
    // gb->pending_cycles = 4;
    // return ret;
}

void execute_cpu(uint8_t *gb_val) {
    uint8_t opcode = 0;     // Placeholder. Not positive what to put here...
    //uint8_t opcode = get_cycle(gb_val, gb_val->pc++);



// OP_CODE EXAMPLE:

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


}

