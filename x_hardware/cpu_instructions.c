#include "cpu.h"
#include <sys/types.h>
//#include <numeric>
#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
// #include <stdlib.h>
#include <stdint.h>

#include "cpu_instructions.h"



/// TODO: Make Flags easier to Use and more readable. /// BAD: set_flag(2), clear_flag(1)
/*

cpu->F.Z = 0;
cpu->F.N = 0;
cpu->F.H = 0;
cpu->F.C = 0;

*/


#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"




/*
    printf("%sred\n", KRED);
    printf("%sgreen\n", KGRN);
    printf("%syellow\n", KYEL);
    printf("%sblue\n", KBLU);
    printf("%smagenta\n", KMAG);
    printf("%scyan\n", KCYN);
    printf("%swhite\n", KWHT);
    printf("%snormal\n", KNRM);

    ("%scyan\n", KCYN);
    ("%snormal\n", KNRM);
*/




/// TODO: Make the CPU Flags easier to read.
/*
IE: 
#define FLAG_Z 0x80 OR ... 0, 1, 2, 3 etc 
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

then I can do: 
set_flag(FLAG_Z);
clear_flag(FLAG_Z);

Instead of:
set_flag(0)... as I have no idea what the number refers to.. So it makes it not very readable..
*/


/// CONSIDER: Using a ADDITION_8bit Set flags... Might make things easier... 
void set_add_flags_8bit(uint8_t a, uint8_t b, int affect_Z, int affect_N) {
    // Clear the bits we're modifying
    if (affect_Z) clear_flag(0);  // Z always cleared on LD HL, SP+e8
    if (affect_N) clear_flag(1);  // N always cleared on addition

    // Half carry: bit 3 overflow into bit 4
    if (((a & 0xF) + (b & 0xF)) > 0xF)
        set_flag(2);  // H
    else
        clear_flag(2);

    // Full carry: bit 7 overflow into bit 8
    if ((uint16_t)a + (uint16_t)b > 0xFF)
        set_flag(3);  // C
    else
        clear_flag(3);
}



typedef void opcode_t(CPU *cpu, instruction_T instrc);

extern CPU_STATE cpu_status;


// INTERUPT Instructions:
// HALT!
static void HALT(CPU *cpu, instruction_T instrc) {      // Likely completely HALT / kill the system.
    printf("HALT Called. Exit Now.. (Guessing)\n");
    cpu_status.halt = 1;
}
// DI?
static void DI(CPU *cpu, instruction_T instrc) {

}
// EI? 
static void EI(CPU *cpu, instruction_T instrc) {

}




// MISC Instructions:
// NOP - No operation
static void NOP(CPU *cpu, instruction_T instrc) {                    // Placeholder..
    // DO NOTHING
    printf("NOP Called. Just Advance PC\n");
    cpu->PC ++; // Do nothing, just advance the PC
}
// STOP
static void STOP(CPU *cpu, instruction_T instrc) {      // Unsure, might be like Pause.
    printf("STOP Called, not setup HALT\n");
    cpu_status.halt = 1;
    

}
// DAA (WEIRD INSTRUCTION) -- VERY complicated what it actually does!
static void DAA(CPU *cpu, instruction_T instrc) {
    
    // DAA => Decimal Adjust Accumulator.
    printf("DAA. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
}
// BLANK
static void BLANK(CPU *cpu, instruction_T instrc) {      // Do nothing, basically NOP, but for clarity don't write it like that.
    // DO NOTHING - Not even any command.
    // This shouldn't Even be called.
    printf("%sBLANK Called, This should never be called. Halting%s\n", KRED, KNRM);
    cpu_status.halt = 1;
}

// Carry Flag Instructions:
static void CCF(CPU *cpu, instruction_T instrc) {           // Complement Carry Flag
    printf("CCF. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    /*
        FLAGS:
        Z = --
        N = 0
        H = 0
        C = Inverted
    */     
}
static void SCF(CPU *cpu, instruction_T instrc) {           // Set Carry Flag
    printf("SCF. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    /*
        FLAGS:
        Z = --
        N = 0
        H = 0
        C = 1
    */
}



// This is the PREFIXED Instructions... Or rather the entry point to them.
// Haven't setup yet. But, take the sub opcode or something?
// Then point to the --> Specific PREFIXED Instruction?
static void CB_PREFIX(CPU *cpu, instruction_T instrc) {  // Ummmm Maybe points to Table 2 in the OP_CODE Map

    printf("CB Prefix call.... blehhhhh was hoping this would be later... bleh\nI don't know how this works or data works. \n");
    // Entry point to Table 2 (Prefix) OP_CODES?

    // Should be only 1byte code. so.. umm huh..
    


    // IE: RLC, RL, RRC, RR, SRA, SRL, SWAP, BIT, RES, SET

    printf("CB Prefix Called, not setup HALT\n");
    cpu_status.halt = 1;    
}



// -----------------------------------------------
//
///                    NOTICE:
// ==---- Starting main OP Code instructions ----==
//
// -----------------------------------------------


// -----------------------------------------------
/// SECTION:
// LD High Register, LD Low Register instructions

static void LD_hr_n8(CPU *cpu, instruction_T instrc) {      // Copy Byte Value into HR (High) 8 byte Register IE: B, D, H
    printf("LD HR, n8.                 ; EXP: HR <- n8    ..    Reg.H <- n8\n");
    switch (instrc.opcode) {
        case 0x06:
            cpu->B = instrc.operand1;
            break;
        case 0x16:
            cpu->D = instrc.operand1;
            break;
        case 0x26:
            cpu->H = instrc.operand1;
            break;
    }
    cpu->PC += 2;
    // No flags affected.
}

static void LD_lr_n8(CPU *cpu, instruction_T instrc) {      // Copy Byte Value into LR (Low) 8 byte Register IE: C, E, L
    printf("LD LR, n8.                ; EXP: LR <- n8       .. Reg.C <- n8\n");
    switch (instrc.opcode) {
        case 0x0E:
            cpu->C = instrc.operand1;
            break;
        case 0x1E:
            cpu->E = instrc.operand1;
            break;
        case 0x2E:
            cpu->L = instrc.operand1;
            break;
    }
    cpu->PC += 2;
    // No flags affected
}

static void LD_r16_n16(CPU *cpu, instruction_T instrc) {
    printf("LD r16 n16. Copy n16 value into r16 Register\n");
    //printf("Values? OP1: %02X OP2: %02X\n", instrc.operand1, instrc.operand2);
    uint16_t load_n16;
    load_n16 = cnvrt_lil_endian(instrc.operand1, instrc.operand2);

    switch (instrc.opcode) {
        case 0x01:        
            // BC
            cpu->BC = load_n16;
            break;
        case 0x11:
            // DE
            cpu->DE = load_n16;
            break;
        case 0x21:
            // HL
            cpu->HL = load_n16;
            break;
        case 0x31:
            // SP
            cpu->SP = load_n16;
            break;
    }

    cpu->PC += 3;

    // Bytes: 3
    // Flags: None Affected
}

static void LD_p_r16_n16(CPU *cpu, instruction_T instrc) {
    printf("Load 16bit data, into pointer in 16bit Register.\n");
    printf("Values? OP1: %02X OP2: %02X", instrc.operand1, instrc.operand2);
    uint16_t load_n16;
    load_n16 = cnvrt_lil_endian(instrc.operand1, instrc.operand2);

    
}

// Pointed to HL Instructions:
static void LD_p_HL_r8(CPU *cpu, instruction_T instrc) {       // This might be a little complicated cause it needs to know, which range. HR, or LR (B, D, H) or (C, E, L)
    printf("LD [HL] r8. Called, not setup HALT\n");
    cpu_status.halt = 1;        
}


static void LD_p_HL_n8(CPU *cpu, instruction_T instrc) {       // Copy data from n8, into where HL is being pointed to
    printf("LD [HL] n8. Called, not setup HALT\n");
    cpu_status.halt = 1;            

    // Bytes = 2
    // No flags Affected
}

static void LD_r8_p_HL(CPU *cpu, instruction_T instrc) {       // Copy data inside HL (that is being pointed to it) Into Register r8 (Needs to know range. HR, or LR (B, D, H) or (C, E, L))
    printf("LD r8 [HL]. Called, not setup HALT\n");
    cpu_status.halt = 1;        
}




// -----------------------------------------------
/// SECTION:
// LD Acculator (A Register) instructions

static void LD_p_r16_A(CPU *cpu, instruction_T instrc) {
    printf("LD [r16] A.               ; EXP: [DE] <- A \n");
    switch (instrc.opcode) {
        case 0x02:
            // LD [BC], A
            external_write(cpu->BC, cpu->A);
        case 0x12:
            // LD [DE], A
            external_write(cpu->DE, cpu->A);
    }
    cpu->PC++;
}
static void LD_p_n16_A(CPU *cpu, instruction_T instrc) {
    printf("LD [n16], A. Called, not setup HALT\n");
    cpu_status.halt = 1;
}
static void LD_A_p_r16(CPU *cpu, instruction_T instrc) {
    printf("LD A, [r16]. Called, not setup HALT\n");
    cpu_status.halt = 1;
}
static void LD_A_p_n16(CPU *cpu, instruction_T instrc) {
    printf("LD A, [n16]. Called, not setup HALT\n");
    cpu_status.halt = 1;
}

// LDH (A) Load Instructions        -- THESE I MIGHT HAVE TURN INTO MACRO!
static void LDH_p_n16_A(CPU *cpu, instruction_T instrc) {
    printf("LDH [n16], A. Called, not setup HALT\n");
    cpu_status.halt = 1;
}
static void LDH_p_C_A(CPU *cpu, instruction_T instrc) {
    printf("LDH [C], A. Called, not setup HALT\n");
    cpu_status.halt = 1;
}
static void LDH_A_p_n16(CPU *cpu, instruction_T instrc) {
    printf("LDH A, [n16]. Called, not setup HALT\n");
    cpu_status.halt = 1;
}
static void LDH_A_p_C(CPU *cpu, instruction_T instrc) {
    printf("LDH A, [C]. Called, not setup HALT\n");
    cpu_status.halt = 1;
}

// LD/ Load (A) with Increment and Decrement to HL after.
static void LD_p_HLI_A(CPU *cpu, instruction_T instrc) {
    printf("LD [HLI] A, Copy value in A, into the value pointed by HL, then Increment HL\n");

    uint8_t a_val = cpu->A;
    external_write(cpu->HL, a_val);

    // Increment HL & the PC By one.
    cpu->HL ++;
    cpu->PC ++;
}
static void LD_p_HLD_A(CPU *cpu, instruction_T instrc) {
    printf("LD [HLI] A, Copy value in A, into the value pointed by HL, then Decement HL\n");
 
    uint8_t a_val = cpu->A;
    external_write(cpu->HL, a_val); 

    cpu->HL --;
    cpu->PC --;
}
static void LD_A_p_HLI(CPU *cpu, instruction_T instrc) {
    printf("LD A, [HLI].               ; A <- [HL] (Then HL++)\n");
    // LD A, [HL+], Copy value pointed from HL, into A register, then Increment HL
    uint8_t hl_val = external_read(cpu->HL);
    cpu->A = hl_val;

    printf("--- Diag, what is value pointed by HL? 0x%02X\n", hl_val);

    cpu->HL ++;
    cpu->PC ++;
    // Bytes = 1
    // Flags not affected
}
static void LD_A_p_HLD(CPU *cpu, instruction_T instrc) {
    printf("LD A, [HLD] (LD A, [HL-]), Copy value pointed from HL, into A register, then Decrement HL\n");

    uint8_t hl_val = external_read(cpu->HL);
    cpu->A = hl_val;
    printf("  --- Diag, what is value pointed by HL? 0x%02X\n", hl_val);

    cpu->HL --;
    cpu->PC ++;

    // Bytes = 1
    // Flags not affected
}
// Load Register A & a16 instructions
static void LD_p_a16_A(CPU *cpu, instruction_T instrc) {
    printf("LD [a16] A, Called => Copy 8bit A Register into 8bit value located in [a16]\n");
    
    uint16_t a16 = cnvrt_lil_endian(instrc.operand1, instrc.operand2);
    
    printf(":LD: a16 Location val %04X\n", a16);
    external_write(a16, cpu->A);
    
    cpu->PC +=3;

    // Bytes = 3
    // No flags affected
}
static void LD_A_p_a16(CPU *cpu, instruction_T instrc) {
    printf("LD A [a16], Copy 8bit value in [a16] into Register A\n");
    // Copy the value in RAM pointed to by a16. Into Register A
    
    uint16_t a16 = cnvrt_lil_endian(instrc.operand1, instrc.operand2);

    cpu->A = external_read(a16);

    cpu->PC += 3;
    // Bytes = 3
    // No flags affected
}

// LDH a8 instructions:
static void LDH_A_p_a8(CPU *cpu, instruction_T instrc) {
    printf("LDH A [a8] Called => Copy value in (0xFF00 + a8) into A register\n");
    // This one is wonky. Takes an [a8] value, converts it to 16bit and is Zero Extended
    // 0x21 => 0xFF21, This area is often HRAM, I/O. IE Register
    // [a8] brackets mean: loading value of a8, so 0xFF00 + a8

    uint8_t a8 = instrc.operand1;
    uint16_t combined_addr = 0xFF00 + a8;

    printf(":LDH: Combined val %04X\n", combined_addr);
    uint8_t load_h_range = external_read(combined_addr);

    cpu->A = load_h_range;

    cpu->PC +=2;
}
static void LDH_p_a8_A(CPU *cpu, instruction_T instrc) {
    printf("LDH [a8] A, Called => Copy A Register value into 8bit value located at (0xFF00 + a8) \n");
    
    uint8_t a8 = instrc.operand1;
    uint16_t combined_addr = 0xFF00 + a8;

    printf(":LDH: Combined val %04X\n", combined_addr);
    external_write(combined_addr, cpu->A);

    cpu->PC +=2;    
}


/// SECTION:
// LD Stack manipulation Instructions:
static void LD_SP_n16(CPU *cpu, instruction_T instrc) {
    printf("LD_SP_n16 Overwrite the stack pointer, with value in n16.\n");

    // Overwrites the stack pointer with the value in the operands.
    cpu->SP = cnvrt_lil_endian(instrc.operand1, instrc.operand2);
    cpu->PC += 3;

    // Bytes = 3
    // No Flags affected
    
}
static void LD_p_a16_SP(CPU *cpu, instruction_T instrc) {
    printf("LD [n16] SP, Write SP_Low & SP_High Bytes, into the [a16] and [a16+1] memory locations\n");

    uint16_t n16_addr = cnvrt_lil_endian(instrc.operand1, instrc.operand2);
    uint8_t SP_LOW = cpu->SP & 0xFF;
    uint8_t SP_HIGH = (cpu->SP >> 8) & 0xFF;

    external_write(n16_addr, SP_LOW);
    external_write(n16_addr + 1, SP_HIGH);

    cpu->PC ++;
    
}
static void LD_HL_SP_Pe8(CPU *cpu, instruction_T instrc) {     // Load value in SP + (8bit (e) SIGNED int) into HL Register
    printf("LD HL, (SP + e8). Overwrite the address (SP + e8) into the HL Register\n");

    // Add the signed value e8 to SP and copy the result in HL.

    // OVERWRITES the HL Register.

    int8_t e_signed_offset = (int8_t)instrc.operand1;       // NOTICE int8_t = signed, because e = signed 8bit register. Because it's relative a: +- 
    cpu->HL = (cpu->SP + e_signed_offset);                  // SET HL to the Calulated +- n16 value.

    uint8_t SP_LOW = cpu->SP & 0xFF;                        // This is first 8Bits. (Instead of full 16).
    uint8_t e8_flag_calc = (uint8_t)e_signed_offset;        // For easy Calculations, change back into uint8_t (Unsigned)

    // 
    clear_flag(0);
    clear_flag(1);

    if (((SP_LOW & 0xF) + (e8_flag_calc & 0xF)) > 0xF)       // IF combined the (first 4 bits) of each is GREATER than 4 bits.
        set_flag(2);    // Set H Flag
    else
        clear_flag(2);  // Clear H Flag

    if ((SP_LOW + e8_flag_calc) > 0xFF)                      // If combined, the values are greater than 8 Bits.
        set_flag(3);    // Set C Flag
    else
        clear_flag(3);  // Clear C Flag

    cpu->PC += 2;

    // Bytes = 2
    /*
    Flags:
    Z = 0
    N = 0
    H = Set if overflow from bit 3.
    C = Set if overflow from bit 7.
    */
}
static void LD_SP_HL(CPU *cpu, instruction_T instrc) {
    printf("LD_SP_HL.                   ; Copy register HL into register SP\n");

    cpu->SP = cpu->HL;  // 
    cpu->PC ++;

    // Bytes = 1
    // No flags effected
}



/// SECTION:
// MACRO'D LD instructions. Covers LD, r8, r8. LD X, [HL]. LD [HL], B

// This is LD from Register --> Register
#define LD_X_Y(X, Y) \
static void LD_##X##_##Y(CPU *cpu, instruction_T instrc) \
{ \
    printf("LD r8 to r8\n");\
    cpu->X = cpu-> Y;\
    cpu->PC += 1;\
}

// LD B, [HL]
#define LD_X_DHL(X) \
static void LD_##X##_##DHL(CPU *cpu, instruction_T instrc) \
{ \
    printf("LD r8, [HL]\n");\
    cpu->X = external_read(cpu->HL);\
    cpu->PC += 1;\
}

// LD [HL], B
#define LD_DHL_Y(Y) \
static void LD_##DHL##_##Y(CPU *cpu, instruction_T instrc) \
{ \
    printf("LD [HL], r8\n");\
    external_write(cpu->HL, cpu->Y);\
    cpu->PC += 1;\
}

/* NOP */   LD_X_Y(B,C) LD_X_Y(B,D) LD_X_Y(B,E) LD_X_Y(B,H) LD_X_Y(B,L) LD_X_DHL(B) LD_X_Y(B,A) LD_X_Y(C,B) /* NOP */   LD_X_Y(C,D) LD_X_Y(C,E) LD_X_Y(C,H) LD_X_Y(C,L) LD_X_DHL(C) LD_X_Y(C,A)
LD_X_Y(D,B) LD_X_Y(D,C) /* NOP */   LD_X_Y(D,E) LD_X_Y(D,H) LD_X_Y(D,L) LD_X_DHL(D) LD_X_Y(D,A) LD_X_Y(E,B) LD_X_Y(E,C) LD_X_Y(E,D) /* NOP */   LD_X_Y(E,H) LD_X_Y(E,L) LD_X_DHL(E) LD_X_Y(E,A)
LD_X_Y(H,B) LD_X_Y(H,C) LD_X_Y(H,D) LD_X_Y(H,E) /* NOP */   LD_X_Y(H,L) LD_X_DHL(H) LD_X_Y(H,A) LD_X_Y(L,B) LD_X_Y(L,C) LD_X_Y(L,D) LD_X_Y(L,E) LD_X_Y(L,H) /* NOP */   LD_X_DHL(L) LD_X_Y(L,A)
LD_DHL_Y(B) LD_DHL_Y(C) LD_DHL_Y(D) LD_DHL_Y(E) LD_DHL_Y(H) LD_DHL_Y(L) /* NOP */   LD_DHL_Y(A) LD_X_Y(A,B) LD_X_Y(A,C) LD_X_Y(A,D) LD_X_Y(A,E) LD_X_Y(A,H) LD_X_Y(A,L) LD_X_DHL(A) /* NOP */


// -----------------------------------------------
/// SECTION:
// JUMPs and Relative Jumps (JRs)

// Jump instructions
static void JP_HL(CPU *cpu, instruction_T instrc) {    // Copy Address in HL to PC
    printf("JP HL.               ; Reg.PC = Reg.HL\n");

    cpu->PC = cpu->HL;
    // Copy HL into PC.
    // Bytes: 1
    // No Flags changed.
}

/// NOTE: Technically n16 == a16 in this case. Clarity of the OP Table. Naming these only a16, a8 instructions.

// Jump a16 instructions:
static void JP_a16(CPU *cpu, instruction_T instrc) {
    printf("JP a16.               ; Reg.PC = a16 OR n16\n");
    cpu->PC = cnvrt_lil_endian(instrc.operand1, instrc.operand2);
}
static void JP_cc_a16(CPU *cpu, instruction_T instrc) {    
    int proceed = 0;

    switch(instrc.opcode) {
        case 0xC2:
            if (!(cpu->F & FLAG_Z)) proceed = 1;
            break;
        case 0xD2:
            if (!(cpu->F & FLAG_C)) proceed = 1;
            break;
        case 0xCA:
            if ((cpu->F & FLAG_Z)) proceed = 1;
            break;
        case 0xDA:
            if ((cpu->F & FLAG_C)) proceed = 1;
            break;
    }
    if (proceed) cpu->PC = cnvrt_lil_endian(instrc.operand1, instrc.operand2);

    // 3 Bytes
    // No flags Changed
}

// Relative Jumps: e8
static void JR_e8(CPU *cpu, instruction_T instrc) {

    int8_t e_signed_offset;       // e = signed 8bit register. Because it's relative to the PC location +- a value.
    e_signed_offset = (int8_t)instrc.operand1;
    cpu->PC += e_signed_offset;
 
    printf("Relative jump to: +- %02X\n", e_signed_offset);
    

    // Cycles 3
    // Bytes 2
    // Flags Changed, none
}
static void JR_cc_e8(CPU *cpu, instruction_T instrc) {
    printf("JR cc e8, Relative Jump, Conditional\n");

    int8_t e_signed_offset;       // e = signed 8bit register. Because it's relative to the PC location +- a value.
    e_signed_offset = (int8_t)instrc.operand1;

    uint16_t jr_addr_test = (cpu->PC + e_signed_offset);

    switch (instrc.opcode) {
        case 0x20:                  // JR, NZ e8
            // (cpu->F & FLAG_Z) ? /* NOP */  : /* Jump */ 
            if (!(cpu->F & FLAG_Z)) { 
                printf("%sJR NZ e8 Condition Met -> Relative jump %02X %s\n", KCYN, instrc.operand1, KNRM);
                cpu->PC += e_signed_offset;
            }
            else cpu->PC +=2;
            break;
        case 0x28:
            if (cpu->F & FLAG_Z) {
                printf("JR Z e8 Condition Met -> Relative jump %02X\n", instrc.operand1);
                cpu->PC += e_signed_offset;
            }
            else cpu->PC +=2;
            break;
        case 0x30:
            if (!(cpu->F & FLAG_C)) { 
                printf("JR NC e8 Condition Met -> Relative jump %02X\n", instrc.operand1);
                cpu->PC += e_signed_offset;
            }
            else cpu->PC +=2;
            break;
        case 0x38:
            if (cpu->F & FLAG_C) { 
                printf("JR C e8 Condition Met -> Relative jump %02X\n", instrc.operand1);
                cpu->PC += e_signed_offset;
            }
            else cpu->PC +=2;
            break;
        default:
            printf("ERROR, JR CC did not match ANY OPCODES.. should abort.\n");
            cpu->PC +=2;
    }

    // Cycles: 3 taken / 2 untaken
    // Bytes: 2
    // Flags Changed, None
}



// -----------------------------------------------
/// SECTION:
// ADD ADC, SUB, SBC, INC, DEC, CP
// 8-Bit arithmetic instructions:

// ADD/ ADC Instructions:
static void ADD_A_r8(CPU *cpu, instruction_T instrc) {      // Add value of r8 into A
    printf("ADD A, r8. Called.          ; Add value in r8 into Register A\n");

    // Table calculates WHICH register is called, based on the OP code provided.
    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, NULL, &cpu->A };
    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.

    
    uint16_t add_result = (cpu->A + *reg_table[op_index]);
    uint8_t final_8bit = (uint8_t)add_result;

    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag    
    ((cpu->A & 0x0F) + (op_r8 & 0x0F) > 0x0F) ? set_flag(2) : clear_flag(2); // H Flag
    (add_result > 0xFF) ? set_flag(3) : clear_flag(3); // C Flag
    clear_flag(1);  // N Flag (Subtraction)

    cpu->A = final_8bit;
    cpu->PC += 1;


    // Bytes = 2

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = Set if overflow bit 3
        C = Set if overflow bit 7
    */
}
static void ADD_A_p_HL(CPU *cpu, instruction_T instrc) {    // Add value pointed by HL into A
    printf("ADD A, [HL]. Called.            ; Add Value inside [HL] into Register A\n");
    uint8_t hl_val = external_read(cpu->HL);
    uint16_t add_result = (cpu->A + hl_val);
    uint8_t final_8bit = (uint8_t)add_result;

    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag
    ((cpu->A & 0x0F) + (hl_val & 0x0F) > 0x0F) ? set_flag(2) : clear_flag(2); // H Flag
    (add_result > 0xFF) ? set_flag(3) : clear_flag(3); // C Flag
    clear_flag(1);  // N Flag (Subtraction)

    cpu->A = final_8bit;
    cpu->PC += 1;

    // Bytes = 1
}
static void ADD_A_n8(CPU *cpu, instruction_T instrc) {
    printf("ADD A, n8. Called.              ; Add immediate value to Register A\n");
    uint8_t n8_val = instrc.operand1;

    uint16_t add_result = (cpu->A + n8_val);
    uint8_t final_8bit = (uint8_t)add_result;

    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag    
    ((cpu->A & 0x0F) + (n8_val & 0x0F) > 0x0F) ? set_flag(2) : clear_flag(2); // H Flag
    (add_result > 0xFF) ? set_flag(3) : clear_flag(3); // C Flag
    clear_flag(1);  // N Flag (Subtraction)

    cpu->A = final_8bit;
    cpu->PC += 2;

    // Bytes = 2
}
// ADC Add instructions:
static void ADC_A_r8 (CPU *cpu, instruction_T instrc) {
    printf("ADC A, r8.                  ; Add the value in r8 PLUS the carry flag to Register A \n");

    // Yes, A + r8 + Carry Flag. -- If it rolls over. That's ok, track it with the Carry Flag.

    uint8_t *reg_table[8] = { 
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, NULL, &cpu->A
    };
    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.


    uint8_t carry_val = (cpu->F & FLAG_C) ? 1 : 0;

    // Use 16bit for Flag checks. 8bit will truncate results
    uint16_t add_16bit  = (cpu->A + op_r8 + carry_val);
    uint8_t final_8bit = (uint8_t)add_16bit;

    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag    
    (((cpu->A & 0x0F) + (op_r8 & 0x0F) + carry_val) > 0x0F) ? set_flag(2) : clear_flag(2); // H Flag
    (add_16bit > 0xFF) ? set_flag(3) : clear_flag(3); // C Flag
    clear_flag(1);  // N Flag (Subtraction) Always cleared on ADC

    cpu->A = final_8bit;
    cpu->PC ++;

    // Bytes = 1
}
static void ADC_A_p_HL (CPU *cpu, instruction_T instrc) {
    printf("ADC A, [HL]. Called, not setup.\n");

    uint8_t hl_val = external_read(cpu->HL);
    uint8_t carry_val = (cpu->F & FLAG_C) ? 1 : 0;

    uint16_t add_16bit = (cpu->A + hl_val + carry_val);
    uint8_t final_8bit = (uint8_t)add_16bit;    


    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag    
    (((cpu->A & 0x0F) + (hl_val & 0x0F) + carry_val) > 0x0F) ? set_flag(2) : clear_flag(2); // H Flag
    (add_16bit > 0xFF) ? set_flag(3) : clear_flag(3); // C Flag
    clear_flag(1);  // N Flag (Subtraction) Always cleared on ADC

    cpu->A = final_8bit;
    cpu->PC ++;
}
static void ADC_A_n8 (CPU *cpu, instruction_T instrc) {
    printf("ADC A, n8. Called, not setup.\n");

    uint8_t n8_val = instrc.operand1;
    uint8_t carry_val = (cpu->F & FLAG_C) ? 1 : 0;

    uint16_t add_16bit = (cpu->A + n8_val + carry_val);
    uint8_t final_8bit = (uint8_t)add_16bit;    


    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag    
    (((cpu->A & 0x0F) + (n8_val & 0x0F) + carry_val) > 0x0F) ? set_flag(2) : clear_flag(2); // H Flag
    (add_16bit > 0xFF) ? set_flag(3) : clear_flag(3); // C Flag
    clear_flag(1);  // N Flag (Subtraction) Always cleared on ADC

    cpu->A = final_8bit;
    cpu->PC += 2;
}

// SUB / SBC Instructions:
static void SUB_A_r8 (CPU *cpu, instruction_T instrc) {     // Subtract values in a, by 8byte register
    printf("SUB A, r8. Called.              ; Sub Value in Register A, by r8 value\n");

    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, NULL, &cpu->A
    };
    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.
    uint8_t reg_a = cpu->A;

    uint16_t sub_16bit = (cpu->A - op_r8);
    uint8_t final_8bit = (uint8_t)sub_16bit;


    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag
    ((reg_a & 0x0F) < (op_r8 & 0x0F)) ? set_flag(2) : clear_flag(2); // H Flag
    (reg_a < op_r8) ? set_flag(3) : clear_flag(3); // C Flag
    set_flag(1);  // N Flag (Subtraction) Always SET on SUB/SBC

    cpu->A = final_8bit;
    cpu->PC ++;

    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (i.e. if r8 > A).
    */
}
static void SUB_A_p_HL(CPU *cpu, instruction_T instrc) {
    printf("SUB A, [HL]. Called, not setup.\n");


    uint8_t hl_val = external_read(cpu->HL);
    uint8_t reg_a = cpu->A;

    uint16_t sub_16bit = (cpu->A - hl_val);
    uint8_t final_8bit = (uint8_t)sub_16bit;


    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag
    ((reg_a & 0x0F) < (hl_val & 0x0F)) ? set_flag(2) : clear_flag(2); // H Flag
    (reg_a < hl_val) ? set_flag(3) : clear_flag(3); // C Flag
    set_flag(1);  // N Flag (Subtraction) Always SET on SUB/SBC

    cpu->A = final_8bit;
    cpu->PC ++;

}
static void SUB_A_n8(CPU *cpu, instruction_T instrc) {
    printf("SUB A, n8. Called.                  ; Subtract Register A by immediate value n8\n");

    uint8_t n8 = instrc.operand1;
    uint8_t reg_a = cpu->A;

    uint16_t sub_16bit = (cpu->A - n8);
    uint8_t final_8bit = (uint8_t)sub_16bit;


    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag
    ((reg_a & 0x0F) < (n8 & 0x0F)) ? set_flag(2) : clear_flag(2); // H Flag
    (reg_a < n8) ? set_flag(3) : clear_flag(3); // C Flag
    set_flag(1);  // N Flag (Subtraction) Always SET on SUB/SBC

    cpu->A = final_8bit;
    cpu->PC ++;
}
// SBC (Sub with the cary flag):
static void SBC_A_r8(CPU *cpu, instruction_T instrc) {     // Subtract the value in r8 and the carry flag from A.
    printf("SBC A, r8. Called.                  ; Subtract value in r8 (and the carry flag) from Register A\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;

    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, NULL, &cpu->A
    };
    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.

    uint8_t carry_val = (cpu->F & FLAG_C) ? 1 : 0;

    // Use 16bit for Flag checks. 8bit will truncate results
    uint16_t sub_16bit  = (cpu->A - op_r8 - carry_val);
    uint8_t final_8bit = (uint8_t)sub_16bit;
    uint8_t reg_a = cpu->A;


    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag
    (((reg_a & 0x0F) - (op_r8 & 0x0F) - carry_val) < 0) ? set_flag(2) : clear_flag(2); // H Flag
    
    // This has + carry, as it checks if the whole result underflowed
    // If register a is less than the TOTAL amount of in op_r8. Then set the flag. (Hence the + carry_val)
    (reg_a < (op_r8 + carry_val)) ? set_flag(3) : clear_flag(3); // C Flag
    set_flag(1);  // N Flag (Subtraction) Always SET on SUB/SBC

    cpu->A = final_8bit;
    cpu->PC ++;


    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (i.e. if (r8 + carry) > A)
    */
}
static void SBC_A_p_HL(CPU *cpu, instruction_T instrc) {   // Subtract the byte pointed to by HL and the carry flag from A.
    printf("SBC A, [HL]. Called, not setup.             ; Subtract value in hl (and the carry flag) from Register A\n");

    uint8_t hl_val = external_read(cpu->HL);
    uint8_t carry_val = (cpu->F & FLAG_C) ? 1 : 0;

    // Use 16bit for Flag checks. 8bit will truncate results
    uint16_t sub_16bit  = (cpu->A - hl_val - carry_val);
    uint8_t final_8bit = (uint8_t)sub_16bit;
    uint8_t reg_a = cpu->A;


    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag
    (((reg_a & 0x0F) - (hl_val & 0x0F) - carry_val) < 0) ? set_flag(2) : clear_flag(2); // H Flag
    
    // This has + carry, as it checks if the whole result underflowed
    (reg_a < (hl_val + carry_val)) ? set_flag(3) : clear_flag(3); // C Flag
    set_flag(1);  // N Flag (Subtraction) Always SET on SUB/SBC

    cpu->A = final_8bit;
    cpu->PC ++;
    // FLAGS: see sbc_a_r8
}
static void SBC_A_n8(CPU *cpu, instruction_T instrc) {     // Subtract the value n8 and the carry flag from A.
    printf("SBC A, n8. Called.          ; Subtract value in n8 (and the carry flag) from Register A\n");

    uint8_t n8 = instrc.operand1;
    uint8_t carry_val = (cpu->F & FLAG_C) ? 1 : 0;

    // Use 16bit for Flag checks. 8bit will truncate results
    uint16_t sub_16bit  = (cpu->A - n8 - carry_val);
    uint8_t final_8bit = (uint8_t)sub_16bit;
    uint8_t reg_a = cpu->A;


    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag
    (((reg_a & 0x0F) - (n8 & 0x0F) - carry_val) < 0) ? set_flag(2) : clear_flag(2); // H Flag
    
    // This has + carry, as it checks if the whole result underflowed
    (reg_a < (n8 + carry_val)) ? set_flag(3) : clear_flag(3); // C Flag
    set_flag(1);  // N Flag (Subtraction) Always SET on SUB/SBC
    
}

// Increment & Decrement Instructions:
static void INC_r8(CPU *cpu, instruction_T instrc) {    // Increment Register r8
    printf("INC_r8.               ; Registry H ++\n");

    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, NULL, &cpu->A
    };
    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.

    ((op_r8 & 0x0F) == 0x0F) ? set_flag(2) : clear_flag(2);    // Set H Flag for overlow from bit 3
    (*reg_table[op_index]) ++;  // Make sure it decrements the actual Register (through a pointer)
    (op_r8 == 0) ? set_flag(0) : clear_flag(0);                // Set (Z) Zero Flag

    clear_flag(1);  // N Flag Cleared (SUB Flag)
    cpu->PC ++;

}
// Decrement Instructions
static void DEC_r8(CPU *cpu, instruction_T instrc) {    // Decrement High bit Register  (-- => B, D, H)
    printf("DEC_hr8.               ; Registery H --\n");

    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, NULL, &cpu->A
    };
    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.

    set_flag(1);        // Set subtraction Flag always set.
    (op_r8 == 0) ? set_flag(2) : clear_flag(2);     // Set/Clear H borrow flag from bit 4.
    (*reg_table[op_index]) --;  // Make sure it decrements the actual Register (through a pointer)
    (op_r8 == 0) ? set_flag(0) : clear_flag(0);;    // Set/Clear (Z) Zero Flag

    set_flag(1);        // Set subtraction Flag always set.
    cpu->PC ++;


    // Bytes = 1
}

// Inc/ Dec, HL value inside pointer [ ]
static void INC_p_HL(CPU *cpu, instruction_T instrc) {  // Increment 16 bit HL register, The Value In Pointer ++ HL
    printf("INC [HL].               ; EXP: 8bit++ <- [HL]\n");

    uint8_t hl_val = external_read(cpu->HL);
    ((hl_val & 0x0F) == 0x0F) ? set_flag(2) : clear_flag(2);    // Check Val before DEC. If 0, it will need to borrow from bit 4. (So, then set H flag)
    
    external_write(cpu->HL,(cpu->HL ++));
    (hl_val == 0) ? set_flag(0) : clear_flag(0);                // Set/Clear (Z) Zero Flag.
    external_write(cpu->HL, hl_val);

    clear_flag(1);      // Clear subtraction flag (N)
    cpu->PC ++;

    // Bytes = 1
}
static void DEC_p_HL(CPU *cpu, instruction_T instrc) {      // Decrement 16 bit HL in register, The Value In Pointer -- HL
    printf("DEC [HL].               ; EXP: 8bit-- <- [HL]\n");
    // NOTE, technically this is INC_p_r16. But there is not other time that happens. Except for [HL].. SO NVM!

    uint8_t hl_val = external_read(cpu->HL);
    (hl_val == 0) ? set_flag(2) : clear_flag(2);            // Check Val before DEC. If 0, it will need to borrow from bit 4. (So, then set H flag)
    
    external_write(cpu->HL,(cpu->HL --));
    (hl_val == 0) ? set_flag(0) : clear_flag(0);            // Set/Clear (Z) Zero Flag.
    external_write(cpu->HL, hl_val);

    set_flag(1);        // Set subtraction flag (N)
    cpu->PC ++;

    // Bytes = 1
}

// CP. ComPARE Instructions:
static void CP_A_r8(CPU *cpu, instruction_T instrc) {       // ComPare -> value in pointer HL to A
    // Depending on the OP Code, it changes WHICH instruction is compared.
    printf("CP A, r8.               ; EXP: ComPare (cpu->A - r8_reg) --> Set Flags \n");

    uint8_t op_c_reg = instrc.opcode;
    uint8_t r8_reg = 0;

    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, NULL, &cpu->A
    };
    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.
    
    uint8_t result = (cpu->A - op_r8);

    set_flag(1); // N Flag - Always Set (Subtraction Flag)
    (result == 0) ? set_flag(0) : clear_flag(0);                           // Z Flag  
    
    ((cpu->A & 0x0F) < (op_r8 & 0x0F)) ? set_flag(2) : clear_flag(2);      // H Flag
    (cpu->A < op_r8) ? set_flag(3) : clear_flag(3);                        // C Flag

    cpu->PC++;

    // Bytes = 1
}
static void CP_A_p_HL(CPU *cpu, instruction_T instrc) {     // ComPare -> value in pointer HL to A
    printf("CP A, [HL]. ComPare Called.         ; Subtracts pointered from [HL] from A (Sets flags, disregards results)\n");
    uint8_t hl_val = external_read(cpu->HL);
    uint8_t result = (cpu->A - hl_val);

    // 
    set_flag(1);    // N Flag - Always Set (Subtraction Flag)

    (result == 0) ? set_flag(0) : clear_flag(0);                        // Z Flag
    ((cpu->A & 0x0F) < (hl_val & 0x0F)) ? set_flag(2) : clear_flag(2);  // H Flag
    (cpu->A < hl_val) ? set_flag(3) : clear_flag(3);                    // C Flag
    
    cpu->PC ++;
}
static void CP_A_n8(CPU *cpu, instruction_T instrc) {       // ComPare -> value in n8 to A
    printf("CP A, n8. ComPare.                  ; Subtracts value in n8 from A (Sets flags, disregards results)\n");
    uint8_t n8_val = instrc.operand1;
    uint8_t result = (cpu->A - instrc.operand1);

    // 
    set_flag(1);    // N Flag - Always Set (Subtraction Flag)

    (result == 0) ? set_flag(0) : clear_flag(0);                        // Z Flag
    ((cpu->A & 0x0F) < (n8_val & 0x0F)) ? set_flag(2) : clear_flag(2);  // H Flag
    (cpu->A < n8_val) ? set_flag(3) : clear_flag(3);                    // C Flag
    
    cpu->PC += 2;

    // Bytes = 2
}


// -----------------------------------------------
/// SECTION:
// Stack Manipulation, ADD, INC, DEC, LD, POP, PUSH

// ADD Stack manipulation Instructions:
// Special SP / e8 ADD function.
static void ADD_SP_e8(CPU *cpu, instruction_T instrc) {     // e8 = SIGNED int.
    printf("ADD SP, e8. Called, not setup.\n");

    int8_t e_val = (int8_t)instrc.operand1;       // NOTICE int8_t = signed, because e = signed 8bit register. Because it's relative a: +- 
    uint8_t add_result = (cpu->A + e_val);

    // NOTE: ADD_SP_e8  Z and N aare always cleared.
    clear_flag(0); // Z Flag Cleared
    clear_flag(1); // N Flag Cleared (Subtraction)
    ((cpu->A & 0x0F) + (e_val & 0x0F) > 0x0F) ? set_flag(2) : clear_flag(2); // H Flag
    (add_result > 0xFF) ? set_flag(3) : clear_flag(3); // C Flag
    

    cpu->A = add_result;
    cpu->PC += 2;

    // Bytes = 2
}
static void ADD_HL_r16(CPU *cpu, instruction_T instrc) {
    printf("ADD HL, r16. Called.\n");

    uint16_t *reg16_table[4] = {
        &cpu->BC, &cpu->DE, &cpu->HL, &cpu->SP
    };
    uint8_t register_index = (instrc.opcode >> 4) & 0x03;       
    uint16_t op_r16 = *reg16_table[register_index];                   // The calculated "Source" Register, from the OPCODE.    
    uint16_t hl_val = cpu->HL;

    uint32_t add_result = (hl_val + op_r16);
    uint16_t final_result = (uint16_t)add_result;

    // Z FLAG UNCHANGED, Do not set, or clear.
    clear_flag(1);  // N Flag Cleared (Subtraction)
    ((hl_val & 0x0FFF) + (op_r16 & 0x0FFF) > 0x0FFF) ? set_flag(2) : clear_flag(2); // H Flag
    (add_result > 0xFF) ? set_flag(3) : clear_flag(3); // C Flag
    
    cpu->HL = final_result;
    cpu->PC ++;
}

// Full INC/DEC 16 bit Registers (BC, DE, HL):
static void INC_r16(CPU *cpu, instruction_T instrc) {
    printf("INC r16.               ; EXP: BC ++\n");

    switch (instrc.opcode) {
        case 0x03: cpu->BC ++; break;
        case 0x13: cpu->DE ++; break;
        case 0x23: cpu->HL ++; break;
        case 0x33: cpu->SP ++; break;       // Notice this is technically STACK MANIPULATION. INC_SP
    }

    cpu->PC ++;         // this is only 1 Byte.

    // Bytes = 1
    // FLAGS: NONE AFFECTED
}
static void DEC_r16(CPU *cpu, instruction_T instrc) {
    printf("DEC r16.               ; EXP: BC --\n");

    switch (instrc.opcode) {
        case 0x0B: cpu->BC --; break;
        case 0x1B: cpu->DE --; break;
        case 0x2B: cpu->HL --; break;
        case 0x3B: cpu->SP --; break;       // Notice this is technically STACK MANIPULATION. DEC_SP
    }

    cpu->PC ++;         // This is only 1 Byte.

    // Bytes = 1
    // FLAGS: NONE AFFECTED

}





static void POP_AF(CPU *cpu, instruction_T instrc) {        // Pop register AF from the stack.
    printf("POP AF Registers from SP.\n");

    printf("POP AF. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
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
static void POP_r16(CPU *cpu, instruction_T instrc) {       // Pop register r16 from the stack.
    printf("POP r16 register from SP.\n");
    printf("POP r16. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;

    /*
    This is roughly equivalent to the following imaginary instructions:
        LD LOW(r16), [SP]   ; C, E or L
        INC SP
        LD HIGH(r16), [SP]  ; B, D or H
        INC SP
    */

    // Bytes = ??
    // FLAGS: None affected
}
static void PUSH_AF(CPU *cpu, instruction_T instrc) {       // Push register AF into the stack. 
    printf("PUSH_AF, Writes the A value, and the Flags into SP (Stack Pointer) 0x%04X\n", cpu->SP);

    // Decrement SP, 
    // Copy A => into SP (stack pointer)        NOTE: Don't overwrite the SP's pointer itself, but save it in HRAM or something, Where ever SP is "pointing" to
    // Decrement SP
    // Copy F (The 1 or 0s in Flags) Into where the SP, points to (Likely HRAM)


    cpu->SP --;
    external_write(cpu->SP, cpu->A);
    cpu->SP --;
    external_write(cpu->SP, cpu->F);

    cpu->PC ++;     // PUSH_AF is only 1 byte.

    // Bytes = 1
    // FLAGS: None affected
}


static void PUSH_r16(CPU *cpu, instruction_T instrc) {      // Push register r16 into the Stack.
    // Does SLIGHTLY Different logic from AF. -- Why this is under a different function.

    /// This decrements SP, pushes value in B, D, H (High Register) into first SP location. Decrements SP then pushes the Low Register into the second SP location.

    switch (instrc.opcode) {
        case 0xC5:    // BC to stack
            cpu->SP --;
            external_write(cpu->SP, cpu->B);
            cpu->SP --;
            external_write(cpu->SP, cpu->C);
            break;
        case 0xD5:    // DE to stack
            cpu->SP --;
            external_write(cpu->SP, cpu->D);
            cpu->SP --;
            external_write(cpu->SP, cpu->E);
            break;
        case 0xE5:    // HL to stack
            cpu->SP --;
            external_write(cpu->SP, cpu->H);
            cpu->SP --;
            external_write(cpu->SP, cpu->L);
            break;
    }

    cpu->PC ++;

    // Bytes = 1
    // FLAGS: None affected
}


/// SECTION:
// Subroutine instrucitons.
// CALL, RET, RETI, RST_vec

static void CALL_a16(CPU *cpu, instruction_T instrc) {      // Pushes the address of the instruction after the CALL, on the stack. Such that RET can pop it later; Then it executes implicit JP n16
    printf("CALL_n16 Called, 'Push PC into SP, so RET can POP later', then jump to n16 Address\n");
    // This pushes the address of the instruction after the CALL on the stack, such that RET can pop it later; then, it executes an implicit JP n16.

    // Call n16 is like "Go to subroutine at n16. And remember to come back here".
    // The [remember to come back here] is done by stack pointers. 
    
    // The RET will later "Pop two bytes from the stack", then jump back to the saved PC.
    uint16_t pc_loc = (cpu->PC + 3);

    uint8_t split_addr_LOW = pc_loc & 0xFF;
    uint8_t split_addr_HIGH = (pc_loc >> 8) & 0xFF;

    // YES Low byte First WHEN it's incrementing.. / reading.
    // BUT, because it's Decrementing the SP, High Byte first.
    
    // Push return address onto stack (high byte first)
    cpu->SP --; 
    external_write(cpu->SP, split_addr_HIGH);
    cpu->SP --;
    external_write(cpu->SP, split_addr_LOW);
    

    // Next jump to Location:
    cpu->PC = cnvrt_lil_endian(instrc.operand1, instrc.operand2);

    // DO not assign PC to anything.


    /*
    The TL;DR
    Save where this instruction was called from (IE the PC + 3) into the SP (Stack pointer).
    Then jump to the location inside operand 1/ 2 of (the n16 Value).

    That will usually be a set of instructions. Or just one instruction.

    Eventually RET will be called, where it will use the PC that was saved in the SP. 
    To jump back to just AFTER this call was executed.

    Kind of like .........| ..>>continuing..................
                          | ^^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<..
                          |>>..>>>>>> lalalalallal executing.. RET ^^
    */


}
static void CALL_cc_a16(CPU *cpu, instruction_T instrc) {   // Call address n16 if condition cc is met.
    printf("CONDITIONAL CALL_cc_n16 Called, IF* condition is met 'Push PC into SP, so RET can POP later', then jump to n16 Address\n");

    int proceed = 0;

    switch (instrc.opcode) {
        case 0xC4:
            // Z flag is NOT set.
            if (!(cpu->F & FLAG_Z)) proceed = 1;
            break;
        case 0xD4:
            // C flag is NOT set.
            if (!(cpu->F & FLAG_C)) proceed = 1;
            break;
        case 0xCC:
            // Z flag IS set.
            if ((cpu->F & FLAG_Z)) proceed = 1;
            break;
        case 0xCD:
            // C flag IS set.
            if ((cpu->F & FLAG_C)) proceed = 1;
            break;
    }

    if (proceed) {
        uint16_t pc_loc = (cpu->PC + 3);

        uint8_t split_addr_LOW = pc_loc & 0xFF;
        uint8_t split_addr_HIGH = (pc_loc >> 8) & 0xFF;

        cpu->SP --; 
        external_write(cpu->SP, split_addr_HIGH);
        cpu->SP --;
        external_write(cpu->SP, split_addr_LOW);

        cpu->PC = cnvrt_lil_endian(instrc.operand1, instrc.operand2);
    }
    else {
        printf("CALL cc Conditions are NOT met. Skipping..\n");
        cpu->PC += 3;       // Skip over the entire CALL instruction (Which is 3 bytes in Length)
    }   
}

static void RET(CPU *cpu, instruction_T instrc) {           // RETurn from subroutine.
    printf("Ret Called, 'RETurn from subroutine..' ... ' Populate the PC stored in the SP\n");
    // This RETurns back to the PC saved in the Stack Pointer (SP) from the CALL function.

    // TL;DR : Populate the PC from the SP.
    uint8_t low_byte = external_read(cpu->SP);
    cpu->SP ++;
    uint8_t high_byte = external_read(cpu->SP);
    cpu->SP ++;

    cpu->PC = cnvrt_lil_endian(low_byte, high_byte);
    
    // Bytes: 1
    // FLAGS: None affected
}
static void RET_cc(CPU *cpu, instruction_T instrc) {        // RETurn from subroutine if condition CC is met
    int proceed = 0;

    switch (instrc.opcode) {
        case 0xC0:
            // Z flag is NOT set.
            if (!(cpu->F & FLAG_Z)) proceed = 1;
            break;
        case 0xD0:
            // C flag is NOT set.
            if (!(cpu->F & FLAG_C)) proceed = 1;
            break;
        case 0xC8:
            // Z flag IS set.
            if ((cpu->F & FLAG_Z)) proceed = 1;
            break;
        case 0xD8:
            // C flag IS set.
            if ((cpu->F & FLAG_C)) proceed = 1;
            break;
    }

    if (proceed) {
        // TL;DR : Populate the PC from the SP.
        uint8_t low_byte = external_read(cpu->SP);
        cpu->SP ++;
        uint8_t high_byte = external_read(cpu->SP);
        cpu->SP ++;

        cpu->PC = cnvrt_lil_endian(low_byte, high_byte);
    }
    else {
        printf("CALL cc Conditions are NOT met. Skipping..\n");
        cpu->PC ++;       // Skip over the entire CALL instruction (Which is 3 bytes in Length)
    }  
    
    // Bytes: 1
    // FLAGS: None affected
}
static void RETI(CPU *cpu, instruction_T instrc) {          // RETurn from subroutine and enable I-nterupts.
    // This is basically equivalent to executing EI then RET, meaning that IME is set right after this instruction.
    printf("RETI. Called, not setup HALT\n");
    cpu_status.halt = 1;
    // FLAGS: None affected
}
static void RST_vec(CPU *cpu, instruction_T instrc) {       // Call address vec. This is a shorter and faster equivalent to CALL for suitable values of vec.
    printf("RST_vec. Called, not setup HALT\n");
    cpu_status.halt = 1;
    // FLAGS: None affected
}





// -----------------------------------------------
/// SECTION:
// Bitwise, AND, OR XOR instructions.

static void AND_A_r8(CPU *cpu, instruction_T instrc) {      // Set A to the bitwise AND between the value in r8 and A
    printf("AND A, r8. Called.          ; Result = Set bit to 1 or 0. For each indvidual bit. Bit 0 through Bit 7.\n");
    // This is a BITWISE AND. Which is to say. It returns an 8bit value. Where each individual val of 1 bits. Match in both 8bit variable.
    uint8_t *reg_table[8] = { &cpu->B, &cpu->C, &cpu->D, &cpu->E, &cpu->H, &cpu->L };

    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t AND_result = (cpu->A & *reg_table[op_index]);

    cpu->A = AND_result;

    (AND_result == 0) ? set_flag(0) : clear_flag(0);
    clear_flag(1);  // ALways cleard
    set_flag(2);    // Always set
    clear_flag(3);  // Always cleared.

    cpu->PC ++;

    // Bytes = 1
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = 0
    */
}
static void AND_A_p_HL(CPU *cpu, instruction_T instrc) {    // Set A to the bitwise AND between the byte pointed to by HL and A
    printf("AND A, [HL]. Called.            ; Bitwise AND. 1&1 = 1, rest =0. From Value in [HL]\n");

    uint8_t AND_result = (cpu->A & external_read(cpu->HL));
    cpu->A = AND_result;

    // One liner:       cpu->A &= external_read(cpu->HL)

    (AND_result == 0) ? set_flag(0) : clear_flag(0);
    clear_flag(1);  // ALways cleared
    set_flag(2);    // Always set
    clear_flag(3);  // Always cleared

    cpu->PC ++;
    // Bytes = 1
    // FLAGS: see: AND A, r8
}
static void AND_A_n8(CPU *cpu, instruction_T instrc) {      // Set A to the bitwise AND between the value n8 and A
    printf("AND A, n8. Called.              ; Bitwise AND. 1&1 = 1, rest =0. From immediate value (n8)\n");

    uint8_t AND_result = (cpu->A & instrc.operand1);
    cpu->A = AND_result;

    // One liner:       cpu->A &= instrc.operand1;

    (AND_result == 0) ? set_flag(0) : clear_flag(0);
    clear_flag(1);  // ALways cleared
    set_flag(2);    // Always set
    clear_flag(3);  // Always cleared

    cpu->PC += 2;

    // Bytes = 2;
    // FLAGS: see: AND A, r8
}
// OR Instructions:
static void OR_A_r8(CPU *cpu, instruction_T instrc) {       // Set A to the bitwise OR between the value in r8 and A
    printf("AND A, r8. Called.          ; Bitwise OR, 1|1 = 1. Only 0 if both bits are Zero, 0&0=0\n");
    // If varbiale one bit(x) = 1, OR variable 2 bit(x) = 1. RESULT = One. 
    // ONLY will give result bit(x) 0. if BOTH variable 1 & variable 2 = 0. IE 0&0
    uint8_t *reg_table[8] = { &cpu->B, &cpu->C, &cpu->D, &cpu->E, &cpu->H, &cpu->L };

    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t OR_result = (cpu->A | *reg_table[op_index]);

    cpu->A = OR_result;

    (OR_result == 0) ? set_flag(0) : clear_flag(0);     // Set if Zero (Z Flag)
    clear_flag(1);  // ALways cleared
    clear_flag(2);  // Always cleared
    clear_flag(3);  // Always cleared

    cpu->PC ++;

    // Bytes = 1
}
static void OR_A_p_HL(CPU *cpu, instruction_T instrc) {     // Set A to the bitwise OR between the byte pointed to by HL and A
    printf("OR A, [HL]. Called.         ; Bitwise OR, 1|1 = 1. Only 0 if both bits are Zero, 0&0=0\n");
    uint8_t OR_result = (cpu->A | external_read(cpu->HL));
    cpu->A = OR_result;

    // One liner:       cpu->A &= external_read(cpu->HL)

    (OR_result == 0) ? set_flag(0) : clear_flag(0);
    clear_flag(1);  // ALways cleared
    clear_flag(2);  // Always cleared
    clear_flag(3);  // Always cleared

    cpu->PC ++;

    // Bytes = 1
    // FLAGS: see: OR_A_r8
}
static void OR_A_n8(CPU *cpu, instruction_T instrc) {       // Set A to the bitwise OR between the value n8 and A
    printf("OR A, n8. Called.         ; Bitwise OR, 1|1 = 1. Only 0 if both bits are Zero, 0&0=0\n");
    uint8_t OR_result = (cpu->A | instrc.operand1);
    cpu->A = OR_result;

    // One liner:       cpu->A &= external_read(cpu->HL)

    (OR_result == 0) ? set_flag(0) : clear_flag(0);
    clear_flag(1);  // ALways cleared
    clear_flag(2);  // Always cleared
    clear_flag(3);  // Always cleared

    cpu->PC += 2;

    // Bytes = 1
    // FLAGS: see: OR_A_r8   
}
// XOR Instructions:
static void XOR_A_r8(CPU *cpu, instruction_T instrc) {      // Set A to the bitwise XOR between the value in r8 and A
    printf("XOR A, r8. Called.              ; Bitwise XOR 1^1=0, 1^0=1, 0^0=0. One or Other is 1. NOT BOTH!\n");

    uint8_t *reg_table[8] = { &cpu->B, &cpu->C, &cpu->D, &cpu->E, &cpu->H, &cpu->L };

    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t XOR_result = (cpu->A ^ *reg_table[op_index]);

    cpu->A = XOR_result;


    (XOR_result == 0) ? set_flag(0) : clear_flag(0);    // Set if Zero (Z Flag)
    clear_flag(1);  // ALways cleared
    clear_flag(2);  // Always cleared
    clear_flag(3);  // Always cleared

    // Bytes = 1
}
static void XOR_A_p_HL(CPU *cpu, instruction_T instrc) {    // Set A to the bitwise XOR between the byte pointed to by HL and A
    printf("XOR A, [HL]. Called.              ; Bitwise XOR 1^1=0, 1^0=1, 0^0=0. One or Other is 1. NOT BOTH!\n");

    uint8_t XOR_result = (cpu->A ^ external_read(cpu->HL));
    cpu->A = XOR_result;

    // One liner:       cpu->A &= external_read(cpu->HL)

    (XOR_result == 0) ? set_flag(0) : clear_flag(0);
    clear_flag(1);  // ALways cleared
    clear_flag(2);  // Always cleared
    clear_flag(3);  // Always cleared

    cpu->PC ++;

    // Bytes = 1
}
static void XOR_A_n8(CPU *cpu, instruction_T instrc) {      // Set A to the bitwise XOR between the value n8 and A
    printf("XOR A, n8. Called.              ; Bitwise XOR 1^1=0, 1^0=1, 0^0=0. One or Other is 1. NOT BOTH!\n");

    uint8_t XOR_result = (cpu->A ^ instrc.operand1);
    cpu->A = XOR_result;

    // One liner:       cpu->A &= external_read(cpu->HL)

    (XOR_result == 0) ? set_flag(0) : clear_flag(0);    // Z condition.
    clear_flag(1);  // N ALways cleared
    clear_flag(2);  // C Always cleared
    clear_flag(3);  // H Always cleared

    cpu->PC += 2;

    // Bytes = 2
}

// CPL Instruction. Like a bitwse NOT
static void CPL(CPU *cpu, instruction_T instrc) {           // ComPLement accumulator (A = ~A); also called bitwise NOT
    printf("CPL. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;

    // Bytes = 1
}




// -----------------------------------------------
/// SECTION:
// Bitshift and ROTATE instructions

// NON-PREFIXED
static void RRA(CPU *cpu, instruction_T instrc) {           // Rotate register A -> Right. Through the carry flag. b8 ---> b0 <> [carry flag]
    printf("RRA. Called.            ;   Rotate Register A (bit) RIGHT --> through Flag C.\n");

    uint8_t a_reg = cpu->A;                             // Snapshot A register.
    uint8_t og_carry = (cpu->F & FLAG_C) ? 1 : 0;       // Bit 4, C carry flag.
    uint8_t new_carry = (cpu->A & 0x1);                 // Bit 0, before shift

    uint8_t shifted_a_reg = (a_reg >> 1) | (og_carry << 7);

    cpu->A = shifted_a_reg;
    if (new_carry > 0) {
        set_flag(3);
    }else {
        clear_flag(3);
    }

    clear_flag(0);
    clear_flag(1);
    clear_flag(2);

    cpu->PC++;

    // Bytes = 1
}

static void RRCA(CPU *cpu, instruction_T instrc) {          // Rotate Register A right. b8 ---> b0
    printf("RRCA Command, 'Rotate register A right?' hmmm\n");
    printf("RRCA. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;

    /*
      ┏━━━━━━━ A ━━━━━━━┓   ┏━ Flags ━┓
    ┌─╂→ b7 → ... → b0 ─╂─┬─╂→   C    ┃
    │ ┗━━━━━━━━━━━━━━━━━┛ │ ┗━━━━━━━━━┛
    └─────────────────────┘
    */

    // Bytes = 1
}

static void RLA(CPU *cpu, instruction_T instrc) {           // Rotate Register A Left. Through the carry flag <---
    printf("RLA. Rotate Register A Left (through carry flag).\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;

    // Rotate register A left, through the carry flag.


    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
    // Bytes = 1
}

static void RLCA(CPU *cpu, instruction_T instrc) {          // Rotate Register A left.
    printf("RLCA. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;

    /*
    ┏━ Flags ━┓   ┏━━━━━━━ A ━━━━━━━┓
    ┃    C   ←╂─┬─╂─ b7 ← ... ← b0 ←╂─┐
    ┗━━━━━━━━━┛ │ ┗━━━━━━━━━━━━━━━━━┛ │
                └─────────────────────┘
    */

    // Bytes = 1

}



/// NOTICE:
// PREFIXED Rotate Instructions:

static void RL_r8(CPU *cpu, instruction_T instrc) {         // Rotate Byte in Register r8 left, through the carry flag. <---
    printf("RL r8. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RL_p_HL(CPU *cpu, instruction_T instrc) {       // Rotate the byte pointed to by HL left, through the carry flag. <---
    printf("RL [HL]. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    // FLAGS: See RL_r8
}



/// NOTICE:
// These are only in the PREFIXED instruction set!

static void RLC_r8(CPU *cpu, instruction_T instrc) {        // Rotate Registers r8 Left. <---
    printf("RLC r8. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    /*
        FLAGS:
        Z = 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RLC_p_HL(CPU *cpu, instruction_T instrc) {      // Rotate the byte pointed to by [HL] left. <---
    printf("RLC [HL]. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    // FLAGS: See RLC_r8

}
static void RR_r8(CPU *cpu, instruction_T instrc) {         // Rotate Register r8 Right. Through the carry flag. -->
    printf("RR r8. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;

    uint8_t *assigned_register = NULL;      // Set this to say, cpu->B, cpu->C, cpu->E etc..

    /// TODO: NOT FINISHED. This code will NOT! work!

    /// NOTICE: THIS IS PREFIXED Instruction, so this approach isn't actually correct...
    switch (instrc.opcode) {
        case 0x18: 
            // This is RR B (0x18) -- PREFIXED!
            assigned_register = &cpu->B;
            break;
    }
    
    if (assigned_register == NULL) {
        // ERROR, invalid opcode or missing case.
        return;
    }

    uint8_t data_snapshot = *assigned_register;                 // Snapshot of Data
    uint8_t og_carry = (cpu->F & FLAG_C) ? 1 : 0;               // Bit 4, C carry flag.
    uint8_t new_carry = (cpu->A & 0x1);                         // Bit 0, before shift

    uint8_t rotated_byte = (data_snapshot >> 1) | (og_carry << 7);

    
    *assigned_register = rotated_byte;


    if (new_carry > 0) {
        set_flag(3);
    }else {
        clear_flag(3);
    }

    clear_flag(0);
    clear_flag(1);
    clear_flag(2);

    cpu->PC++;     




    /*
        FLAGS:
        Z = 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RR_p_HL(CPU *cpu, instruction_T instrc) {       // Rotate the byte pointed to by [HL] Right. Through the carry flag. -->
    printf("rR [HL]. Called         ; Rotate byte in [HL] RIGHT\n");


    uint8_t data_snapshot = external_read(cpu->HL);     // Snapshot of Data pointed BY HL
    uint8_t og_carry = (cpu->F & FLAG_C) ? 1 : 0;               // Bit 4, C carry flag.
    uint8_t new_carry = (cpu->A & 0x1);                         // Bit 0, before shift

    uint8_t rotated_byte = (data_snapshot >> 1) | (og_carry << 7);

    external_write(cpu->HL, rotated_byte);
    if (new_carry > 0) {
        set_flag(3);
    }else {
        clear_flag(3);
    }

    clear_flag(0);
    clear_flag(1);
    clear_flag(2);

    cpu->PC++;    

    // FLAGS: see RR_r8
}
static void RRC_r8(CPU *cpu, instruction_T instrc) {        // Rotate Register r8 Right. -->
    printf("RRC r8. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;

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
static void RRC_p_HL(CPU *cpu, instruction_T instrc) {      // Rotate the byte pointed to by [HL] Right. -->
    printf("RRC [HL]. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;

    // FLAGS: See RRC r8

}




/// NOTICE:
// These are PREFIXED instructions: 
// SHIFT left & right Arithmetically.

static void SLA_r8(CPU *cpu, instruction_T instrc){         // Shift Left Arithmetically Register r8.  <--
    printf("SLA r8. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void SLA_P_HL(CPU *cpu, instruction_T instrc) {      //  Shift Left Arithmetically the byte pointed to by [HL]. <--
    printf("SLA [HL]. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    // Flags: SEE SLA r8
}
static void SRA_r8(CPU *cpu, instruction_T instrc) {        // Shift Right Arithmetically Register r8. -->
    printf("SRA r8. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void SRA_p_HL(CPU *cpu, instruction_T instrc) {      // Shift Right Arithmetically the byte pointed to by HL. --> 
    printf("SRA [HL]. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    // Flags: See SRA_r8
}
static void SRL_r8(CPU *cpu, instruction_T instrc) {        // Shift Right Logically Register r8. -->
    printf("SRL r8. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */    
}
static void SRL_p_HL(CPU *cpu, instruction_T instrc) {      // Shift Right Logically the byte pointed to by [HL]. -->
    printf("SLR [HL]. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    // FLAGS: See SRL_r8
}




// These are PREFIXED instructions: 
// Swap, Complement Carry Flag. Set Carry Flag.
static void SWAP_r8(CPU *cpu, instruction_T instrc) {       // Swap the upper 4 bits in register r8 and the lower 4 ones. X::Y == Y::X
    printf("SWAP r8. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = 0
    */    
}
static void SWAP_p_HL(CPU *cpu, instruction_T instrc) {     // Swap the upper 4 bits in the byte pointed by HL and the lower 4 ones.
    printf("SWAP [HL]. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    // FLAGS: See SWAP_r8

}

// -----------------------------------------------
/// SECTION:
// These are PREFIXED instructions: 
// Bit Flag instructions:

static void BIT_u3_r8(CPU *cpu, instruction_T instrc) {     // Test bit u3 in register r8m set the zero flag if bit not set
    printf("BIT u3, r8. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;

    /*
        FLAGS:
        Z = Set if the select bit is 0
        N = 0
        H = 1
        C = ---
    */
}
static void BIT_u3_p_HL(CPU *cpu, instruction_T instrc) {   // Test bit u3 in the byte pointed by HL, set the flag if bit not set
    printf("BIT u3, [HL]. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    // FLAGS: See BIT_u3_r8
}
static void RES_u3_r8(CPU *cpu, instruction_T instrc) {     // Set bit u3 in register r8 to 0. Bit 0 is the rightmost one, bit 7 the leftmost one
    printf("RES u3, r8. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    // FLAGS: None affected
}
static void RES_u3_p_HL(CPU *cpu, instruction_T instrc) {   // Set bit u3 in the byte pointed by HL to 0. Bit 0 is the rightmost one, bit 7 the leftmost one
    printf("RES u3, [HL]. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    // FLAGS: None affected
}
static void SET_u3_r8(CPU *cpu, instruction_T instrc) {     // Set bit u3 in register r8 to 1. Bit 0 is the rightmost one, bit 7 the leftmost one
    printf("SET u3, r8. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    // FLAGS: None affected
}
static void SET_u3_p_HL(CPU *cpu, instruction_T instrc) {   // Set bit u3 in the byte pointed by HL to 1. Bit 0 is the rightmost one, bit 7 the leftmost one
    printf("SET u3, [HL]. Called, not setup.\n");
    printf("%sHALTING%s\n", KRED, KNRM);
    cpu_status.halt = 1;
    // FLAGS: None affected
}






















static opcode_t *opcodes[256] = {
/*  ---> X0, X1, X2, X3, X4 ... XB .. XF etc */
/* 0X */ NOP,        LD_r16_n16,    LD_p_r16_A, INC_r16,  INC_r8,     DEC_r8,  LD_hr_n8,   RLCA,     /* || */ LD_p_a16_SP,  ADD_HL_r16, LD_A_p_r16,  DEC_r16,   INC_r8,     DEC_r8,   LD_lr_n8,   RRCA,
/* 1X */ STOP,       LD_r16_n16,    LD_p_r16_A, INC_r16,  INC_r8,     DEC_r8,  LD_hr_n8,   RLA,      /* || */ JR_e8,        ADD_HL_r16, LD_A_p_r16,  DEC_r16,   INC_r8,     DEC_r8,   LD_lr_n8,   RRA,
/* 2X */ JR_cc_e8,   LD_r16_n16,    LD_p_HLI_A, INC_r16,  INC_r8,     DEC_r8,  LD_hr_n8,   DAA,      /* || */ JR_cc_e8,     ADD_HL_r16, LD_A_p_HLI,  DEC_r16,   INC_r8,     DEC_r8,   LD_lr_n8,   CPL,
/* 3X */ JR_cc_e8,   LD_r16_n16,    LD_p_HLI_A, INC_r16,  INC_p_HL,    DEC_p_HL, LD_p_HL_n8, SCF,      /* || */ JR_cc_e8,     ADD_HL_r16, LD_A_p_HLD,  DEC_r16,   INC_r8,     DEC_r8,   LD_hr_n8,   CCF,
/* 4X */ NOP,        LD_B_C,        LD_B_D,     LD_B_E,   LD_B_H,      LD_B_L,   LD_B_DHL,   LD_B_A,   /* || */ LD_C_B,       NOP,        LD_C_D,      LD_C_E,    LD_C_H,      LD_C_L,    LD_C_DHL,   LD_C_A,
/* 5X*/  LD_D_B,     LD_D_C,        NOP,        LD_D_E,   LD_D_H,      LD_D_L,   LD_D_DHL,   LD_D_A,   /* || */ LD_E_B,       LD_E_C,     LD_E_D,      NOP,       LD_E_H,      LD_E_L,    LD_E_DHL,   LD_E_A,
/* 6X */ LD_H_B,     LD_H_C,        LD_H_D,     LD_H_E,   NOP,         LD_H_L,   LD_H_DHL,   LD_H_A,   /* || */ LD_L_B,       LD_L_C,     LD_L_D,      LD_L_E,    LD_L_H,      NOP,       LD_L_DHL,   LD_L_A,
/* 7X */ LD_DHL_B,   LD_DHL_C,      LD_DHL_D,   LD_DHL_E, LD_DHL_H,    LD_DHL_L, HALT,       LD_DHL_A, /* || */ LD_A_B,       LD_A_C,     LD_A_D,      LD_A_E,    LD_A_H,      LD_A_L,    LD_A_DHL,   NOP,
/* 8X */ ADD_A_r8,   ADD_A_r8,      ADD_A_r8,   ADD_A_r8, ADD_A_r8,    ADD_A_r8, ADD_A_p_HL,   ADD_A_r8, /* || */ ADC_A_r8,     ADC_A_r8,   ADC_A_r8,    ADC_A_r8,  ADC_A_r8,    ADC_A_r8,  ADC_A_p_HL,   ADC_A_r8,  
/* 9X */ SUB_A_r8,   SUB_A_r8,      SUB_A_r8,   SUB_A_r8, SUB_A_r8,    SUB_A_r8, SUB_A_p_HL,   SUB_A_r8, /* || */ SBC_A_r8,     SBC_A_r8,   SBC_A_r8,    SBC_A_r8,  SBC_A_r8,    SBC_A_r8,  SBC_A_p_HL,   SBC_A_r8,
/* AX */ AND_A_r8,   AND_A_r8,      AND_A_r8,   AND_A_r8, AND_A_r8,    AND_A_r8, AND_A_p_HL,   AND_A_r8, /* || */ XOR_A_r8,     XOR_A_r8,   XOR_A_r8,    XOR_A_r8,  XOR_A_r8,    XOR_A_r8,  XOR_A_p_HL,   XOR_A_r8,
/* BX */ OR_A_r8,    OR_A_r8,       OR_A_r8,    OR_A_r8,  OR_A_r8,     OR_A_r8,  OR_A_p_HL,    OR_A_r8,  /* || */ CP_A_r8,      CP_A_r8,    CP_A_r8,     CP_A_r8,   CP_A_r8,     CP_A_r8,   CP_A_p_HL,    CP_A_r8,
/* CX */ RET_cc,     POP_r16,       JP_cc_a16,  JP_a16,   CALL_cc_a16, PUSH_r16, ADD_A_n8,   RST_vec,  /* || */ RET_cc,       RET,        JP_cc_a16,   CB_PREFIX, CALL_cc_a16, CALL_a16,  ADC_A_n8,   RST_vec,
/* DX */ RET_cc,     POP_r16,       JP_cc_a16,  BLANK,    CALL_cc_a16, PUSH_r16, SUB_A_n8,   RST_vec,  /* || */ RET_cc,       RETI,       JP_cc_a16,   BLANK,     CALL_cc_a16, BLANK,     SBC_A_n8,   RST_vec,
/* EX */ LDH_p_a8_A, POP_r16,      LDH_p_C_A,  BLANK,    BLANK,        PUSH_r16, AND_A_n8,   RST_vec,  /* || */ ADD_SP_e8,    JP_HL,      LD_p_a16_A,  BLANK,     BLANK,       BLANK,     XOR_A_n8,   RST_vec,
/* FX */ LDH_A_p_a8, POP_AF,      LDH_A_p_C,  DI,       BLANK,        PUSH_AF, OR_A_n8,     RST_vec,  /* || */ LD_HL_SP_Pe8,  LD_SP_HL,  LD_A_p_a16,  EI,        BLANK,       BLANK,     CP_A_n8,    RST_vec,
};


int execute_instruction(CPU *cpu, instruction_T instrc) {
    printf(" PC=%04X, OPCODE=%02X, OP1=0x%02X, OP2=0x%02X\n", cpu->PC, instrc.opcode, instrc.operand1, instrc.operand2);

    opcodes[instrc.opcode](cpu, instrc);

    printf("%sExecution Block Finished.%s\n", KYEL, KNRM);
    return 0;
}

