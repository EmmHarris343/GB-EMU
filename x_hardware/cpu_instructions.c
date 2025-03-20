#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


typedef void opcode_t(uint8_t *gb_val, uint8_t opcode);         // Placeholder to stop throwing compile error


// NOTICE all the uint8_t *gb_val, in the code. Is a placeholder. -- I have no idea what it was supposed to be..


// NOP - No operation
static void nop(uint8_t *gb, uint8_t opcode)                    // Placeholder..
{

}

// STOP         -- THIS Might be more than just NOP.. Like kill emulator. Or Pause

static void stop(uint8_t opcode) {

}


// HALT!
static void halt(uint8_t opcode) {      // Likely completely HALT / kill the system.

}

// BLANK. THere is just an instruction missing at this location. 
static void blank(uint8_t opcode) {      // Likely completely HALT / kill the system.

}


// Load instructions
static void ld_r8_r8(uint8_t *gb, uint8_t opcode) {     // This is insanely Common instruction. B => C, A=> L, C=>D etc. Which is copied 40 + times in CPU instructions. 

}

// THIS 1 Instruction don't actually use. Cause it needs extra logic. (I think...)
// static void ld_r8_n8(uint8_t *gb, uint8_t opcode) {
    
// }

static void ld_hr_n8(uint8_t opcode) {      // Copy Byte Value into HR (High) 8 byte Register IE: B, D, H

}

static void ld_lr_n8(uint8_t opcode) {      // Copy Byte Value into LR (Low) 8 byte Register IE: C, E, L
    
}


static void ld_r16_r16(uint8_t *gb, uint8_t opcode) {       // Simple Instruction. Copy Data from 16byte Register into 16Byte Register. BC => HL. HL => BC etc
    
}

// Pointed to HL Instructions:
static void ld_p_HL_r8(uint8_t *gb, uint8_t opcode) {       // This might be a little complicated cause it needs to know, which range. HR, or LR (B, D, H) or (C, E, L)
    
}


static void ld_p_HL_n8(uint8_t *gb, uint8_t opcode) {       // Copy data from n8, into where HL is being pointed to
    
}

static void ld_r8_p_HL(uint8_t *gb, uint8_t opcode) {       // Copy data inside HL (that is being pointed to it) Into Register r8 (Needs to know range. HR, or LR (B, D, H) or (C, E, L))
    
}





//// Accumulator related LOAD instructions:

static void ld_p_r16_A(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_p_n16_A(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_A_p_r16(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_A_p_n16(uint8_t *gb, uint8_t opcode) {
    
}

// LDH (A) Load Instructions
static void ldh_p_n16_A(uint8_t *gb, uint8_t opcode) {
    
}

static void ldh_p_C_A(uint8_t *gb, uint8_t opcode) {
    
}


static void ldh_A_p_n16(uint8_t *gb, uint8_t opcode) {
    
}

static void ldh_A_p_C(uint8_t *gb, uint8_t opcode) {
    
}

// LD/ Load (A) with Increment and Decrement Instructions:
static void ld_p_HLI_A(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_p_HLD_A(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_A_p_HLD(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_A_p_HLI(uint8_t *gb, uint8_t opcode) {
    
}



// LD/Load (weird) SP instructions:
static void ld_SP_n16(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_p_n16_sp(uint8_t *gb, uint8_t opcode) {
    
}

static void ld_HL_SP_plus_e8(uint8_t *gb, uint8_t opcode) {     // Load value in SP + (8bit (e) SIGNED int) into HL Register
    
}

static void ld_SP_HL(uint8_t *gb, uint8_t opcode) {
    
}


// Jump instructions
static void jp_hl(uint8_t *gb, uint8_t opcode) {    // Copy Address in HL to PC
    // Bytes 1
    // Cycles 1
    // Flags Changed, none

    // Jump to address in HL; effectively, copy the value in register HL into PC.
    // COPY HL into PC
}

static void jp_n16(uint8_t opcode) {                // Copy Address into PC from n16 value

}

static void jp_cc_n16(uint8_t opcode) {             // Copy address into PC from n16 value.. IF conditions met.

}


static void jr_e8(uint8_t opcode) {
    // NOTICE values like e8, needs to have the bit retreaved, from the memory. THEN CAST! Into an int8_t value (Not uint!) So it can have -128 to +127 memory offset.
    // NOTE: int8_t != uint8_t  THESE ARE VERY DIFFERNT! (Alows for -negative and +positive values)

    int8_t e_address;       // e = signed 8bit register. This is required. Becaues it needs to be able to have Negative or positive Values.

    // Note. This is basically the same as jr_n16 ... but... For clarity I'm spliting up the functions.


    // Cycles 3
    // Bytes 2
    // Flags Changed, none

}

static void jr_cc_e8(uint8_t opcode) {
    // NOTICE values like e8, needs to have the bit retreaved, from the memory. THEN CAST! Into an int8_t value (Not uint!) So it can have -128 to +127 memory offset.
    // NOTE: int8_t != uint8_t  THESE ARE VERY DIFFERNT! (Alows for -negative and +positive values)

    int8_t e_address;       // e = signed 8bit register. This is required. Becaues it needs to be able to have Negative or positive Values.

    // Note. This is basically the same as jr_cc_n16 ... but... For clarity I'm spliting up the functions.

    // Cycles: 3 taken / 2 untaken
    // Bytes: 2
    // Flags Changed, None
}


// JR (Relative Jump) Instructions:                 //  NOTE: Must be within -128 bytes and 127 bytes from current address in PC
static void jr_n16(uint8_t *gb, uint8_t opcode) {   // Relative Jump to 16 byte address (Must be close enough)
    // Cycles 3
    // Bytes 2
    // Flags Changed, none

    // The address is encoded as a signed 8-bit offset from the address immediately following the JR instruction, so the target address n16 must be between -128 and 127 bytes away. For example:
    // Example: 
    // JR Label  ; no-op; encoded offset of 0
    // JR Label  ; infinite loop; encoded offset of -2
}

static void jr_cc_n16(uint8_t *gb, uint8_t opcode) {    // Relative Jump to 16 Byte Address (Must be close enough). As long as CC Conditions met.
    // Relative Jump to address n16 if condition cc is met.
    // Cycles: 3 taken / 2 untaken
    // Bytes: 2
    // Flags Changed, None
}






// Increment / Decrement Registers:
static void inc_hr8(uint8_t opcode) {    // Increment High bit Register  (++ => B, D, H)
    // Flags: Z 0 H -

    // Z = Set if result is 0.
    // N = 0
    // H = Set if overflow from bit 3. 
}

static void inc_lr8(uint8_t opcode) {    // Increment lower bit Register (++ => C, E, L)
    // Flags: Z 0 H -

    // Z = Set if result is 0.
    // N = 0
    // H = Set if overflow from bit 3. 
}

static void dec_hr8(uint8_t opcode) {    // Decrement High bit Register  (-- => B, D, H)
    // Flags: Z 1 H -       The one is the subtraction flag. Saying yes, there was subtraction

    // Z = Set if result is 0.
    // N = 1
    // H = Set if borrow from bit 4.
}

static void dec_lr8(uint8_t opcode) {    // Decrement lower bit Register (-- => C, E, L)
    // Flags: Z 1 H -       The one is the subtraction flag. Saying yes, there was subtraction

    // Z = Set if result is 0.
    // N = 1
    // H = Set if borrow from bit 4.
}

// Inc/ Dec, HL value inside pointer [ ]

static void inc_p_hl(uint8_t opcode) {      // Increment 16 bit HL register, In Pointer ++ HL

    // Z = Set if result is 0.
    // N = 1
    // H = Set if borrow from bit 4.
}

static void dec_p_hl(uint8_t opcode) {      // Decrement 16 bit HL in register, In Pointer -- HL

    // Z = Set if result is 0.
    // N = 1
    // H = Set if borrow from bit 4.
}

// Full 16 bit Registers (BC, DE, HL):

static void inc_r16(uint8_t opcode) {
    
    // FLAGS: NONE AFFECTED
}

static void dec_r16(uint8_t opcode) {
    
    // FLAGS: NONE AFFECTED
}

// INC / Dec SP:
static void inc_sp(uint8_t opcode) {
    
    // FLAGS: NONE AFFECTED
}
static void dec_sp(uint8_t opcode) {

    // FLAGS: NONE AFFECTED
}





// Add/ Subtract Inscructions:
static void add_A_r8(uint8_t opcode) {      // Add value of r8 into A           (NOTICE there is a lot of these instructions, Maybe simplfy it if possible.)
    
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = Set if overflow bit 3
        C = Set if overflow bit 7
    */
}
static void add_A_p_hl(uint8_t opcode) {    // Add value pointed by HL into A

    // FLAGS: SEE add_A_r8
}
static void add_A_n8(uint8_t opcode) {

    // FLAGS: SEE add_A_r8
}
static void add_hl_r16(uint8_t opcode) {

    /*
        FLAGS:
        Z = --
        N = 0
        H = Set if overflow bit 3
        C = Set if overflow bit 7
    */
}
static void add_hl_sp(uint8_t opcode) {

    // FLAGS: SEE add_A_r8
}
static void add_sp_e8(uint8_t opcode) {     // e8 = SIGNED int. So technically same as sp_n16

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
static void adc_A_r8 (uint8_t opcode) {

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = Set if overflow bit 3.
        C = Set if overflow bit 7
    */
}
static void adc_A_p_hl (uint8_t opcode) {   // Subtract the byte pointed to by HL and the carry flag from A.

    // FLAGS: see adc_A_r8
}
static void adc_A_n8 (uint8_t opcode) {     // Subtract the value n8 and the carry flag from A.

    // FLAGS: see adc_A_r8
}




// Subtraction Instructions:
static void sub_A_r8 (uint8_t opcode) {     // Subtract values in a, by 8byte register

    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (i.e. if r8 > A).
    */
}
static void sub_A_p_HL(uint8_t opcode) {

    // FLAGS: See sub_aAr8
}
static void sub_A_n8(uint8_t opcode) {

    // FLAGS: See sub_A_r8
}


// Special SBC (Sub with the cary flag):
static void sbc_A_r8 (uint8_t opcode) {     // Subtract the value in r8 and the carry flag from A.

    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (i.e. if (r8 + carry) > A)
    */
}
static void sbc_A_p_hl (uint8_t opcode) {   // Subtract the byte pointed to by HL and the carry flag from A.

    // FLAGS: see sbc_a_r8    
}
static void sbc_A_n8 (uint8_t opcode) {     // Subtract the value n8 and the carry flag from A.

    // FLAGS: see sbc_a_r8    
}





// TEST: This uses macros to make a function dynmatically.
#define LD_X_Y(x, y) \
static void ld_##x##_##y(uint8_t *gb, uint8_t opcode) \
{ \
    printf("x");\
}
//gb->x = gb->y;



#define LD_X_DHL(x) \
static void ld_##x##_##dhl(uint8_t *gb, uint8_t opcode) \
{ \
    printf("b");\
}
// gb->x = cycle_read(gb, gb->hl); \

#define LD_DHL_Y(y) \
static void ld_##dhl##_##y(uint8_t *gb, uint8_t opcode) \
{ \
    printf("C");\
}
//cycle_write(gb, gb->hl, gb->y); \

/* NOP */   LD_X_Y(b,c) LD_X_Y(b,d) LD_X_Y(b,e) LD_X_Y(b,h) LD_X_Y(b,l) LD_X_DHL(b) LD_X_Y(b,a) LD_X_Y(c,b) /* NOP */   LD_X_Y(c,d) LD_X_Y(c,e) LD_X_Y(c,h) LD_X_Y(c,l) LD_X_DHL(c) LD_X_Y(c,a)
LD_X_Y(d,b) LD_X_Y(d,c) /* NOP */   LD_X_Y(d,e) LD_X_Y(d,h) LD_X_Y(d,l) LD_X_DHL(d) LD_X_Y(d,a) LD_X_Y(e,b) LD_X_Y(e,c) LD_X_Y(e,d) /* NOP */   LD_X_Y(e,h) LD_X_Y(e,l) LD_X_DHL(e) LD_X_Y(e,a)
LD_X_Y(h,b) LD_X_Y(h,c) LD_X_Y(h,d) LD_X_Y(h,e) /* NOP */   LD_X_Y(h,l) LD_X_DHL(h) LD_X_Y(h,a) LD_X_Y(l,b) LD_X_Y(l,c) LD_X_Y(l,d) LD_X_Y(l,e) LD_X_Y(l,h) /* NOP */   LD_X_DHL(l) LD_X_Y(l,a)
LD_DHL_Y(b) LD_DHL_Y(c) LD_DHL_Y(d) LD_DHL_Y(e) LD_DHL_Y(h) LD_DHL_Y(l) /* NOP */   LD_DHL_Y(a) LD_X_Y(a,b) LD_X_Y(a,c) LD_X_Y(a,d) LD_X_Y(a,e) LD_X_Y(a,h) LD_X_Y(a,l) LD_X_DHL(a) /* NOP */






void ld_drr_a(uint8_t passed_opcode) {
    uint8_t register_id;
    register_id = (passed_opcode >> 4) + 1;
    // cycle_write(gb, gb->registers[register_id], gb->af >> 8);

}





// ld_da16_sp pointer 



static void ld_rr_d16(uint8_t *gb, uint8_t opcode)      // Placeholder..
{
   
}

static void ld_da16_sp(uint8_t *gb, uint8_t opcode)     // Placeholder
{
    // I genuinely have no idea what this is supposed to be, pointer a, 16 to _sp register... DA FUKKKKKK???
    // Their on naming confention mentions nothing with an "a" in it...

    // This is sitting at: LD SP, n16 3  12 (@ 0x31)
}

static opcode_t *opcodes[256] = {
    nop,        ld_rr_d16, 
};

// ld_hr_d8 => Load data in n8 into High byte register (B, D, H)
// ld_lr_d8 => Load data in n8 into low byte register (C, E, l)
// So basically, ld_r8_n8



// LD A,[HL-]
// LD A,[HL+]

       
// ld_dhli_a = LD [HL+], A
// ld_dhld_a = LD [HL-], A

// ld_a_dhli = LD A, [HL+]
// ld a_dhld = LD A, [HL-]


/* Operand naming conventions for functions:
   r = 8-bit register
   lr = low 8-bit register
   hr = high 8-bit register
   rr = 16-bit register
   d8 = 8-bit imm   (Immediate?)        (n8)    -- 8-bit integer constant   ... So ROM data / RAM data?
   d16 = 16-bit imm (Immediate?)        (n16)   -- 16-bit integer constant  ... So ROM data / RAM data?
   d.. = [..]       (Pointer? huh)
   cc = condition code (z, nz, c, nc)
*/

static opcode_t *opcodes_test[256] = {
/* 0X */ nop,        ld_rr_d16,  ld_p_r16_A,   inc_r16,     inc_hr8,     dec_hr8,     ld_hr_n8,    rlca,     /* || */ ld_da16_sp, add_hl_r16, ld_A_p_r16,     dec_r16,     inc_lr8,     dec_lr8,     ld_lr_n8,   rrca,
/* 1X */ stop,       ld_rr_d16,  ld_p_r16_A,   inc_r16,     inc_hr8,     dec_hr8,     ld_hr_n8,    rla,      /* || */ jr_e8,      add_hl_r16, ld_A_p_r16,     dec_r16,     inc_lr8,     dec_lr8,     ld_lr_n8,   rra,
/* 2X */ jr_cc_e8,   ld_rr_d16,  ld_p_HLI_A,   inc_r16,     inc_hr8,     dec_hr8,     ld_hr_n8,    daa,      /* || */ jr_cc_e8,   add_hl_r16, ld_A_p_HLI,     dec_r16,     inc_lr8,     dec_lr8,     ld_lr_n8,   cpl,
/* 3X */ jr_cc_e8,   ld_rr_d16,  ld_p_HLI_A,   inc_r16,     inc_p_hl,    dec_p_hl,    ld_p_HL_n8,  scf,      /* || */ jr_cc_e8,   add_hl_r16, ld_A_p_HLD,     dec_r16,     inc_hr8,     dec_hr8,     ld_hr_n8,   ccf,
/* 4X */ nop,        ld_b_c,     ld_b_d,       ld_b_e,      ld_b_h,      ld_b_l,      ld_b_dhl,    ld_b_a,   /* || */ ld_c_b,     nop,        ld_c_d,         ld_c_e,      ld_c_h,      ld_c_l,      ld_c_dhl,   ld_c_a,
/* 5X*/  ld_d_b,     ld_d_c,     nop,          ld_d_e,      ld_d_h,      ld_d_l,      ld_d_dhl,    ld_d_a,   /* || */ ld_e_b,     ld_e_c,     ld_e_d,         nop,         ld_e_h,      ld_e_l,      ld_e_dhl,   ld_e_a,
/* 6X */ ld_h_b,     ld_h_c,     ld_h_d,       ld_h_e,      nop,         ld_h_l,      ld_h_dhl,    ld_h_a,   /* || */ ld_l_b,     ld_l_c,     ld_l_d,         ld_l_e,      ld_l_h,      nop,         ld_l_dhl,   ld_l_a,
/* 7X */ ld_dhl_b,   ld_dhl_c,   ld_dhl_d,     ld_dhl_e,    ld_dhl_h,    ld_dhl_l,    halt,        ld_dhl_a, /* || */ ld_a_b,     ld_a_c,     ld_a_d,         ld_a_e,      ld_a_h,      ld_a_l,      ld_a_dhl,   nop,
/* 8X */ add_A_r8,   add_A_r8,   add_A_r8,     add_A_r8,    add_A_r8,    add_A_r8,    add_A_r8,    add_A_r8, /* || */ adc_A_r8,   adc_A_r8,   adc_A_r8,       adc_A_r8,    adc_A_r8,    adc_A_r8,    adc_A_r8,    adc_A_r8,  
/* 9X */ sub_A_r8,   sub_A_r8,   sub_A_r8,     sub_A_r8,    sub_A_r8,    sub_A_r8,    sub_A_r8,     sub_A_r8, /* || */ sbc_A_r8,   sbc_A_r8,   sbc_A_r8,       sbc_A_r8,    sbc_A_r8,    sbc_A_r8,    sbc_A_r8,    sbc_A_r8,
/* AX */ and_a_r,    and_a_r,    and_a_r,      and_a_r,     and_a_r,     and_a_r,     and_a_r,    and_a_r, /* || */ xor_a_r,    xor_a_r,    xor_a_r,    xor_a_r,    xor_a_r,    xor_a_r,    xor_a_r,    xor_a_r,
/* BX */ or_a_r,     or_a_r,     or_a_r,       or_a_r,      or_a_r,      or_a_r,      or_a_r,     or_a_r, /* || */ cp_a_r,     cp_a_r,     cp_a_r,     cp_a_r,     cp_a_r,     cp_a_r,     cp_a_r,     cp_a_r,
/* CX */ ret_cc,     pop_rr,     jp_cc_a16,    jp_a16,      call_cc_a16, push_rr,     add_A_r8,   rst, /* || */ ret_cc,     ret,        jp_cc_a16,  cb_prefix,  call_cc_a16,call_a16,   adc_A_r8,   rst,
/* DX */ ret_cc,     pop_rr,     jp_cc_a16,    blank,       call_cc_a16, push_rr,     sub_A_r8,   rst, /* || */ ret_cc,     reti,       jp_cc_a16,  blank,        call_cc_a16,  blank,        sbc_A_r8,   rst,
/* EX */ ld_da8_a,   pop_rr,     ld_dc_a,      blank,       blank,       push_rr,     and_a_d8,   rst, /* || */ add_sp_r8,  jp_hl,      ld_da16_a,  blank,    blank,    blank,        xor_a_d8,   rst,
/* FX */ ld_a_da8,   pop_rr,     ld_a_dc,      di,          blank,       push_rr,     or_a_d8,    rst, /* || */ ld_hl_sp_r8,ld_sp_hl,   ld_a_da16,  ei,         blank,        blank,        cp_a_d8,    rst,
};




/// NOTICE values such as jp_a16. or jp_a8. Are specific memory address. a8 often means a specific memory I/0 Space. (So likely High memeory (fast memeory))

// NOTICE values like e8, needs to have the bit retreaved, from the memory. THEN CAST! Into an int8_t value (Not uint!) So it can have -128 to +127 memory offset.