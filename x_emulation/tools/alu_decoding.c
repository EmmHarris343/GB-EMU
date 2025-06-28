#include "stdint.h"

enum alu_type {
    ADD, ADC, SUB, SBC, 
    AND, XOR, OR, CP,
    INVALID
};

enum from_type {
    NONE,   // Can be INC, DEC etc
    reg,    
    n8,
    n16,
    hl_ptr, // [HL]
    UNKNOWN
};

struct dco_op {
    enum alu_type op;
    enum from_type rand;
};

// I confused myself. this doesn't really do what I was hoping for. 


struct dco_op d_alu(uint8_t opcode) {
    // This will only handle 8bit value in [HL], r8, n8 values.
    struct dco_op result = {
        .op = INVALID,
        .rand = UNKNOWN
    };

    // n8:
    if ((opcode & 0xC7) == 0xC6) {
        result.rand = n8;
        // will need to op / alu_type... (not done yet)
        return result;
    }

    // r8, or [HL]
    if ((opcode & 0xC0) == 0x80) {
        uint8_t sc = (opcode >>3) & 0x07;
        uint8_t regv = opcode & 0x07;

        result.op = (enum alu_type)sc;

        if (regv == 6) {
            result.rand = hl_ptr;
        }
        else {
            result.rand = reg;
        }
        return result;
    }
    // By default this will be invalid/ unknown
    return result;
}