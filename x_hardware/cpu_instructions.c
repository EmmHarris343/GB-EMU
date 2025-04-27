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
static void DI(CPU *cpu, instruction_T instrc) {        // DI - Disables interrupt Handling (Flag set to := 0)
    printf("DI Called.                  ; IME := 0 (Interupt flag 0 - disabled)\n");
    cpu->IME = 0;
    cpu->PC ++;

    // Bytes = 1
    // No flags affected. ---> The Register flags in F that is.
}
 
static void EI(CPU *cpu, instruction_T instrc) {        // EI - Enables interrupt Handling (Flag set to := 1)
    printf("EI Called.                  ; IME := 1 (Interupt flag 1 - enabled)\n");
    cpu->IME = 1;
    cpu->PC ++;
    
    // Bytes = 1
    // No flags affected. ---> The Register flags in F that is.
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
    printf("STOP Called, not setup PANIC HALT\n");
    cpu_status.panic = 1;
    

}
// DAA (WEIRD INSTRUCTION) -- VERY complicated what it actually does!
static void DAA(CPU *cpu, instruction_T instrc) {
    
    // DAA => Decimal Adjust Accumulator.
    printf("DAA. Called, not setup.\n");
    printf("%sPANIC HALTING%s\n", KRED, KNRM);
    cpu_status.panic = 1;
}
// BLANK
static void BLANK(CPU *cpu, instruction_T instrc) {      // Do nothing, basically NOP, but for clarity don't write it like that.
    // DO NOTHING - Not even any command.
    // This shouldn't Even be called.
    printf("%sBLANK Called, This should never be called. Panic Halt.%s\n", KRED, KNRM);
    cpu_status.panic = 1;
}

// Carry Flag Instructions:
static void CCF(CPU *cpu, instruction_T instrc) {           // Complement Carry Flag
    printf("CCF. Called.                ; Invert Carry Flag 0=1, 1=0\n");

    // If it's set, clear it, otherwise set it.
    (cpu->F & FLAG_C) ? clear_flag(3) : set_flag(3);    // Invert the C flag.
    


    /*
        FLAGS:
        Z = --
        N = 0
        H = 0
        C = Inverted
    */     
}
static void SCF(CPU *cpu, instruction_T instrc) {           // Set Carry Flag
    printf("SCF. Called.                ; Set Carry Flag (True)\n");
    set_flag(3);

    /*
        FLAGS:
        Z = --
        N = 0
        H = 0
        C = 1
    */
}











// -----------------------------------------------
//
///                    NOTICE:
// ==---- Starting main OP Code instructions ----==
//
// -----------------------------------------------










// -----------------------------------------------
/// SECTION:
// LD Functions
static void LD_r8_n8(CPU *cpu, instruction_T instrc) {
    printf("LD r8, n8.                 ; EXP: r8 <- n8    ..    Reg.H <- n8 OR Reg.C <- n8\n");
    uint8_t op_index = (instrc.opcode >> 3) & 0x07;      // <-- This style needed for left, right. DOWN left right, DOWN etc
    uint8_t *reg_table[8] = { 
        &cpu->B, &cpu->C, 
        &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, 
        NULL, &cpu->A
    };
    *reg_table[op_index] = instrc.operand1;

    cpu->PC += 2;
    // No flags affected.
}
static void LD_r16_n16(CPU *cpu, instruction_T instrc) {
    printf("LD r16 n16. Copy n16 value into r16 Register\n");
    //printf("Values? OP1: %02X OP2: %02X\n", instrc.operand1, instrc.operand2);
    uint8_t op_index = (instrc.opcode >> 4) & 0x03; // <- This style needed for straight down op codes.
    uint16_t *reg_16table[4] = { 
        &cpu->BC, 
        &cpu->DE, 
        &cpu->HL, 
        &cpu->SP
    };
    *reg_16table[op_index] = cnvrt_lil_endian(instrc.operand1, instrc.operand2);

    cpu->PC += 3;

    // Bytes: 3
    // Flags: None Affected
}
static void LD_p_r16_n16(CPU *cpu, instruction_T instrc) {
    printf("LD [r16] n16                    ; Load n16 value into value pointed by Register in [r16]\n");
    uint16_t load_n16;
    load_n16 = cnvrt_lil_endian(instrc.operand1, instrc.operand2);
}
static void LD_p_HL_n8(CPU *cpu, instruction_T instrc) {       // Copy data from n8, into where HL is being pointed to
    printf("LD [HL] n8 Called.                  ; LD n8 Value into [HL]\n");

    external_write(cpu->HL, instrc.operand1);
    cpu->PC ++;
    // Bytes = 2
    // No flags Affected
}



// -----------------------------------------------
/// SECTION:
// LD Acculator (A Register) instructions

static void LD_p_r16_A(CPU *cpu, instruction_T instrc) {
    printf("LD [r16] A.                     ; Copy Register A into value pointed by register [r16]\n");
    switch (instrc.opcode) {
        case 0x02:
            // LD [BC], A
            external_write(cpu->BC, cpu->A);
            break;
        case 0x12:
            // LD [DE], A
            external_write(cpu->DE, cpu->A);
            break;
    }
    cpu->PC++;
}
static void LD_A_p_r16(CPU *cpu, instruction_T instrc) {
    printf("LD A, [r16] Called.             ; Copy value in register [r16] into Register A\n");
    switch (instrc.opcode) {
        case 0x0A:
            // LD A, [BC]
            cpu->A = external_read(cpu->BC);
            break;
        case 0x1A:
            // LD A, [DE]            
            cpu->A = external_read(cpu->DE);
            break;
            printf("|DEBUG| : A Regiser: %02X || [DE] value: %02X | Addr: %04X\n", cpu->A, external_read(cpu->DE), cpu->DE);
            
    }
    cpu->PC++;
}
// LD/ Load (A) with Increment and Decrement to HL after.
static void LD_p_HLI_A(CPU *cpu, instruction_T instrc) {
    printf("LD [HLI] A, Copy value in A, into the value pointed by HL, then Increment HL\n");

    uint8_t a_val = cpu->A;
    printf("|DEBUG| : A:%02X HL Addr: %04X\n", a_val, cpu->HL);
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

// LDH a8/ [c] instructions:
static void LDH_p_C_A(CPU *cpu, instruction_T instrc) {     // LDH => Load (High Range). 0xFF the high bit, is fixed.
    printf("LDH A [C]. Called.                          ; LD value pointed by (High Range)[0xFF + C] into A Register\n");

    uint16_t combined_addr = 0xFF00 + cpu->C;
    external_write(combined_addr, cpu->A);

    cpu->PC ++;
}
static void LDH_A_p_C(CPU *cpu, instruction_T instrc) {     // LDH => Load (High Range). 0xFF the high bit, is fixed.
    printf("LDH [C], A. Called.                         ; LD Register A into value pointed by (High Range)[0xFF + C]\n");

    uint16_t combined_addr = 0xFF00 + cpu->C;
    cpu->A = external_read(combined_addr);
    cpu->PC ++;
}
static void LDH_A_p_a8(CPU *cpu, instruction_T instrc) {
    printf("LDH A [a8]. Called.                         ; LD value pointed by (High Range)[0xFF + a8] into A Register\n");

    uint8_t a8 = instrc.operand1;
    uint16_t combined_addr = 0xFF00 + a8;
    cpu->A = external_read(combined_addr);

    cpu->PC +=2;
}
static void LDH_p_a8_A(CPU *cpu, instruction_T instrc) {
    printf("LDH [a8], A. Called.                         ; LD Register A into value pointed by (High Range)[0xFF + a8]\n");
    
    uint8_t a8 = instrc.operand1;
    uint16_t combined_addr = 0xFF00 + a8;
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
        &cpu->H, &cpu->L, NULL, &cpu->A 
    };
    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.

    
    uint16_t add_result = (cpu->A + *reg_table[op_index]);
    uint8_t final_8bit = (uint8_t)add_result;

    (final_8bit == 0) ? set_flag(0) : clear_flag(0);    // Z Flag    
    ((cpu->A & 0x0F) + (op_r8 & 0x0F) > 0x0F) ? set_flag(2) : clear_flag(2); // H Flag
    (add_result > 0xFF) ? set_flag(3) : clear_flag(3); // C Flag
    clear_flag(1);  // N Flag (Subtraction)

    cpu->A = final_8bit;
    cpu->PC ++;

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
    cpu->PC ++ ;

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
static void ADC_A_r8(CPU *cpu, instruction_T instrc) {
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
static void ADC_A_p_HL(CPU *cpu, instruction_T instrc) {
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
static void ADC_A_n8(CPU *cpu, instruction_T instrc) {
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
static void SUB_A_r8(CPU *cpu, instruction_T instrc) {     // Subtract values in a, by 8byte register
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
    printf("SBC A, [HL]. Called, not setup.             ; Subtract value in [HL] (and the carry flag) from Register A\n");

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

    cpu->A = final_8bit;
    cpu->PC += 2;
    
    // Bytes = 2
}

// Increment & Decrement Instructions:
static void INC_r8(CPU *cpu, instruction_T instrc) {    // Increment Register r8
    printf("INC_r8.               ; Registry H ++\n");

    uint8_t op_index = (instrc.opcode >> 3) & 0x07;
    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, NULL, &cpu->A
    };
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

    uint8_t op_index = (instrc.opcode >> 3) & 0x07;
    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, NULL, &cpu->A
    };
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
    
    //external_write(cpu->HL,(cpu->HL ++));
    hl_val ++;
    (hl_val == 0) ? set_flag(0) : clear_flag(0);                // Set/Clear (Z) Zero Flag.
    
    external_write(cpu->HL, hl_val);    // This saves the changes in the location pointed by [HL]

    clear_flag(1);      // Clear subtraction flag (N)
    cpu->PC ++;

    // Bytes = 1
}
static void DEC_p_HL(CPU *cpu, instruction_T instrc) {      // Decrement the Byte insde the location pointed by [HL]
    printf("DEC [HL].               ; EXP: 8bit-- <- [HL]\n");
    // NOTE, technically this is INC_p_r16. But there is not other time that happens. Except for [HL].. SO NVM!

    uint8_t hl_val = external_read(cpu->HL);
    (hl_val == 0) ? set_flag(2) : clear_flag(2);            // Check Val before DEC. If 0, it will need to borrow from bit 4. (So, then set H flag)
    
    //external_write(cpu->HL,(cpu->HL --));
    hl_val --;
    (hl_val == 0) ? set_flag(0) : clear_flag(0);            // Set/Clear (Z) Zero Flag.
    
    external_write(cpu->HL, hl_val);    // This saves the changes in the location pointed by [HL]

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
    
    // If I use table use:
    // uint8_t register_index = (instrc.opcode >> 4) & 0x03;

    switch (instrc.opcode) {
        case 0x03: cpu->BC ++; break;
        case 0x13: cpu->DE ++; break;
        case 0x23: cpu->HL ++; break;
        case 0x33: cpu->SP ++; break;       // Notice this is technically STACK MANIPULATION. INC_SP
    }

    cpu->PC ++;

    // Bytes = 1
    // FLAGS: NONE AFFECTED
}
static void DEC_r16(CPU *cpu, instruction_T instrc) {
    printf("DEC r16.               ; EXP: BC --\n");

    // If I use table use:
    // uint8_t register_index = (instrc.opcode >> 4) & 0x03;
    switch (instrc.opcode) {
        case 0x0B: cpu->BC --; break;
        case 0x1B: cpu->DE --; break;
        case 0x2B: cpu->HL --; break;
        case 0x3B: cpu->SP --; break;       // Notice this is technically STACK MANIPULATION. DEC_SP
    }

    cpu->PC ++;

    // Bytes = 1
    // FLAGS: NONE AFFECTED
}





static void POP_AF(CPU *cpu, instruction_T instrc) {        // Pop register AF from the stack.
    printf("POP AF Called.              ; Populate AF Register from SP.\n");

    uint8_t low_byte = external_read(cpu->SP);
    cpu->SP ++;
    uint8_t high_byte = external_read(cpu->SP);
    cpu->SP ++;

    // Believe how I have the flags setup. this should work.
    //cpu->AF = cnvrt_lil_endian(low_byte, high_byte);

    cpu->AF = (high_byte << 8) | (low_byte & 0xF0);     // Being safe, as the 0-3 bits are 0, and the flag is only 4-7. Previous code might work, but might not also.

    cpu->PC ++;

    /*
    This is roughly equivalent to the following imaginary instructions:
        LD F, [SP]  ; See below for individual flags
        INC SP
        LD A, [SP]
        INC SP
    */
    // Bytes = 1
    /*
        FLAGS:
        Z = Set from bit 7 of the popped low Byte
        N = Set from bit 6 of the popped low Byte
        H = Set from bit 5 of the popped low Byte
        C = Set from bit 4 of the popped low Byte
    */
}
static void POP_r16(CPU *cpu, instruction_T instrc) {       // Pop register r16 from the stack.
    printf("POP r16 Called.                 ; Populate 16byte Register from SP\n");
    
    uint8_t op_index = (instrc.opcode >> 4) & 0x03;
    uint16_t *reg_table[3] = {
        &cpu->BC, &cpu->DE, &cpu->HL
    };    

    uint8_t low_byte = external_read(cpu->SP);
    cpu->SP ++;
    uint8_t high_byte = external_read(cpu->SP);
    cpu->SP ++;

    // Depending on the OPCODE called, will set the specific Register to the Opcode.
    *reg_table[op_index] = cnvrt_lil_endian(low_byte, high_byte);

    cpu->PC ++;

    /*
    This is roughly equivalent to the following imaginary instructions:
        LD LOW(r16), [SP]   ; C, E or L
        INC SP
        LD HIGH(r16), [SP]  ; B, D or H
        INC SP
    */

    // Bytes = 1
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

    printf("Push r16 Called.                ; Push 16 Byte Regiser (BC, DE, HL) to the Stack Pointer SP\n");
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
    // The [remember to come back here] is done by stack pointers / RET (RETurn)
    
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

    printf("What is the Stack Pointer showing? %04X\n", cpu->SP);

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

    printf("RET CC Called.          ; Populate PC from SP, if CC Condition met\n");
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
        printf("RET CC Met. Populating PC from SP\n");
        // TL;DR : Populate the PC from the SP.
        uint8_t low_byte = external_read(cpu->SP);
        cpu->SP ++;
        uint8_t high_byte = external_read(cpu->SP);
        cpu->SP ++;

        cpu->PC = cnvrt_lil_endian(low_byte, high_byte);
    }
    else {
        printf("RET cc Conditions are NOT met. Skipping..\n");
        cpu->PC ++;       // Skip over the entire CALL instruction (Which is 3 bytes in Length)
    }  
    
    // Bytes: 1
    // FLAGS: None affected
}
static void RETI(CPU *cpu, instruction_T instrc) {          // RETurn from subroutine and enable Interupts.
    // This is basically equivalent to executing EI then RET, meaning that IME is set right after this instruction.
    printf("RETI. Called.           ; Set IME Flag (Interupt), Then RETurn / Pop PC from SP\n");

    // Enables interrupts and returns (same as ei immediately followed by ret).


    // TL;DR :
    // Set IME flag to 1 - enable
    cpu->IME = 1;
    // Populate the PC from the SP.
    uint8_t low_byte = external_read(cpu->SP);
    cpu->SP ++;
    uint8_t high_byte = external_read(cpu->SP);
    cpu->SP ++;

    cpu->PC = cnvrt_lil_endian(low_byte, high_byte);


    // FLAGS: None affected --> In Register F that is.
}
static void RST_vec(CPU *cpu, instruction_T instrc) {       // Runs Basically CALL, then Jumps to a specific Vector address (basically a predetermined offset)
    printf("RST_vec. Called.                ; Push PC to Stack Pointer (SP), then Jump to specific Vector location\n");

    uint8_t vec_table[8] = {
        0x00, 0x08, 0x10, 0x18, 
        0x20, 0x28, 0x30, 0x38
    };

    uint8_t op_index = (instrc.opcode >> 3) & 0x07;
    uint16_t jump_addr = vec_table[op_index];

    uint16_t pc_loc = cpu->PC;

    uint8_t split_addr_LOW = pc_loc & 0xFF;
    uint8_t split_addr_HIGH = (pc_loc >> 8) & 0xFF;

    // Push return address onto stack (high byte first)
    cpu->SP --; 
    external_write(cpu->SP, split_addr_HIGH);
    cpu->SP --;
    external_write(cpu->SP, split_addr_LOW);

    cpu->PC = jump_addr;





    // Main difference, is like the op_codes that are LD A, r8. It knows by the opcode, which register to load the data from. 
    // RST Vector is a specific Vector 
    // An RST vector (0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, and 0x38).

    // FLAGS: None affected
}





// -----------------------------------------------
/// SECTION:
// Bitwise, AND, OR XOR instructions.

static void AND_A_r8(CPU *cpu, instruction_T instrc) {      // Set A to the bitwise AND between the value in r8 and A
    printf("AND A, r8. Called.          ; Result = Set bit to 1 or 0. For each indvidual bit. Bit 0 through Bit 7.\n");
    // This is a BITWISE AND. Which is to say. It returns an 8bit value. Where each individual val of 1 bits. Match in both 8bit variable.
    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, NULL, &cpu->A 
    };

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
    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, NULL, &cpu->A 
    };

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

    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, 
        &cpu->H, &cpu->L, NULL, &cpu->A 
    };

    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t XOR_result = (cpu->A ^ *reg_table[op_index]);

    cpu->A = XOR_result;


    (XOR_result == 0) ? set_flag(0) : clear_flag(0);    // Set if Zero (Z Flag)
    clear_flag(1);  // ALways cleared
    clear_flag(2);  // Always cleared
    clear_flag(3);  // Always cleared

    cpu->PC ++;

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
    clear_flag(2);  // H Always cleared
    clear_flag(3);  // C Always cleared

    cpu->PC += 2;

    // Bytes = 2
}

// CPL Instruction. Like a bitwse NOT
static void CPL(CPU *cpu, instruction_T instrc) {           // ComPLement accumulator (A = ~A); also called bitwise NOT a
    printf("CPL. Called.                    ; ComPLement accumulator, \n");
    cpu->A = ~cpu->A;   // A = NOT A. Basically flip the bits around.
    
    set_flag(1);    // N Flag Set (Subtraction Flag)
    set_flag(2);    // H Flag Set (Half-Carry Flag)         -- This isn't really true, but mimics what original GB hardware did.

    cpu->PC ++;
    // Bytes = 1
    // Don't clear Z or C flags.
    // - N H -
}




// -----------------------------------------------
/// SECTION:
// Bitshift and ROTATE instructions

// NON-PREFIXED
static void RRA(CPU *cpu, instruction_T instrc) {           // Rotate register A -> Right. Through the carry flag. b8 ---> b0 <> [carry flag]
    printf("RRA. Called.            ; Rotate Register A RIGHT --> through Flag C.\n");

    uint8_t a_reg = cpu->A;
    uint8_t carry_in = (cpu->F & FLAG_C) ? 1 : 0;
    uint8_t carry_out = (cpu->A & 0x1);
    uint8_t rotated_a = (a_reg >> 1) | (carry_in << 7);     // Rotate Right, Set bit 7
    cpu->A = rotated_a;

    (carry_out) ? set_flag(3) : clear_flag(3);  
    clear_flag(0);
    clear_flag(1);
    clear_flag(2);

    cpu->PC++;
    // Bytes = 1
}

static void RRCA(CPU *cpu, instruction_T instrc) {          // Rotate Register A right. (WITHOUT CARRY)
    printf("RRCA Called.                    ; Rotate Register A Right Without the carry flag input\n");

    uint8_t a_reg = cpu->A;
    uint8_t carry_out = (a_reg & 0x1);
    uint8_t rotated_a = (a_reg >> 1) | (carry_out << 7);    // Rotate Right, Set bit 7
    cpu->A = rotated_a;

    (carry_out) ? set_flag(3) : clear_flag(3);
    clear_flag(0);  // Z Flag
    clear_flag(1);  // N (Subtraction) Flag
    clear_flag(2);  // H (Half Carry) Flag

    cpu->PC++;
    // Bytes = 1
}

static void RLA(CPU *cpu, instruction_T instrc) {           // Rotate Register A Left. Through the carry flag <---
    printf("RLA Called.                     ; Rotate Register A Left (through carry flag).\n");

    uint8_t a_reg = cpu->A;
    uint8_t carry_in = (cpu->F & FLAG_C) ? 1 : 0;
    uint8_t carry_out = ((a_reg >> 7) & 0x1);               // Isolate and extract bit 7
    uint8_t rotated_a = (a_reg << 1) | carry_in;            // Shift left, set bit 0
    cpu->A = rotated_a;

    (carry_out) ? set_flag(3) : clear_flag(3);
    clear_flag(0);  // Z Flag
    clear_flag(1);  // N (Subtraction) Flag
    clear_flag(2);  // H (Half Carry) Flag

    cpu->PC++;
    // Bytes = 1
}

static void RLCA(CPU *cpu, instruction_T instrc) {          // Rotate Register A left. (without carry flag input)
    printf("RLCA Called.                    ; Rotate Register A Left, wont use carry flag input\n");
    
    uint8_t a_reg = cpu->A;
    uint8_t carry_out = ((a_reg >> 7) & 0x1);           // Isolate and extract bit 7
    uint8_t rotated_a = (a_reg << 1) | carry_out;       // Shift left, set bit 0
    cpu->A = rotated_a;
    
    (carry_out) ? set_flag(3) : clear_flag(3);
    clear_flag(0);  // Z Flag
    clear_flag(1);  // N (Subtraction) Flag
    clear_flag(2);  // H (Half Carry) Flag

    cpu->PC++;
    // Bytes = 1
}







// ------------------------------------------------------
//
///                    NOTICE:
// ==---- Starting PREFIXED OP Code instructions ----==
//
// ------------------------------------------------------









static void RL_r8(CPU *cpu, instruction_T instrc) {         // Rotate Byte in Register r8 left, through the carry flag. <---
    printf("RL r8. Called.                  ; Rotate r8 Left through carry flag.\n");

    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, &cpu->H, &cpu->L, NULL, &cpu->A
    };
    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t r8_reg = *reg_table[op_index];

    uint8_t carry_in = (cpu->F & FLAG_C) ? 1 : 0;
    uint8_t carry_out = ((r8_reg >> 7) & 0x1);              // Isolate and extract bit 7
    uint8_t rotated_r8 = (r8_reg << 1) | carry_in;          // Shift left, set bit 0
    
    *reg_table[op_index] = rotated_r8;                      // Set register to value.

    (carry_out) ? set_flag(3) : clear_flag(3);      // C Flag
    (rotated_r8) ? set_flag(0) : clear_flag(0);     // Z Flag
    clear_flag(1);  // N (Subtraction) Flag
    clear_flag(2);  // H (Half Carry) Flag

    cpu->PC ++;
    // PREFIXED => Yes, 2 Bytes (TOTAL), already advanced once.

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RL_p_HL(CPU *cpu, instruction_T instrc) {       // Rotate the byte pointed to by HL left, through the carry flag. <---
    printf("RL [HL]. Called.                  ; Rotate value pointed by [HL] Left through carry flag.\n");

    uint8_t hl_val = external_read(cpu->HL);
    uint8_t carry_in = (cpu->F & FLAG_C) ? 1 : 0;
    uint8_t carry_out = ((hl_val >> 7) & 0x1);              // Isolate and extract bit 7
    uint8_t rotated_hl = (hl_val << 1) | carry_in;          // Shift byte left, set bit 0 value.
    
    external_write(cpu->HL, rotated_hl);    // Set [HL] value

    (carry_out) ? set_flag(3) : clear_flag(3);          // C Flag
    (rotated_hl) ? set_flag(0) : clear_flag(0);     // Z Flag
    clear_flag(1);  // N (Subtraction) Flag
    clear_flag(2);  // H (Half Carry) Flag

    cpu->PC ++;
    // PREFIXED => Yes, 2 Bytes (TOTAL), already advanced once.


    // FLAGS: See RL_r8
}

static void RLC_r8(CPU *cpu, instruction_T instrc) {        // Rotate Registers r8 Left. <--- (without Carry flag input)
    printf("RLC r8. Called.                   ; Rotate r8 Left (without carry flag input).\n");
    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, &cpu->H, &cpu->L, NULL, &cpu->A
    };
    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t r8_reg = *reg_table[op_index];
    uint8_t carry_out = ((r8_reg >> 7) & 0x1);              // Isolate and extract bit 7
    uint8_t rotated_r8 = (r8_reg << 1) | carry_out;         // Shift left, set bit 0 value.

    *reg_table[op_index] = rotated_r8;                      // Set r8 Register.

    (carry_out) ? set_flag(3) : clear_flag(3);           // C Flag
    (rotated_r8) ? set_flag(0) : clear_flag(0);          // Z Flag
    clear_flag(1);  // N (Subtraction) Flag
    clear_flag(2);  // H (Half Carry) Flag

    cpu->PC ++;
    // PREFIXED => Yes, 2 Bytes (TOTAL), already advanced once.

    /*
        FLAGS:
        Z = 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RLC_p_HL(CPU *cpu, instruction_T instrc) {      // Rotate the byte pointed to by [HL] left. <--- (without Carry flag input)
    printf("RLC [HL]. Called.                   ; Rotate valued pointed by [HL] Left (without carry flag input).\n");

    uint8_t hl_val = external_read(cpu->HL);
    uint8_t carry_out = ((hl_val >> 7) & 0x1);              // Isolate and extract bit 7
    uint8_t rotated_hl = (hl_val << 1) | carry_out;         // Shift Byte Left, set bit 0

    external_write(cpu->HL, rotated_hl);    // Set [HL] value

    (carry_out) ? set_flag(3) : clear_flag(3);           // C Flag
    (rotated_hl) ? set_flag(0) : clear_flag(0);          // Z Flag
    clear_flag(1);  // N (Subtraction) Flag
    clear_flag(2);  // H (Half Carry) Flag

    cpu->PC ++;
    // PREFIXED => Yes, 2 Bytes (TOTAL), already advanced once.

    // FLAGS: See RLC_r8
}

// PREFIXED Rotate Right Instructions:
static void RR_r8(CPU *cpu, instruction_T instrc) {         // Rotate Register r8 Right. Through the carry flag. -->
    printf("RR r8. Called.                      ; Rotate r8 Right through carry flag\n");

    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, &cpu->H, &cpu->L, NULL, &cpu->A
    };
    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t r8_reg = *reg_table[op_index];

    uint8_t carry_in = (cpu->F & FLAG_C) ? 1 : 0;
    uint8_t carry_out = (r8_reg & 0x1);
    uint8_t rotated_r8 = (r8_reg >> 1) | (carry_in << 7);   // Shift Byte right, set bit 7
    
    *reg_table[op_index] = rotated_r8;                      // Set register to value.

    (carry_out) ? set_flag(3) : clear_flag(3);      // C Flag
    (rotated_r8) ? set_flag(0) : clear_flag(0);     // Z Flag
    clear_flag(1);  // N (Subtraction) Flag
    clear_flag(2);  // H (Half Carry) Flag

    cpu->PC ++;
    // PREFIXED => Yes, 2 Bytes (TOTAL), already advanced once.

    /*
        FLAGS:
        Z = 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RR_p_HL(CPU *cpu, instruction_T instrc) {       // Rotate the byte pointed to by [HL] Right. Through the carry flag. -->
    printf("rR [HL]. Called         ; Rotate value pointed by [HL] Right through carry flag.\n");

    uint8_t hl_val = external_read(cpu->HL);
    uint8_t carry_in = (cpu->F & FLAG_C) ? 1 : 0;
    uint8_t carry_out = (hl_val & 0x1);
    uint8_t rotated_hl = (hl_val >> 1) | (carry_in << 7);   // Shift Byte right, set bit 7

    external_write(cpu->HL, rotated_hl);    // Set [HL] value.

    (carry_out) ? set_flag(3) : clear_flag(3);           // C Flag
    (rotated_hl) ? set_flag(0) : clear_flag(0);          // Z Flag
    clear_flag(1);  // N (Subtraction) Flag
    clear_flag(2);  // H (Half Carry) Flag

    cpu->PC ++;
    // PREFIXED => Yes, 2 Bytes (TOTAL), already advanced once.

    // FLAGS: see RR_r8
}
static void RRC_r8(CPU *cpu, instruction_T instrc) {        // Rotate Register r8 Right. --> (Without carry flag input)
    printf("RRC r8. Called.                     ; Rotate r8, without carry flag input\n");

    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, &cpu->H, &cpu->L, NULL, &cpu->A
    };
    uint8_t op_index = (instrc.opcode & 0x07);
    uint8_t r8_reg = *reg_table[op_index];
    uint8_t carry_out = (r8_reg & 0x1);
    uint8_t rotated_r8 = (r8_reg >> 1) | (carry_out << 7);      // Shift Byte Right, set bit 7

    *reg_table[op_index] = rotated_r8;                          // Set r8 Register.

    (carry_out) ? set_flag(3) : clear_flag(3);           // C Flag
    (rotated_r8) ? set_flag(0) : clear_flag(0);          // Z Flag
    clear_flag(1);  // N (Subtraction) Flag
    clear_flag(2);  // H (Half Carry) Flag

    cpu->PC ++;
    // PREFIXED => Yes, 2 Bytes (TOTAL), already advanced once.

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RRC_p_HL(CPU *cpu, instruction_T instrc) {      // Rotate the value pointed to by [HL] Right. -->  (Without carry flag input)
    printf("RRC [HL]. Called.                   ; Rotate value pointed by [HL] Right, without carry flag input");


    uint8_t hl_val = external_read(cpu->HL);
    uint8_t carry_out = (hl_val & 0x1);
    uint8_t rotated_hl = (hl_val >> 1) | (carry_out << 7);      // Shift Byte Right, set bit 7

    external_write(cpu->HL, rotated_hl);        // Set [HL] value

    (carry_out) ? set_flag(3) : clear_flag(3);           // C Flag
    (rotated_hl) ? set_flag(0) : clear_flag(0);          // Z Flag
    clear_flag(1);  // N (Subtraction) Flag
    clear_flag(2);  // H (Half Carry) Flag

    cpu->PC ++;
    // PREFIXED => Yes, 2 Bytes (TOTAL), already advanced once.

    // FLAGS: See RRC r8
}




// PREFIXED SHIFT left & right Arithmetically.
static void SLA_r8(CPU *cpu, instruction_T instrc){         // Shift Left Arithmetically Register r8.  <--
    printf("SLA r8. Called, not setup.\n");
    printf("%sPANIC HALTING%s\n", KRED, KNRM);
    cpu_status.panic = 1;
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
    printf("%sPANIC HALTING%s\n", KRED, KNRM);
    cpu_status.panic = 1;
    // Flags: SEE SLA r8
}
static void SRA_r8(CPU *cpu, instruction_T instrc) {        // Shift Right Arithmetically Register r8. -->
    printf("SRA r8. Called, not setup.\n");
    printf("%sPANIC HALTING%s\n", KRED, KNRM);
    cpu_status.panic = 1;

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
    printf("%sPANIC HALTING%s\n", KRED, KNRM);
    cpu_status.panic = 1;
    // Flags: See SRA_r8
}
static void SRL_r8(CPU *cpu, instruction_T instrc) {        // Shift Right Logically Register r8. -->
    printf("SRL r8. Called, not setup.\n");
    printf("%sPANIC HALTING%s\n", KRED, KNRM);
    cpu_status.panic = 1;
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
    printf("%sPANIC HALTING%s\n", KRED, KNRM);
    cpu_status.panic = 1;
    // FLAGS: See SRL_r8
}


// PREFIXED Swap instructions
static void SWAP_r8(CPU *cpu, instruction_T instrc) {       // Swap the upper 4 bits in register r8 and the lower 4 ones. X::Y == Y::X
    printf("SWAP r8. Called, not setup.\n");
    printf("%sPANIC HALTING%s\n", KRED, KNRM);
    cpu_status.panic = 1;
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
    printf("%sPANIC HALTING%s\n", KRED, KNRM);
    cpu_status.panic = 1;
    // FLAGS: See SWAP_r8

}

// PREFIXED Bit Flag instructions
static void BIT_u3_r8(CPU *cpu, instruction_T instrc) {     // Test bit u3 in register r8 set the zero flag if bit not set
    printf("BIT u3, r8. Called.                     ; Check r8 bit at index u3. Set/ Clear Z flag accordingly.\n");

    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, &cpu->H, &cpu->L, NULL, &cpu->A
    };

    uint8_t u3_num = (instrc.opcode >> 3);       // Acts as a divide by 8
    uint8_t reg_index = (instrc.opcode & 0x07);  // Provides 0-7 Index to Match Register
    uint8_t r8_reg = *reg_table[reg_index];

    uint8_t get_state = ((r8_reg >> u3_num) & 1);
    (get_state) ? clear_flag(0) : set_flag(0); // Z flag, Set if specific r8 bit NOT set.

    clear_flag(1);  // Always clear N Flag. (Just GB CPU logic)
    set_flag(2);    // Always SET h Flag.   (Just GB CPU logic)
    // C flag unaffected.

    cpu->PC ++;
    // Bytes = 2 (CB logic already advanced once)
}
static void BIT_u3_p_HL(CPU *cpu, instruction_T instrc) {   // Test bit u3 in the byte pointed by HL, set the flag if bit not set
    printf("BIT u3, [HL]. Called.                      ; Check [HL] bit at index u3. Set/ Clear Z flag accordingly.\n");

    uint8_t u3_num = (instrc.opcode >> 3);                  // Acts as a divide by 8
    uint8_t hl_val = external_read(cpu->HL);

    uint8_t get_state = ((hl_val >> u3_num) & 1);
    (get_state) ? clear_flag(0) : set_flag(0); // Z flag, Set if specific r8 bit NOT set.

    clear_flag(1);  // Always clear N Flag.
    set_flag(2);    // Always SET h Flag.
    // C flag unaffected.

    cpu->PC ++;
    // Bytes = 2 (CB logic already advanced once)
}

// PREFIXED RES Instructions (Set a specific bit to 0?)
static void RES_u3_r8(CPU *cpu, instruction_T instrc) {     // Set bit u3 in register r8 to 0. Bit 0 is the rightmost one, bit 7 the leftmost one
    printf("RES u3, r8. Called.                         ; Set bit to 0, at index u3 in r8 Register.\n");
    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, &cpu->H, &cpu->L, NULL, &cpu->A
    };

    uint8_t u3_num = (instrc.opcode >> 3);       // Acts as a divide by 8
    uint8_t reg_index = (instrc.opcode & 0x07);  // Provides 0-7 Index to Match Register

    *reg_table[reg_index] &= ~(1 << u3_num);     // Set bit at u3 index to 0.

    cpu->PC ++;
    // Bytes = 2 (CB logic already advanced once)
    // FLAGS: None affected
}
static void RES_u3_p_HL(CPU *cpu, instruction_T instrc) {   // Set bit u3 in the byte pointed by HL to 0. Bit 0 is the rightmost one, bit 7 the leftmost one
    printf("RES u3, [HL]. Called.                      ; Set bit to 0, at index u3, in value [HL].\n");

    uint8_t u3_num = (instrc.opcode >> 3);       // Acts as a divide by 8
    uint8_t hl_val = external_read(cpu->HL);
    
    hl_val &= ~(1 << u3_num);                   // Set bit at u3 index to 0.
    external_write(cpu->HL, hl_val);

    cpu->PC ++;
    // Bytes = 2 (CB logic already advanced once)
    // FLAGS: None affected
}
// PREFIXED SET instructions (Set bit to 1, at u3 Index?)
static void SET_u3_r8(CPU *cpu, instruction_T instrc) {     // Set bit u3 in register r8 to 1. Bit 0 is the rightmost one, bit 7 the leftmost one
    printf("SET u3, r8. Called.                         ; Set bit to 1, at index u3 in r8 Register.\n");

    uint8_t *reg_table[8] = {
        &cpu->B, &cpu->C, &cpu->D, &cpu->E, &cpu->H, &cpu->L, NULL, &cpu->A
    };

    uint8_t u3_num = (instrc.opcode >> 3);       // Acts as a divide by 8
    uint8_t reg_index = (instrc.opcode & 0x07);  // Provides 0-7 Index to Match Register

    *reg_table[reg_index] |= (1 << u3_num);     // Sets a single bit to 1, in the Index of r8 Register.

    cpu->PC ++;
    // Bytes = 2 (CB logic already advanced once)
    // FLAGS: None affected
}
static void SET_u3_p_HL(CPU *cpu, instruction_T instrc) {   // Set bit u3 in the byte pointed by HL to 1. Bit 0 is the rightmost one, bit 7 the leftmost one
    printf("SET u3, [HL]. Called.                      ; Set bit to 1, at index u3, in value [HL]..\n");

    uint8_t u3_num = (instrc.opcode >> 3);       // Acts as a divide by 8
    uint8_t hl_val = external_read(cpu->HL);
    hl_val |= (1 << u3_num);

    external_write(cpu->HL, hl_val);

    cpu->PC ++;
    // Bytes = 2 (CB logic already advanced once)
    // FLAGS: None affected
}






/// NOTICE:
// CB Prefixed BIT, RES, SET, Handlers.

// Handlers help point to the correct function (RES X, r8), (RES X, [HL]).. 
// AND which parameters for each Function:
// BIT 0, B. or BIT 4, H. 
// RES 4, C. or Set 7 C.
static void CB_BIT_Handler(CPU *cpu, instruction_T instrc){
    printf("CB BIT handler called.                      ; Calling Sub-Instruction\n");

    ((instrc.opcode & 0x07) == 0x06) ? BIT_u3_p_HL(cpu, instrc) : BIT_u3_r8(cpu, instrc);
}

static void CB_RES_Handler(CPU *cpu, instruction_T instrc){
    printf("CB RES handler called.                      ; Calling Sub-Instruction\n");
    
    ((instrc.opcode & 0x07) == 0x06) ? RES_u3_p_HL(cpu, instrc) : RES_u3_r8(cpu, instrc);
}

static void CB_SET_Handler(CPU *cpu, instruction_T instrc){
    printf("CB SET handler called.                      ; Calling Sub-Instruction\n");

    ((instrc.opcode & 0x07) == 0x06) ? SET_u3_p_HL(cpu, instrc) : SET_u3_r8(cpu, instrc);
}

static opcode_t *cb_opcodes[256] = {
    [0x00 ... 0x05] = RLC_r8, [0x06] = RLC_p_HL, [0x07] = RLC_r8, [0x08 ... 0x0D] = RRC_r8, [0x0E] = RRC_p_HL, [0x0F] = RRC_r8,
    [0x10 ... 0x15] = RL_r8,  [0x16] = RL_p_HL,  [0x17] = RL_r8,  [0x18 ... 0x1D] = RR_r8,  [0x1E] = RR_p_HL,  [0x1F] = RR_r8,
    [0x20 ... 0x25] = RL_r8,  [0x26] = RL_p_HL,  [0x27] = RL_r8,  [0x28 ... 0x2D] = RR_r8,  [0x2E] = RR_p_HL,  [0x2F] = RR_r8,
    [0x30 ... 0x35] = RL_r8,  [0x36] = RL_p_HL,  [0x37] = RL_r8,  [0x38 ... 0x3D] = RR_r8,  [0x3E] = RR_p_HL,  [0x3F] = RR_r8,
    [0x40 ... 0x7F] = CB_BIT_Handler,
    [0x80 ... 0xBF] = CB_RES_Handler,
    [0xC0 ... 0xFF] = CB_SET_Handler,
};

static void CB_PREFIX(CPU *cpu, instruction_T instrc) {
    printf("CB PREFIX Called.               ; Load next Byte, to call Prefixed OPCODE\n");
    // RLC, RL, RRC, RR, SRA, SRL, SWAP, BIT, RES, SET

    // Advance the PC, and read it at the same time. 
    uint8_t prefixed_opcode = external_read(cpu->PC++);       // Read CB Opcoad at PC++ location. 
    printf("%sCB OPCODE:=[0x%02X]%s\n", KBLU, prefixed_opcode, KNRM);

    cb_opcodes[prefixed_opcode](cpu, instrc);
    printf("%sCB Block Finished.%s\n", KBLU, KNRM);
}





static opcode_t *opcodes[256] = {
/*  ---> X0, X1, X2, X3, X4 ... XB .. XF etc */
/* 0X */ NOP,        LD_r16_n16,    LD_p_r16_A, INC_r16,  INC_r8,     DEC_r8,  LD_r8_n8,   RLCA,     /* || */ LD_p_a16_SP,  ADD_HL_r16, LD_A_p_r16,  DEC_r16,   INC_r8,     DEC_r8,   LD_r8_n8,   RRCA,
/* 1X */ STOP,       LD_r16_n16,    LD_p_r16_A, INC_r16,  INC_r8,     DEC_r8,  LD_r8_n8,   RLA,      /* || */ JR_e8,        ADD_HL_r16, LD_A_p_r16,  DEC_r16,   INC_r8,     DEC_r8,   LD_r8_n8,   RRA,
/* 2X */ JR_cc_e8,   LD_r16_n16,    LD_p_HLI_A, INC_r16,  INC_r8,     DEC_r8,  LD_r8_n8,   DAA,      /* || */ JR_cc_e8,     ADD_HL_r16, LD_A_p_HLI,  DEC_r16,   INC_r8,     DEC_r8,   LD_r8_n8,   CPL,
/* 3X */ JR_cc_e8,   LD_r16_n16,    LD_p_HLI_A, INC_r16,  INC_p_HL,    DEC_p_HL, LD_p_HL_n8, SCF,      /* || */ JR_cc_e8,     ADD_HL_r16, LD_A_p_HLD,  DEC_r16,   INC_r8,     DEC_r8,   LD_r8_n8,   CCF,
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

