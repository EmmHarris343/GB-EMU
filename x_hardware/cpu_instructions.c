#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cpu_instructions.h"



typedef void opcode_t(uint8_t opcode);






// INTERUPT Instructions:
// HALT!
static void HALT(uint8_t opcode) {      // Likely completely HALT / kill the system.
    printf("HALT Called. Exit Now.. (Guessing)\n");
}
// DI?
static void DI(uint8_t opcode) {

}
// EI? 
static void EI(uint8_t opcode) {

}




// MISC Instructions:
// NOP - No operation
static void NOP(uint8_t opcode) {                    // Placeholder..
    // DO NOTHING
    printf("NOP Called. DO Nothing..\n");
}

// STOP
static void STOP(uint8_t opcode) {      // Unsure, might be like Pause.

}

// BLANK
static void BLANK(uint8_t opcode) {      // Do nothing, basically NOP, but for clarity don't write it like that.
    // DO NOTHING - Not even any command.
}

static void CB_PREFIX(uint8_t opcode) {  // Ummmm Maybe points to Table 2 in the OP_CODE Map
    // Entry point to Table 2 (Prefix) OP_CODES?
    // IE: RLC, RL, RRC, RR, SRA, SRL, SWAP, BIT, RES, SET

}









// Load instructions
static void LD_r8_r8(uint8_t *gb, uint8_t opcode) {     // This is insanely Common instruction. B => C, A=> L, C=>D etc. Which is copied 40 + times in CPU instructions. 

}

// THIS 1 Instruction don't actually use. Cause it needs extra logic. (I think...)
// static void ld_r8_n8(uint8_t *gb, uint8_t opcode) {
    
// }

static void LD_hr_n8(uint8_t opcode) {      // Copy Byte Value into HR (High) 8 byte Register IE: B, D, H

}

static void LD_lr_n8(uint8_t opcode) {      // Copy Byte Value into LR (Low) 8 byte Register IE: C, E, L
    
}


static void LD_r16_r16(uint8_t opcode) {       // Simple Instruction. Copy Data from 16byte Register into 16Byte Register. BC => HL. HL => BC etc
    
}

static void LD_r16_n16(uint8_t opcode) {

}


static void LD_p_r16_n16(uint8_t opcode) {

}

// Pointed to HL Instructions:
static void LD_p_HL_r8(uint8_t opcode) {       // This might be a little complicated cause it needs to know, which range. HR, or LR (B, D, H) or (C, E, L)
    
}


static void LD_p_HL_n8(uint8_t opcode) {       // Copy data from n8, into where HL is being pointed to
    
}

static void LD_r8_p_HL(uint8_t opcode) {       // Copy data inside HL (that is being pointed to it) Into Register r8 (Needs to know range. HR, or LR (B, D, H) or (C, E, L))
    
}





//// Accumulator related LOAD instructions:

static void LD_p_r16_A(uint8_t opcode) {
    
}
static void LD_p_n16_A(uint8_t opcode) {
    
}
static void LD_A_p_r16(uint8_t opcode) {
    
}
static void LD_A_p_n16(uint8_t opcode) {
    
}
// LDH (A) Load Instructions
static void LDH_p_n16_A(uint8_t opcode) {
    
}
static void LDH_p_C_A(uint8_t opcode) {
    
}
static void LDH_A_p_n16(uint8_t opcode) {
    
}
static void LDH_A_p_C(uint8_t opcode) {
    
}
// LD/ Load (A) with Increment and Decrement Instructions:
static void LD_p_HLI_A(uint8_t opcode) {
    
}
static void LD_p_HLD_A(uint8_t opcode) {
    
}
static void LD_A_p_HLD(uint8_t opcode) {
    
}
static void LD_A_p_HLI(uint8_t opcode) {
    
}



// LD/Load (weird) SP instructions:
static void LD_SP_n16(uint8_t opcode) {
    
}
static void LD_p_n16_SP(uint8_t opcode) {
    
}
static void LD_HL_SP_Pe8(uint8_t opcode) {     // Load value in SP + (8bit (e) SIGNED int) into HL Register
    
}
static void LD_SP_HL(uint8_t opcode) {
    
}


// Jump instructions
static void JP_HL(uint8_t opcode) {    // Copy Address in HL to PC
    // Bytes 1
    // Cycles 1
    // Flags Changed, none

    // Jump to address in HL; effectively, copy the value in register HL into PC.
    // COPY HL into PC
}
static void JP_n16(uint8_t opcode) {                // Copy Address into PC from n16 value

}

// JR (Relative Jump) Instructions:                 //  NOTE: Must be within -128 bytes and 127 bytes from current address in PC
static void JP_cc_n16(uint8_t opcode) {             // Copy address into PC from n16 value.. IF conditions met.

}
static void JR_e8(uint8_t opcode) {
    // NOTICE values like e8, needs to have the bit retreaved, from the memory. THEN CAST! Into an int8_t value (Not uint!) So it can have -128 to +127 memory offset.
    // NOTE: int8_t != uint8_t  THESE ARE VERY DIFFERNT! (Alows for -negative and +positive values)

    int8_t e_address;       // e = signed 8bit register. This is required. Becaues it needs to be able to have Negative or positive Values.

    // Note. This is basically the same as jr_n16 ... but... For clarity I'm spliting up the functions.


    // Cycles 3
    // Bytes 2
    // Flags Changed, none

}
static void JR_cc_e8(uint8_t opcode) {
    // NOTICE values like e8, needs to have the bit retreaved, from the memory. THEN CAST! Into an int8_t value (Not uint!) So it can have -128 to +127 memory offset.
    // NOTE: int8_t != uint8_t  THESE ARE VERY DIFFERNT! (Alows for -negative and +positive values)

    int8_t e_address;       // e = signed 8bit register. This is required. Becaues it needs to be able to have Negative or positive Values.

    // Note. This is basically the same as jr_cc_n16 ... but... For clarity I'm spliting up the functions.

    // Cycles: 3 taken / 2 untaken
    // Bytes: 2
    // Flags Changed, None
}
static void JR_n16(uint8_t opcode) {   // Relative Jump to 16 byte address (Must be close enough)
    // Cycles 3
    // Bytes 2
    // Flags Changed, none

    // The address is encoded as a signed 8-bit offset from the address immediately following the JR instruction, so the target address n16 must be between -128 and 127 bytes away. For example:
    // Example: 
    // JR Label  ; no-op; encoded offset of 0
    // JR Label  ; infinite loop; encoded offset of -2
}
static void JR_cc_n16(uint8_t opcode) {    // Relative Jump to 16 Byte Address (Must be close enough). As long as CC Conditions met.
    // Relative Jump to address n16 if condition cc is met.
    // Cycles: 3 taken / 2 untaken
    // Bytes: 2
    // Flags Changed, None
}



// Subroutine Instructions:
static void CALL_n16(uint8_t opcode) {      // Pushes the address of the instruction after the CALL, on the stack. Such that RET can pop it later; Then it executes implicit JP n16

}
static void CALL_cc_n16(uint8_t opcode) {   // Call address n16 if condition cc is met.

}
static void RET(uint8_t opcode) {           // RETurn from subroutine.
    // This is basically a POP PC instruction (If such an instruction existed). See POP r16 for an explanation of how POP works.

    // FLAGS: None affected
}
static void RET_cc(uint8_t opcode) {        // RETurn from subroutine if condition CC is met
    
    // FLAGS: None affected
}
static void RETI(uint8_t opcode) {          // RETurn from subroutine and enable I-nterupts.
    // This is basically equivalent to executing EI then RET, meaning that IME is set right after this instruction.
    
    // FLAGS: None affected
}
static void RST_vec(uint8_t opcode) {       // Call address vec. This is a shorter and faster equivalent to CALL for suitable values of vec.

    // FLAGS: None affected
}











// Increment / Decrement Registers:
static void INC_hr8(uint8_t opcode) {    // Increment High bit Register  (++ => B, D, H)
    // Flags: Z 0 H -

    // Z = Set if result is 0.
    // N = 0
    // H = Set if overflow from bit 3. 
}
static void INC_lr8(uint8_t opcode) {    // Increment lower bit Register (++ => C, E, L)
    // Flags: Z 0 H -

    // Z = Set if result is 0.
    // N = 0
    // H = Set if overflow from bit 3. 
}
static void DEC_hr8(uint8_t opcode) {    // Decrement High bit Register  (-- => B, D, H)
    // Flags: Z 1 H -       The one is the subtraction flag. Saying yes, there was subtraction

    // Z = Set if result is 0.
    // N = 1
    // H = Set if borrow from bit 4.
}
static void DEC_lr8(uint8_t opcode) {    // Decrement lower bit Register (-- => C, E, L)
    // Flags: Z 1 H -       The one is the subtraction flag. Saying yes, there was subtraction

    // Z = Set if result is 0.
    // N = 1
    // H = Set if borrow from bit 4.
}
// Inc/ Dec, HL value inside pointer [ ]
static void INC_p_HL(uint8_t opcode) {      // Increment 16 bit HL register, In Pointer ++ HL

    // Z = Set if result is 0.
    // N = 1
    // H = Set if borrow from bit 4.
}
static void DEC_p_HL(uint8_t opcode) {      // Decrement 16 bit HL in register, In Pointer -- HL

    // Z = Set if result is 0.
    // N = 1
    // H = Set if borrow from bit 4.
}
// Full INC/DEC 16 bit Registers (BC, DE, HL):
static void INC_r16(uint8_t opcode) {

    uint8_t register_id = (opcode >> 4) + 1;        // Shift opcode value RIGHT by 4 places, and add one. 
    /*
        Soooo.... If opcode value is: 
        1101 1100 (Shift it right by 4) ->  == 0000 1101 
        0000 1101 = 0x0D
        0x0D +1 = 0x0E 
        0x0E == 0000 1110
    */
    // cycle_oam_bug(gb, register_id);              // Cycle oam bug? Why.. need to find out later..    
    // gb->registers[register_id]++;                // 




    // FLAGS: NONE AFFECTED
}
static void DEC_r16(uint8_t opcode) {
    uint8_t register_id = (opcode >> 4) + 1;        // This is doing the same as INC_r16. (Why is it shifting it over, to get the actual Register ID?)
    // cycle_oam_bug(gb, register_id);              // Does some junk, to cycle this bug, so it doesn't happen (I assume)
    //gb->registers[register_id]--;                 // The actual DEC, --.. (In the gb->registers[<registers_ids>] )

    // 
    

    // FLAGS: NONE AFFECTED
}
// INC / Dec SP:
static void INC_SP(uint8_t opcode) {
    
    // FLAGS: NONE AFFECTED
}
static void DEC_SP(uint8_t opcode) {

    // FLAGS: NONE AFFECTED
}


// ComPARE Instructios:
static void CP_A_r8(uint8_t opcode) {       // ComPare -> value in pointer HL to A

    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (IE: if r8 > A)
    */
}
static void CP_A_p_HL(uint8_t opcode) {     // ComPare -> value in pointer HL to A

}
static void CP_A_n8(uint8_t opcode) {       // ComPare -> value in n8 to A

}

// Special CPL instruction:
static void CPL(uint8_t opcode) {           // ComPLement accumulator (A = ~A); also called bitwise NOT

}


// DAA (WEIRD INSTRUCTION) -- VERY complicated what it actually does!
static void DAA(uint8_t opcode) {
    
    // DAA => Decimal Adjust Accumulator.

    /*
        FLAGS:
        Z = ???
        N = --
        H = ???
        C = ???
    */
}


// Add/ Subtract Inscructions:
static void ADD_A_r8(uint8_t opcode) {      // Add value of r8 into A           (NOTICE there is a lot of these instructions, Maybe simplfy it if possible.)
    
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = Set if overflow bit 3
        C = Set if overflow bit 7
    */
}
static void ADD_A_p_HL(uint8_t opcode) {    // Add value pointed by HL into A

    // FLAGS: SEE add_A_r8
}
static void ADD_A_n8(uint8_t opcode) {

    // FLAGS: SEE add_A_r8
}
static void ADD_HL_r16(uint8_t opcode) {

    /*
        FLAGS:
        Z = --
        N = 0
        H = Set if overflow bit 3
        C = Set if overflow bit 7
    */
}
static void ADD_HL_SP(uint8_t opcode) {     // Add the value in SP to HL

    // FLAGS: SEE ADD_HL_r16
}
static void ADD_SP_e8(uint8_t opcode) {     // e8 = SIGNED int. So technically same as sp_n16

    // NOTICE values like e8, needs to have the bit retreaved, from the memory. THEN CAST! Into an int8_t value (Not uint!) So it can have -128 to +127 memory offset.

    /*
        FLAGS:
        Z = 0
        N = 0
        H = Set if overflow bit 3
        C = Set if overflow bit 7
    */
}

// Special ADC Add instructions:
static void ADC_A_r8 (uint8_t opcode) {

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = Set if overflow bit 3.
        C = Set if overflow bit 7
    */
}
static void ADC_A_p_HL (uint8_t opcode) {   // Subtract the byte pointed to by HL and the carry flag from A.

    // FLAGS: see adc_A_r8
}
static void ADC_A_n8 (uint8_t opcode) {     // Subtract the value n8 and the carry flag from A.

    // FLAGS: see adc_A_r8
}




// Subtraction Instructions:
static void SUB_A_r8 (uint8_t opcode) {     // Subtract values in a, by 8byte register

    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (i.e. if r8 > A).
    */
}
static void SUB_A_p_HL(uint8_t opcode) {

    // FLAGS: See sub_aAr8
}
static void SUB_A_n8(uint8_t opcode) {

    // FLAGS: See sub_A_r8
}


// Special SBC (Sub with the cary flag):
static void SBC_A_r8 (uint8_t opcode) {     // Subtract the value in r8 and the carry flag from A.

    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (i.e. if (r8 + carry) > A)
    */
}
static void SBC_A_p_HL (uint8_t opcode) {   // Subtract the byte pointed to by HL and the carry flag from A.

    // FLAGS: see sbc_a_r8    
}
static void SBC_A_n8 (uint8_t opcode) {     // Subtract the value n8 and the carry flag from A.

    // FLAGS: see sbc_a_r8    
}



/// STACK Manipulation Instructions:


static void POP_AF(uint8_t opcode) {        // Pop register AF from the stack.
    /*
    This is roughly equivalent to the following imaginary instructions:
        LD F, [SP]  ; See below for individual flags
        INC SP
        LD A, [SP]
        INC SP
    */

    /*
        FLAGS:
        Z = Set from bit 7 of the popped low Byte
        N = Set from bit 6 of the popped low Byte
        H = Set from bit 5 of the popped low Byte
        C = Set from bit 4 of the popped low Byte
    */
}
static void POP_r16(uint8_t opcode) {       // Pop register r16 from the stack.
    /*
    This is roughly equivalent to the following imaginary instructions:
        LD LOW(r16), [SP]   ; C, E or L
        INC SP
        LD HIGH(r16), [SP]  ; B, D or H
        INC SP
    */

    // FLAGS: None affected
}
static void PUSH_AF(uint8_t opcode) {       // Push register AF into the stack. 
    /*
    This is roughly equivalent to the following imaginary instructions:
        DEC SP
        LD [SP], A
        DEC SP
        LD [SP], F.Z << 7 | F.N << 6 | F.H << 5 | F.C << 4    
    */

   // FLAGS: None affected
}
static void PUSH_r16(uint8_t opcode) {      // Push register r16 into the Stack.
    /*
    This is roughly equivalent to the following imaginary instructions:
        DEC SP
        LD [SP], HIGH(r16)  ; B, D or H
        DEC SP
        LD [SP], LOW(r16)   ; C, E or L
    */

    // FLAGS: None affected
}



// Bitwise Logic Instructions:
// And instructions:
static void AND_A_r8(uint8_t opcode) {      // Set A to the bitwise AND between the value in r8 and A

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = 0
    */
}
static void AND_A_p_HL(uint8_t opcode) {    // Set A to the bitwise AND between the byte pointed to by HL and A

    // FLAGS: see: AND A, r8
}
static void AND_A_n8(uint8_t opcode) {      // Set A to the bitwise AND between the value n8 and A

    // FLAGS: see: AND A, r8
}
// OR Instructions:
static void OR_A_r8(uint8_t opcode) {       // Set A to the bitwise OR between the value in r8 and A

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = 0
    */
}
static void OR_A_p_HL(uint8_t opcode) {     // Set A to the bitwise OR between the byte pointed to by HL and A

    // FLAGS: see: OR_A_r8
}
static void OR_A_n8(uint8_t opcode) {       // Set A to the bitwise OR between the value n8 and A

    // FLAGS: see: OR_A_r8   
}
// XOR Instructions:
static void XOR_A_r8(uint8_t opcode) {      // Set A to the bitwise XOR between the value in r8 and A

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = 0
    */
}
static void XOR_A_p_HL(uint8_t opcode) {    // Set A to the bitwise XOR between the byte pointed to by HL and A

    // FLAGS: see: XOR_A_r8
}
static void XOR_A_n8(uint8_t opcode) {      // Set A to the bitwise XOR between the value n8 and A

    // FLAGS: see: XOR_A_r8
}

// Bit Flag instructions:
static void BIT_u3_r8(uint8_t opcode) {     // Test bit u3 in register r8m set the zero flag if bit not set
    /*
        FLAGS:
        Z = Set if the select bit is 0
        N = 0
        H = 1
        C = ---
    */
}
static void BIT_u3_p_HL(uint8_t opcode) {   // Test bit u3 in the byte pointed by HL, set the flag if bit not set
    // FLAGS: See BIT_u3_r8
}
static void RES_u3_r8(uint8_t opcode) {     // Set bit u3 in register r8 to 0. Bit 0 is the rightmost one, bit 7 the leftmost one
    // FLAGS: None affected
}
static void RES_u3_p_HL(uint8_t opcode) {   // Set bit u3 in the byte pointed by HL to 0. Bit 0 is the rightmost one, bit 7 the leftmost one
    // FLAGS: None affected
}
static void SET_u3_r8(uint8_t opcode) {     // Set bit u3 in register r8 to 1. Bit 0 is the rightmost one, bit 7 the leftmost one
    // FLAGS: None affected
}
static void SET_u3_p_HL(uint8_t opcode) {   // Set bit u3 in the byte pointed by HL to 1. Bit 0 is the rightmost one, bit 7 the leftmost one
    // FLAGS: None affected
}

// Weird Bitshift Instructions
static void RL_r8(uint8_t opcode) {         // Rotate bits in register r8 left, through the carry flag
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RL_p_HL(uint8_t opcode) {       // Rotate the byte pointed to by HL left, through the carry flag
    // FLAGS: See RL_r8
}
static void RLA(uint8_t opcode) {           // Rotate register A left, through the carry flag
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RLC_r8(uint8_t opcode) {        // Rotate registers r8 left
    /*
        FLAGS:
        Z = 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RLC_p_HL(uint8_t opcode) {      // Rotate the byte pointed to by HL left
    // FLAGS: See RLC_r8

}
static void RLCA(uint8_t opcode) {          //Rotate Register a left

}
static void RR_r8(uint8_t opcode) {         // Rotate Register r8 right, through the carry flag

    /*
        FLAGS:
        Z = 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RR_p_HL(uint8_t opcode) {       // Rotate the byte pointed to by HL right, through the carry flag

    // FLAGS: see RR_r8
}
static void RRA(uint8_t opcode) {           // Rotate register A <- Right. through the carry flag
    /*
        FLAGS:
        Z = 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RRC_r8(uint8_t opcode) {        // Rotate register r8 <- Right
    //          r8           Flags
    //  -> b7-> ... -> b0 --> [C]
    // ^<--------------<--
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RRC_p_HL(uint8_t opcode) {      // Rotate the byte pointed to by HL <- Right
    // FLAGS: See RRC r8

}
static void RRCA(uint8_t opcode) {          // Rotate register A <- Right

}
static void SLA_r8(uint8_t opcode){         // Shift Left Arithmetically register r8
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void SLA_P_HL(uint8_t opcode) {      //  Shift Left Arithmetically the byte pointed to by HL.
    // Flags: SEE SLA r8
}
static void SRA_r8(uint8_t opcode) {        // Shift Right Arithmetically Register r8 (bit 7, of r8 is unchanged)
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void SRA_p_HL(uint8_t opcode) {      // Shift Right Arithmetically the byte pointed to by HL (Bit 7 of the byte pointed to by HL is unchanged)
    // Flags: See SRA_r8
}
static void SRL_r8(uint8_t opcode) {        // Shift Right Logically register r8
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */    
}
static void SRL_p_HL(uint8_t opcode) {      // Shift Right Logically the byte pointed to by HL
    // FLAGS: See SRL_r8
}
static void SWAP_r8(uint8_t opcode) {       // Swap the upper 4 bits in register r8 and the lower 4 ones
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = 0
    */    
}
static void SWAP_p_HL(uint8_t opcode) {     // Swap the upper 4 bits in the byte pointed by HL and the lower 4 ones.
    // FLAGS: See SWAP_r8

}

// Carry Flag Instructions:
static void CCF(uint8_t opcode) {           // Complement Carry Flag
    /*
        FLAGS:
        Z = --
        N = 0
        H = 0
        C = Inverted
    */     
}
static void SCF(uint8_t opcode) {           // Set Carry Flag
    /*
        FLAGS:
        Z = --
        N = 0
        H = 0
        C = 1
    */
}





/// THE LAST Weird a8 or a16 Instructions:
// Load a16 instructions
static void LD_p_a16_A(uint8_t opcode) {

}
static void LD_A_p_a16(uint8_t opcode) {
    
}
static void LD_p_a16_SP(uint8_t opcode) {

}
// LDH a8 instructions:
static void LDH_A_p_a8(uint8_t opcode) {
    
}
static void LDH_p_a8_A(uint8_t opcode) {
    
}

// Jump a16 instructions:
static void JP_cc_a16(uint8_t opcode) {
    
}
static void JP_a16(uint8_t opcode) {
    
}

// Calls a16:
static void CALL_cc_a16(uint8_t opcode) {

}
static void CALL_a16(uint8_t opcode) {

}





// TEST: This uses macros to make a function dynmatically.
#define LD_X_Y(X, Y) \
static void LD_##X##_##Y(uint8_t opcode) \
{ \
    printf("x");\
}
//gb->x = gb->y;



#define LD_X_DHL(X) \
static void LD_##X##_##DHL(uint8_t opcode) \
{ \
    printf("b");\
}
// gb->x = cycle_read(gb, gb->hl); \

#define LD_DHL_Y(Y) \
static void LD_##DHL##_##Y(uint8_t opcode) \
{ \
    printf("C");\
}
//cycle_write(gb, gb->hl, gb->y); \

/* NOP */   LD_X_Y(B,C) LD_X_Y(B,D) LD_X_Y(B,E) LD_X_Y(B,H) LD_X_Y(B,L) LD_X_DHL(B) LD_X_Y(B,A) LD_X_Y(C,B) /* NOP */   LD_X_Y(C,D) LD_X_Y(C,E) LD_X_Y(C,H) LD_X_Y(C,L) LD_X_DHL(C) LD_X_Y(C,A)
LD_X_Y(D,B) LD_X_Y(D,C) /* NOP */   LD_X_Y(D,E) LD_X_Y(D,H) LD_X_Y(D,L) LD_X_DHL(D) LD_X_Y(D,A) LD_X_Y(E,B) LD_X_Y(E,C) LD_X_Y(E,D) /* NOP */   LD_X_Y(E,H) LD_X_Y(E,L) LD_X_DHL(E) LD_X_Y(E,A)
LD_X_Y(H,B) LD_X_Y(H,C) LD_X_Y(H,D) LD_X_Y(H,E) /* NOP */   LD_X_Y(H,L) LD_X_DHL(H) LD_X_Y(H,A) LD_X_Y(L,B) LD_X_Y(L,C) LD_X_Y(L,D) LD_X_Y(L,E) LD_X_Y(L,H) /* NOP */   LD_X_DHL(L) LD_X_Y(L,A)
LD_DHL_Y(B) LD_DHL_Y(C) LD_DHL_Y(D) LD_DHL_Y(E) LD_DHL_Y(H) LD_DHL_Y(L) /* NOP */   LD_DHL_Y(A) LD_X_Y(A,B) LD_X_Y(A,C) LD_X_Y(A,D) LD_X_Y(A,E) LD_X_Y(A,H) LD_X_Y(A,L) LD_X_DHL(A) /* NOP */









static opcode_t *opcodes[256] = {
/*  ---> X0, X1, X2, X3, X4 ... XB .. XF etc */
/* 0X */ NOP,        LD_p_r16_n16,  LD_p_r16_A, INC_r16,  INC_hr8,     DEC_hr8,  LD_hr_n8,   RLCA,     /* || */ LD_p_a16_SP,  ADD_HL_r16, LD_A_p_r16,  DEC_r16,   INC_lr8,     DEC_lr8,   LD_lr_n8,   RRCA,
/* 1X */ STOP,       LD_p_r16_n16,  LD_p_r16_A, INC_r16,  INC_hr8,     DEC_hr8,  LD_hr_n8,   RLA,      /* || */ JR_e8,        ADD_HL_r16, LD_A_p_r16,  DEC_r16,   INC_lr8,     DEC_lr8,   LD_lr_n8,   RRA,
/* 2X */ JR_cc_e8,   LD_p_r16_n16,  LD_p_HLI_A, INC_r16,  INC_hr8,     DEC_hr8,  LD_hr_n8,   DAA,      /* || */ JR_cc_e8,     ADD_HL_r16, LD_A_p_HLI,  DEC_r16,   INC_lr8,     DEC_lr8,   LD_lr_n8,   CPL,
/* 3X */ JR_cc_e8,   LD_p_r16_n16,  LD_p_HLI_A, INC_r16,  INC_p_HL,    DEC_p_HL, LD_p_HL_n8, SCF,      /* || */ JR_cc_e8,     ADD_HL_r16, LD_A_p_HLD,  DEC_r16,   INC_hr8,     DEC_hr8,   LD_hr_n8,   CCF,
/* 4X */ NOP,        LD_B_C,        LD_B_D,     LD_B_E,   LD_B_H,      LD_B_L,   LD_B_DHL,   LD_B_A,   /* || */ LD_C_B,       NOP,        LD_C_E,      LD_C_E,    LD_C_H,      LD_C_L,    LD_C_DHL,   LD_C_A,
/* 5X*/  LD_D_B,     LD_D_C,        NOP,        LD_D_E,   LD_D_H,      LD_D_L,   LD_D_DHL,   LD_D_A,   /* || */ LD_E_B,       LD_E_C,     LD_E_D,      NOP,       LD_E_H,      LD_E_L,    LD_E_DHL,   LD_E_A,
/* 6X */ LD_H_B,     LD_H_C,        LD_H_D,     LD_H_E,   NOP,         LD_H_L,   LD_H_DHL,   LD_H_A,   /* || */ LD_L_B,       LD_L_C,     LD_L_D,      LD_L_E,    LD_L_H,      NOP,       LD_L_DHL,   LD_L_A,
/* 7X */ LD_DHL_B,   LD_DHL_C,      LD_DHL_D,   LD_DHL_E, LD_DHL_H,    LD_DHL_L, HALT,       LD_DHL_A, /* || */ LD_A_B,       LD_A_C,     LD_A_D,      LD_A_E,    LD_A_H,      LD_A_L,    LD_A_DHL,   NOP,
/* 8X */ ADD_A_r8,   ADD_A_r8,      ADD_A_r8,   ADD_A_r8, ADD_A_r8,    ADD_A_r8, ADD_A_r8,   ADD_A_r8, /* || */ ADC_A_r8,     ADC_A_r8,   ADC_A_r8,    ADC_A_r8,  ADC_A_r8,    ADC_A_r8,  ADC_A_r8,   ADC_A_r8,  
/* 9X */ SUB_A_r8,   SUB_A_r8,      SUB_A_r8,   SUB_A_r8, SUB_A_r8,    SUB_A_r8, SUB_A_r8,   SUB_A_r8, /* || */ SBC_A_r8,     SBC_A_r8,   SBC_A_r8,    SBC_A_r8,  SBC_A_r8,    SBC_A_r8,  SBC_A_r8,   SBC_A_r8,
/* AX */ AND_A_r8,   AND_A_r8,      AND_A_r8,   AND_A_r8, AND_A_r8,    AND_A_r8, AND_A_r8,   AND_A_r8, /* || */ XOR_A_r8,     XOR_A_r8,   XOR_A_r8,    XOR_A_r8,  XOR_A_r8,    XOR_A_r8,  XOR_A_r8,   XOR_A_r8,
/* BX */ OR_A_r8,    OR_A_r8,       OR_A_r8,    OR_A_r8,  OR_A_r8,     OR_A_r8,  OR_A_r8,    OR_A_r8,  /* || */ CP_A_r8,      CP_A_r8,    CP_A_r8,     CP_A_r8,   CP_A_r8,     CP_A_r8,   CP_A_r8,    CP_A_r8,
/* CX */ RET_cc,     POP_r16,       JP_cc_a16,  JP_a16,   CALL_cc_a16, PUSH_r16, ADD_A_r8,   RST_vec,  /* || */ RET_cc,       RET,        JP_cc_a16,   CB_PREFIX, CALL_cc_a16, CALL_a16,  ADC_A_r8,   RST_vec,
/* DX */ RET_cc,     POP_r16,       JP_cc_a16,  BLANK,    CALL_cc_a16, PUSH_r16, SUB_A_r8,   RST_vec,  /* || */ RET_cc,       RETI,       JP_cc_a16,   BLANK,     CALL_cc_a16, BLANK,     SBC_A_r8,   RST_vec,
/* EX */ LDH_p_a8_A,  POP_r16,      LDH_p_C_A,  BLANK,   BLANK,        PUSH_r16, AND_A_r8,   RST_vec,  /* || */ ADD_SP_e8,    JP_HL,      LD_p_a16_A,  BLANK,     BLANK,       BLANK,     XOR_A_r8,   RST_vec,
/* FX */ LDH_A_p_a8,  POP_r16,      LDH_A_p_C,  DI,      BLANK,        PUSH_r16, OR_A_r8,    RST_vec,  /* || */ LD_HL_SP_Pe8,  LD_SP_HL,  LD_A_p_a16,  EI,        BLANK,       BLANK,     CP_A_r8,    RST_vec,
};



void calc_op_num(uint8_t num) {
    // OP CODE -> to Decimal to work with Array.

}



int main() {
    // Random Entry point for testing sections of code..
    printf("CPU Instructions... Needs much more stuff...\n");



    uint8_t store_code = 0xB1;

    opcodes[store_code](store_code);

    opcodes[store_code];
}
