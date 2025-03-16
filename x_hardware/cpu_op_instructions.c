#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


typedef void opcode_t(uint8_t *gb_val, uint8_t opcode);        // Placeholder to make code work


// NOP - No operation
static void nop(uint8_t *gb, uint8_t opcode)            // Placeholder..
{

}



// Load instructions

static void ld_r8_r8(uint8_t *gb, uint8_t opcode) {

}

static void ld_r8_n8(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_r16_r16(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_p_HL_r8(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_p_HL_n8(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_r8_p_HL(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_p_r16_A(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_p_n16_A(uint8_t *gb, uint8_t opcode) {
    
}

static void ldh_p_n16_A(uint8_t *gb, uint8_t opcode) {
    
}

static void ldh_p_C_A(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_A_p_r16(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_A_p_n16(uint8_t *gb, uint8_t opcode) {
    
}

static void ldh_A_p_n16(uint8_t *gb, uint8_t opcode) {
    
}

static void ldh_A_p_C(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_p_HLI_A(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_p_HLD_A(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_A_p_HLD(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_A_p_HLI(uint8_t *gb, uint8_t opcode) {
    
}



// Load Instructions:
static void ld_SP_n16(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_p_n16_sp(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_HL_SP_plus_e8(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_SP_HL(uint8_t *gb, uint8_t opcode) {
    
}


// Jump instructions

static void jp_hl(uint8_t *gb, uint8_t opcode) {
    // Bytes 1
    // Cycles 1
    // Flags Changed, none

    // Jump to address in HL; effectively, copy the value in register HL into PC.
    // COPY HL into PC
    
}


static void jr_n16(uint8_t *gb, uint8_t opcode) {
    // Cycles 3
    // Bytes 2
    // Flags Changed, none

    // The address is encoded as a signed 8-bit offset from the address immediately following the JR instruction, so the target address n16 must be between -128 and 127 bytes away. For example:
    // Example: 
    // JR Label  ; no-op; encoded offset of 0
    // JR Label  ; infinite loop; encoded offset of -2


}

static void jr_cc_n16(uint8_t *gb, uint8_t opcode) {
    // Relative Jump to address n16 if condition cc is met.
    // Cycles: 3 taken / 2 untaken
    // Bytes: 2
    // Flags Changed, None
    
}










static void ld_rr_d16(uint8_t *gb, uint8_t opcode)      // Placeholder..
{
   
}


static opcode_t *opcodes[256] = {
    nop,        ld_rr_d16, 
};