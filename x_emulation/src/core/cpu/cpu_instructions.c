#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>

#include <stdbool.h> // Remove later

#include "../gb.h"
#include "cpu.h"
#include "cpu_instructions.h"


extern FILE *debug_dump_file;

// The reference for the pointers to functions, to select instruction based on OP_CODE
typedef void opcode_t(GB *gb, CPU *cpu, instruction_T instruction);

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

////trace_general_line(0x00, 0x00, 0x00, 0x00, 0, "txt", 2);


void set_cpu_flag(GB *gb, uint8_t flag_hex) {
    gb->cpu.reg.F |= flag_hex;
}

void clear_cpu_flag(GB *gb, uint8_t flag_hex) {
    gb->cpu.reg.F &= ~flag_hex;
}

void tggle_cpu_flag(GB *gb, uint8_t flag_hex) {
    gb->cpu.reg.F ^= flag_hex;
}

/// CONSIDER: Using a ADDITION_8bit Set flags... Might make things easier...
// void set_add_flags_8bit(uint8_t a, uint8_t b, int affect_Z, int affect_N) {
//     // Clear the bits we're modifying
//     if (affect_Z) clear_flag(0);  // Z always cleared on LD HL, SP+e8
//     if (affect_N) clear_flag(1);  // N always cleared on addition

//     // Half carry: bit 3 overflow into bit 4
//     if (((a & 0xF) + (b & 0xF)) > 0xF)
//         set_flag(2);  // H
//     else
//         clear_flag(2);

//     // Full carry: bit 7 overflow into bit 8
//     if ((uint16_t)a + (uint16_t)b > 0xFF)
//         set_flag(3);  // C
//     else
//         clear_flag(3);
// }






// INTERUPT Instructions:
// HALT!
static void HALT(GB *gb, CPU *cpu, instruction_T instruction) {     // Halt - Set cpu halt flag.
    cpu->state.halt = 1;
    gb->cpu.reg.PC += 1;
}
static void DI(GB *gb, CPU *cpu, instruction_T instruction) {        // DI - Disables interrupt Handling (Flag set to := 0)
    cpu->state.IME = 0; // Clear interrupt flag.
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // t-cycles = 4
    // Bytes = 1
    // No flags affected. (Inside F Flags)
}

static void EI(GB *gb, CPU *cpu, instruction_T instruction) {        // EI - Enables interrupt Handling (Flag set to := 1) - With Delay.
    // This uses a delay logic of 2 (counting down after instruction finishes)
    // This ensures IME is set ONLY after the current instruction (EI) AND the subsequent operation both have finished executing.

    // TL;DR Why? The GB gives an one-additional-instruction grace period before the interupt is executed.

    cpu->state.IME_delay = 2;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // t-cycles = 4
    // Bytes = 1
    // No flags affected. (Inside F Flags)
}




// MISC Instructions:
// NOP - No operation
static void NOP(GB *gb, CPU *cpu, instruction_T instruction) {                    // Placeholder..
    // DO NOTHING, just advance PC and cycles
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

}
// STOP
static void STOP(GB *gb, CPU *cpu, instruction_T instruction) {      // Unsure, might be like Pause.
    printf("STOP Called.. Waiting until Interrupt.\n");
    gb->cpu.reg.PC += 1;
    cpu->state.stop = 1;
    gb->cpu.cycle = 4;

    //gb->panic = 1;
}
// DAA - After binary arithmetic, A may no longer be valid BCD (Binary-Coded Decimal). DAA => corrects it.
static void DAA(GB *gb, CPU *cpu, instruction_T instruction) {
    uint8_t reg_a = gb->cpu.reg.A;
    uint8_t correction = 0x00;

    if (gb->cpu.reg.F & FLAG_N) {  // Previous instruction was Subtraction.
        if (gb->cpu.reg.F & FLAG_H) {  // H set => subtract 0x06
            correction |= 0x06;
        }
        if (gb->cpu.reg.F & FLAG_C) { // C set => subtract 0x60
            correction |= 0x60;
        }

        // C is preserved after subtraction.
        reg_a -= correction;
    }
    else {  // Previous instruction was Addition.
        if ((gb->cpu.reg.F & FLAG_H) || (reg_a & 0x0F) > 0x09) { // H set, or BCD-low nib exeeded 0x09; add 0x06
            correction |= 0x06;
        }
        if ((gb->cpu.reg.F & FLAG_C) || (reg_a) > 0x99) { // C set, or BCD exeeded 0x99; add 0x60
            correction |= 0x60;
        }

        // Set C flag only if correction has 0x60, and was needed, otherwise clear.
        (correction & 0x60) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);
        reg_a += correction;
    }
    (reg_a == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    clear_cpu_flag(gb, FLAG_H); // H flag used for "corrections", but always cleared when done.
    // C flag preserved for Sub / Set or cleared in Add logic.

    gb->cpu.reg.A = (uint8_t)reg_a;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // bytes = 1
    // t-cycles = 4

    /*
        FLAGS:
        Z = Set if result is 0
        H = 0 - always cleared
        C = Set / cleared depending on operations.
    */
}
// BLANK
static void BLANK(GB *gb, CPU *cpu, instruction_T instruction) {      // Do nothing, basically NOP, but any call to this should cause fault.
    // DO NOTHING - Not even any command.
    // This shouldn't Even be called.

    trace_general_line(gb->instruction.opcode, gb->cpu.cycle, gb->cpu.reg.F, gb->cpu.reg.PC, 0, "Blank Called. FATAL", 2);

    printf("%sBLANK Called, This should never be called. Panic.%s\n", KRED, KNRM);
    printf("FATAL ERROR: PC=%04X, OPCODE=[%02X]\n", gb->cpu.reg.PC, instruction.opcode);
    gb->panic = 1;
}

// Carry Flag Instructions:
static void CCF(GB *gb, CPU *cpu, instruction_T instruction) {           // Complement Carry Flag
    // Invert C flag. Set if unset. Clear if set.

    // Z untouched.
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);
    (gb->cpu.reg.F & FLAG_C) ? clear_cpu_flag(gb, FLAG_C) : set_cpu_flag(gb, FLAG_C);    // Invert the C flag.

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // bytes = 1
    // t-cycles = 4

    /*
        FLAGS:
        Z = --
        N = 0
        H = 0
        C = Inverted
    */
}
static void SCF(GB *gb, CPU *cpu, instruction_T instruction) {           // Set Carry Flag
    // Z untouched.
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);
    set_cpu_flag(gb, FLAG_C);
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // Bytes = 1
    // t-cycles = 4

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
static void LD_r8_n8(GB *gb, CPU *cpu, instruction_T instruction) {
    uint8_t op_index = (instruction.opcode >> 3) & 0x07;      // <-- This style needed for left, right. DOWN left right, DOWN etc
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C,
        &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L,
        NULL, &gb->cpu.reg.A
    };
    *reg_table[op_index] = instruction.operand1;

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycles = 8
    // No flags affected.
}
static void LD_r16_n16(GB *gb, CPU *cpu, instruction_T instruction) {
    uint8_t op_index = (instruction.opcode >> 4) & 0x03; // <- This style needed for straight down op codes.
    uint16_t *reg_16table[4] = {
        &gb->cpu.reg.BC,
        &gb->cpu.reg.DE,
        &gb->cpu.reg.HL,
        &gb->cpu.reg.SP
    };
    *reg_16table[op_index] = cnvrt_lil_endian(instruction.operand1, instruction.operand2);

    gb->cpu.reg.PC += 3;
    gb->cpu.cycle = 12;

    // Bytes: 3
    // t-cycle = 12
    // Flags: None Affected
}
static void LD_p_HL_n8(GB *gb, CPU *cpu, instruction_T instruction) {       // Copy data from n8, into where HL is being pointed to
    external_write(gb, gb->cpu.reg.HL, instruction.operand1);
    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 12;

    // Bytes = 2
    // t-cycles = 12
    // No flags Affected
}



// -----------------------------------------------
/// SECTION:
// LD Acculator (A Register) instructions

static void LD_p_r16_A(GB *gb, CPU *cpu, instruction_T instruction) {
    switch (instruction.opcode) {
        case 0x02:
            // LD [BC], A
            external_write(gb, gb->cpu.reg.BC, gb->cpu.reg.A);
            break;
        case 0x12:
            // LD [DE], A
            external_write(gb, gb->cpu.reg.DE, gb->cpu.reg.A);
            break;
    }
    gb->cpu.reg.PC++;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycles = 8
}
static void LD_A_p_r16(GB *gb, CPU *cpu, instruction_T instruction) {
    uint8_t read_val;
    switch (instruction.opcode) {
        case 0x0A:
            // LD A, [BC]
            read_val = external_read(gb, gb->cpu.reg.BC);
            gb->cpu.reg.A = external_read(gb, gb->cpu.reg.BC);
            break;
        case 0x1A:
            // LD A, [DE]
            read_val = external_read(gb, gb->cpu.reg.DE);
            gb->cpu.reg.A = external_read(gb, gb->cpu.reg.DE);
            break;
    }
    // No [HL] as instruction is HL+ HL- (Different Instruction)
    gb->cpu.reg.PC++;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycles = 8
}
// LD/ Load (A) with Increment and Decrement to HL after.
static void LD_p_HLI_A(GB *gb, CPU *cpu, instruction_T instruction) {   // Copy value in A into byte pointed by [HL] then increment HL.
    uint8_t a_val = gb->cpu.reg.A;
    external_write(gb, gb->cpu.reg.HL, a_val);
    gb->cpu.reg.HL ++;     // Increment HL after
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    //trace_general_line(gb->instruction.opcode, gb->cpu.cycle, gb->cpu.reg.F, gb->cpu.reg.PC, a_val, "LD [HLI], A", 2);
    //trace_reg_state(gb->instruction.opcode, gb->cpu.reg.PC, gb->cpu.reg.AF, gb->cpu.reg.BC, gb->cpu.reg.DE, gb->cpu.reg.HL, gb->cpu.reg.SP, 2);

    // Bytes = 1
    // t-cycles = 8
}
static void LD_p_HLD_A(GB *gb, CPU *cpu, instruction_T instruction) {   // Copy value in A into byte pointed by [HL] then decrement HL.
    uint8_t a_val = gb->cpu.reg.A;
    external_write(gb, gb->cpu.reg.HL, a_val);
    gb->cpu.reg.HL --;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    //trace_general_line(gb->instruction.opcode, gb->cpu.cycle, gb->cpu.reg.F, gb->cpu.reg.PC, a_val, "LD [HLD], A", 2);
    //trace_reg_state(gb->instruction.opcode, gb->cpu.reg.PC, gb->cpu.reg.AF, gb->cpu.reg.BC, gb->cpu.reg.DE, gb->cpu.reg.HL, gb->cpu.reg.SP, 2);

    // Bytes = 1
    // t-cycles = 8
}
static void LD_A_p_HLI(GB *gb, CPU *cpu, instruction_T instruction) {   // LD A, [HL+], Copy value pointed from HL, into A register, then Increment HL
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    gb->cpu.reg.A = hl_val;
    gb->cpu.reg.HL ++;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycles = 8
    // Flags not affected
}
static void LD_A_p_HLD(GB *gb, CPU *cpu, instruction_T instruction) {
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    gb->cpu.reg.A = hl_val;
    gb->cpu.reg.HL --;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycles = 8
    // Flags not affected
}
static void LD_p_a16_A(GB *gb, CPU *cpu, instruction_T instruction) {
    uint16_t a16 = cnvrt_lil_endian(instruction.operand1, instruction.operand2);
    external_write(gb, a16, gb->cpu.reg.A);
    gb->cpu.reg.PC +=3;
    gb->cpu.cycle = 16;

    // Bytes = 3
    // t-cycles = 16
    // No flags affected
}
static void LD_A_p_a16(GB *gb, CPU *cpu, instruction_T instruction) {
    uint16_t a16 = cnvrt_lil_endian(instruction.operand1, instruction.operand2);
    gb->cpu.reg.A = external_read(gb, a16);
    gb->cpu.reg.PC += 3;
    gb->cpu.cycle = 16;

    // Bytes = 3
    // t-cycles = 16
    // No flags affected
}

// LDH a8/ [c] instructions:
static void LDH_p_C_A(GB *gb, CPU *cpu, instruction_T instruction) {     // LDH => Load (High Range). 0xFF the high bit, is fixed.
    uint16_t combined_addr = 0xFF00 + gb->cpu.reg.C;
    external_write(gb, combined_addr, gb->cpu.reg.A);

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycles = 8
}
static void LDH_A_p_C(GB *gb, CPU *cpu, instruction_T instruction) {     // LDH => Load (High Range). 0xFF the high bit, is fixed.
    uint16_t combined_addr = 0xFF00 + gb->cpu.reg.C;
    gb->cpu.reg.A = external_read(gb, combined_addr);
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycles = 8
}
static void LDH_A_p_a8(GB *gb, CPU *cpu, instruction_T instruction) {
    uint8_t a8 = instruction.operand1;
    uint16_t combined_addr = 0xFF00 + a8;
    uint8_t n8_loaded = external_read(gb, combined_addr);
    gb->cpu.reg.A = external_read(gb, combined_addr);
    gb->cpu.reg.PC +=2;
    gb->cpu.cycle = 12;

    // Bytes = 2
    // t-cycle = 12
}
static void LDH_p_a8_A(GB *gb, CPU *cpu, instruction_T instruction) {
    uint8_t a8 = instruction.operand1;
    uint16_t combined_addr = 0xFF00 + a8;
    external_write(gb, combined_addr, gb->cpu.reg.A);
    gb->cpu.reg.PC +=2;
    gb->cpu.cycle = 12;

    // Bytes = 2
    // t-cycle = 12
}


/// SECTION:
// LD Stack manipulation Instructions:

static void LD_p_a16_SP(GB *gb, CPU *cpu, instruction_T instruction) {
    uint16_t n16_addr = cnvrt_lil_endian(instruction.operand1, instruction.operand2);
    uint8_t SP_LOW = gb->cpu.reg.SP & 0xFF;
    uint8_t SP_HIGH = (gb->cpu.reg.SP >> 8) & 0xFF;

    external_write(gb, n16_addr, SP_LOW);
    external_write(gb, n16_addr + 1, SP_HIGH);
    gb->cpu.reg.PC += 3;
    gb->cpu.cycle = 20;

    // Bytes = 3
    // t-cycles = 20
}
static void LD_HL_SP_Pe8(GB *gb, CPU *cpu, instruction_T instruction) {     // Load value in SP + (8bit (e) SIGNED int) into HL Register
    int8_t e_signed_offset = (int8_t)instruction.operand1;  // NOTICE int8_t = signed, because e = signed 8bit register. Because it's relative a: +-
    gb->cpu.reg.HL = (gb->cpu.reg.SP + e_signed_offset);    // SET HL to the Calulated +- n16 value.

    uint8_t SP_LOW = gb->cpu.reg.SP & 0xFF;                 // This is first 8Bits. (Instead of full 16).
    uint8_t e8_flag_calc = (uint8_t)e_signed_offset;        // For easy Calculations, change back into uint8_t (Unsigned)

    // Both Z and N always cleard
    clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);

    if (((SP_LOW & 0xF) + (e8_flag_calc & 0xF)) > 0xF)       // IF combined the (first 4 bits) of each is GREATER than 4 bits.
        set_cpu_flag(gb, FLAG_H);    // Set H Flag
    else
        clear_cpu_flag(gb, FLAG_H);  // Clear H Flag

    if ((SP_LOW + e8_flag_calc) > 0xFF)                      // If combined, the values are greater than 8 Bits.
        set_cpu_flag(gb, FLAG_C);    // Set C Flag
    else
        clear_cpu_flag(gb, FLAG_C);  // Clear C Flag

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 12;

    // Bytes = 2
    // t-cycle = 12
}
static void LD_SP_HL(GB *gb, CPU *cpu, instruction_T instruction) {
    gb->cpu.reg.SP = gb->cpu.reg.HL;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // Bytes = 1
    // t-cycle = 8
    // No flags effected
}



/// SECTION:
// MACRO'D LD instructions. Covers LD, r8, r8. LD X, [HL]. LD [HL], B

// LD r8, r8. - Register to register.
// Bytes = 1, T-Cycles = 4
#define LD_X_Y(X, Y) \
static void LD_##X##_##Y(GB *gb, CPU *cpu, instruction_T instrc) \
{ \
    gb->cpu.reg.X = gb->cpu.reg. Y;\
    gb->cpu.reg.PC += 1;\
    gb->cpu.cycle = 4;\
}

// LD r8, [HL]. - value pointed by [HL] to r8.
// Bytes = 1, T-Cycles = 8
#define LD_X_DHL(X) \
static void LD_##X##_##DHL(GB *gb, CPU *cpu, instruction_T instrc) \
{ \
    gb->cpu.reg.X = external_read(gb, gb->cpu.reg.HL);\
    gb->cpu.reg.PC += 1;\
    gb->cpu.cycle = 8;\
}

// LD [HL], r8. - r8 to value pointed by [HL]
// Bytes = 1, T-Cycles = 8
#define LD_DHL_Y(Y) \
static void LD_##DHL##_##Y(GB *gb, CPU *cpu, instruction_T instrc) \
{ \
    external_write(gb, gb->cpu.reg.HL, gb->cpu.reg.Y);\
    gb->cpu.reg.PC += 1;\
    gb->cpu.cycle = 8;\
}

/* NOP */   LD_X_Y(B,C) LD_X_Y(B,D) LD_X_Y(B,E) LD_X_Y(B,H) LD_X_Y(B,L) LD_X_DHL(B) LD_X_Y(B,A) LD_X_Y(C,B) /* NOP */   LD_X_Y(C,D) LD_X_Y(C,E) LD_X_Y(C,H) LD_X_Y(C,L) LD_X_DHL(C) LD_X_Y(C,A)
LD_X_Y(D,B) LD_X_Y(D,C) /* NOP */   LD_X_Y(D,E) LD_X_Y(D,H) LD_X_Y(D,L) LD_X_DHL(D) LD_X_Y(D,A) LD_X_Y(E,B) LD_X_Y(E,C) LD_X_Y(E,D) /* NOP */   LD_X_Y(E,H) LD_X_Y(E,L) LD_X_DHL(E) LD_X_Y(E,A)
LD_X_Y(H,B) LD_X_Y(H,C) LD_X_Y(H,D) LD_X_Y(H,E) /* NOP */   LD_X_Y(H,L) LD_X_DHL(H) LD_X_Y(H,A) LD_X_Y(L,B) LD_X_Y(L,C) LD_X_Y(L,D) LD_X_Y(L,E) LD_X_Y(L,H) /* NOP */   LD_X_DHL(L) LD_X_Y(L,A)
LD_DHL_Y(B) LD_DHL_Y(C) LD_DHL_Y(D) LD_DHL_Y(E) LD_DHL_Y(H) LD_DHL_Y(L) /* NOP */   LD_DHL_Y(A) LD_X_Y(A,B) LD_X_Y(A,C) LD_X_Y(A,D) LD_X_Y(A,E) LD_X_Y(A,H) LD_X_Y(A,L) LD_X_DHL(A) /* NOP */


// -----------------------------------------------
/// SECTION:
// JUMPs and Relative Jumps (JRs)

// Jump instructions
static void JP_HL(GB *gb, CPU *cpu, instruction_T instruction) {    // Copy Address in HL to PC

    gb->cpu.reg.PC = gb->cpu.reg.HL;
    gb->cpu.cycle = 4;

    // Bytes: 1
    // t-cycles = 4
    // No Flags changed.
}

/// NOTE: Technically n16 == a16 in this case. Clarity of the OP Table. Naming these only a16, a8 instructions.

// Jump a16 instructions:
static void JP_a16(GB *gb, CPU *cpu, instruction_T instruction) {
    gb->cpu.reg.PC = cnvrt_lil_endian(instruction.operand1, instruction.operand2);
    gb->cpu.cycle = 16;

    // Bytes = 3
    // t-cycles = 16
}
static void JP_cc_a16(GB *gb, CPU *cpu, instruction_T instruction) {
    int proceed = 0;
    gb->cpu.cycle += 12; // +12 default untaken cost (+12 t-cycle. 3 m-cycles)

    switch(instruction.opcode) {
        case 0xC2:
            if (!(gb->cpu.reg.F & FLAG_Z)) proceed = 1;
            break;
        case 0xD2:
            if (!(gb->cpu.reg.F & FLAG_C)) proceed = 1;
            break;
        case 0xCA:
            if ((gb->cpu.reg.F & FLAG_Z)) proceed = 1;
            break;
        case 0xDA:
            if ((gb->cpu.reg.F & FLAG_C)) proceed = 1;
            break;
    }
    if (proceed) {
        gb->cpu.reg.PC = cnvrt_lil_endian(instruction.operand1, instruction.operand2);
        gb->cpu.cycle += 4;    // +4 taken cost (4 t-cycles. 1 m-cycle)
    }
    else { gb->cpu.reg.PC += 3; }

    // Cycles: 4 taken / 3 untaken
    // t-cycles = 16 taken / 12 untaken
    // 3 Bytes
    // No flags Changed
}

// Relative Jumps: e8
static void JR_e8(GB *gb, CPU *cpu, instruction_T instruction) {
    int8_t e_signed_offset;             // e = signed 8bit register. Because it's relative to the PC location +- a value.
    e_signed_offset = (int8_t)instruction.operand1;
    uint16_t next_pc = gb->cpu.reg.PC+2;   // Value of next PC + 2 Bytes

    gb->cpu.reg.PC = (uint16_t)(next_pc + e_signed_offset);   // It's supposed to jump the offsetup. + whatever the PC would be advanced by.
    gb->cpu.cycle = 12;

    // m-Cycles 3
    // t-cycles 12
    // Bytes 2
    // Flags Changed, none
}
static void JR_cc_e8(GB *gb, CPU *cpu, instruction_T instruction) {
    gb->cpu.cycle += 8; // +8 default untaken cost. (+8 t-cycles. +2 m-cycles)

    int8_t e_signed_offset;       // e = signed 8bit register. Because it's relative to the PC location +- a value.
    e_signed_offset = (int8_t)instruction.operand1;
    uint16_t next_pc = gb->cpu.reg.PC+2;  // Value of PC + 2 Bytes. (PC Advanced by 2)

    switch (instruction.opcode) {
        case 0x20:
            if (!(gb->cpu.reg.F & FLAG_Z)) {
                gb->cpu.reg.PC = (uint16_t)(next_pc + e_signed_offset);   // It's supposed to jump the offsetup. + whatever the PC would be advanced by.
                gb->cpu.cycle += 4;    // +4 taken cost (4 t-cycles. 1 m-cycles)
            } else {
                gb->cpu.reg.PC = next_pc;
            }
            break;
        case 0x28:
            if (gb->cpu.reg.F & FLAG_Z) {
                gb->cpu.reg.PC = (uint16_t)(next_pc + e_signed_offset);   // It's supposed to jump the offsetup. + whatever the PC would be advanced by.
                gb->cpu.cycle += 4;    // +4 taken cost (4 t-cycles. 1 m-cycles)
            } else {
                gb->cpu.reg.PC = next_pc;
            }
            break;
        case 0x30:
            if (!(gb->cpu.reg.F & FLAG_C)) {
                gb->cpu.reg.PC = (uint16_t)(next_pc + e_signed_offset);   // It's supposed to jump the offsetup. + whatever the PC would be advanced by.
                gb->cpu.cycle += 4;    // +4 taken cost (4 t-cycles. 1 m-cycles)
            } else {
                gb->cpu.reg.PC = next_pc;
            }
            break;
        case 0x38:
            if (gb->cpu.reg.F & FLAG_C) {
                gb->cpu.reg.PC = (uint16_t)(next_pc + e_signed_offset);   // It's supposed to jump the offsetup. + whatever the PC would be advanced by.
                gb->cpu.cycle += 4;    // +4 taken cost (4 t-cycles. 1 m-cycles)
            } else {
                gb->cpu.reg.PC = next_pc;
            }
            break;
        default:
            printf("ERROR, JR CC did not match ANY OPCODES.. should abort.\n");
            gb->cpu.reg.PC = next_pc;
    }

    // 3 M-cycles = 12 T-cycles
    // 2 M-cycles = 8 T-cycles

    // Cycles: 3 taken / 2 untaken (Machine cycles) .....
    // Bytes: 2
    // Flags Changed, None
}



// -----------------------------------------------
/// SECTION:
// ADD ADC, SUB, SBC, INC, DEC, CP
// 8-Bit arithmetic instructions:

// ADD/ ADC Instructions:
static void ADD_A_r8(GB *gb, CPU *cpu, instruction_T instruction) {      // Add value of r8 into A
    // Table calculates WHICH register is called, based on the OP code provided.
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t op_index = (instruction.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.


    uint16_t add_result = (gb->cpu.reg.A + *reg_table[op_index]);
    uint8_t final_8bit = (uint8_t)add_result;

    (final_8bit == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    ((gb->cpu.reg.A & 0x0F) + (op_r8 & 0x0F) > 0x0F) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag
    (add_result > 0xFF) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag
    clear_cpu_flag(gb, FLAG_N);  // N Flag (Subtraction) Always cleared on ADD

    gb->cpu.reg.A = final_8bit;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // Bytes = 1
    // t-cycle = 4


    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = Set if overflow bit 3
        C = Set if overflow bit 7
    */
}
static void ADD_A_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {    // Add value pointed by HL into A
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint16_t add_result = (gb->cpu.reg.A + hl_val);
    uint8_t final_8bit = (uint8_t)add_result;

    (final_8bit == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    ((gb->cpu.reg.A & 0x0F) + (hl_val & 0x0F) > 0x0F) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag
    (add_result > 0xFF) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag
    clear_cpu_flag(gb, FLAG_N);  // N Flag (Subtraction) Always cleared on ADD

    gb->cpu.reg.A = final_8bit;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycles = 8
}
static void ADD_A_n8(GB *gb, CPU *cpu, instruction_T instruction) {
    uint8_t n8_val = instruction.operand1;

    uint16_t add_result = (gb->cpu.reg.A + n8_val);
    uint8_t final_8bit = (uint8_t)add_result;

    (final_8bit == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    ((gb->cpu.reg.A & 0x0F) + (n8_val & 0x0F) > 0x0F) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag
    (add_result > 0xFF) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag
    clear_cpu_flag(gb, FLAG_N);  // N Flag (Subtraction) Always cleared on ADD

    gb->cpu.reg.A = final_8bit;
    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycles = 8
}
// ADC Add instructions:
static void ADC_A_r8(GB *gb, CPU *cpu, instruction_T instruction) {
    // Yes, A + r8 + Carry Flag. -- If it rolls over. That's ok, track it with the Carry Flag.

    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t op_index = (instruction.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.


    uint8_t carry_val = (gb->cpu.reg.F & FLAG_C) ? 1 : 0;

    // Use 16bit for Flag checks. 8bit will truncate results
    uint16_t add_16bit  = (gb->cpu.reg.A + op_r8 + carry_val);
    uint8_t final_8bit = (uint8_t)add_16bit;

    (final_8bit == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    (((gb->cpu.reg.A & 0x0F) + (op_r8 & 0x0F) + carry_val) > 0x0F) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag
    (add_16bit > 0xFF) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag
    clear_cpu_flag(gb, FLAG_N);  // N Flag (Subtraction) Always cleared on ADC

    gb->cpu.reg.A = final_8bit;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // Bytes = 1
    // t-cycle = 4
}
static void ADC_A_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t carry_val = (gb->cpu.reg.F & FLAG_C) ? 1 : 0;

    uint16_t add_16bit = (gb->cpu.reg.A + hl_val + carry_val);
    uint8_t final_8bit = (uint8_t)add_16bit;


    (final_8bit == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    (((gb->cpu.reg.A & 0x0F) + (hl_val & 0x0F) + carry_val) > 0x0F) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag
    (add_16bit > 0xFF) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag
    clear_cpu_flag(gb, FLAG_N);  // N Flag (Subtraction) Always cleared on ADC

    gb->cpu.reg.A = final_8bit;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycles = 8
}
static void ADC_A_n8(GB *gb, CPU *cpu, instruction_T instruction) {
    uint8_t n8_val = instruction.operand1;
    uint8_t carry_val = (gb->cpu.reg.F & FLAG_C) ? 1 : 0;

    uint16_t add_16bit = (gb->cpu.reg.A + n8_val + carry_val);
    uint8_t final_8bit = (uint8_t)add_16bit;


    (final_8bit == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    (((gb->cpu.reg.A & 0x0F) + (n8_val & 0x0F) + carry_val) > 0x0F) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag
    (add_16bit > 0xFF) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag
    clear_cpu_flag(gb, FLAG_N);  // N Flag (Subtraction) Always cleared on ADC

    gb->cpu.reg.A = final_8bit;
    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycles = 8
}

// SUB / SBC Instructions:
static void SUB_A_r8(GB *gb, CPU *cpu, instruction_T instruction) {     // Subtract values in a, by 8byte register
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t op_index = (instruction.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.
    uint8_t reg_a = gb->cpu.reg.A;

    uint16_t sub_16bit = (gb->cpu.reg.A - op_r8);
    uint8_t final_8bit = (uint8_t)sub_16bit;


    (final_8bit == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    ((reg_a & 0x0F) < (op_r8 & 0x0F)) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag
    (reg_a < op_r8) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag
    set_cpu_flag(gb, FLAG_N);  // N Flag (Subtraction) Always SET on SUB/SBC

    gb->cpu.reg.A = final_8bit;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // Bytes = 1
    // t-cycles = 4

    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (i.e. if r8 > A).
    */
}
static void SUB_A_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t reg_a = gb->cpu.reg.A;

    uint16_t sub_16bit = (gb->cpu.reg.A - hl_val);
    uint8_t final_8bit = (uint8_t)sub_16bit;


    (final_8bit == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    ((reg_a & 0x0F) < (hl_val & 0x0F)) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag
    (reg_a < hl_val) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag
    set_cpu_flag(gb, FLAG_N);  // N Flag (Subtraction) Always SET on SUB/SBC

    gb->cpu.reg.A = final_8bit;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycles = 8
}
static void SUB_A_n8(GB *gb, CPU *cpu, instruction_T instruction) {
    uint8_t n8 = instruction.operand1;
    uint8_t reg_a = gb->cpu.reg.A;

    uint16_t sub_16bit = (gb->cpu.reg.A - n8);
    uint8_t final_8bit = (uint8_t)sub_16bit;


    (final_8bit == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    ((reg_a & 0x0F) < (n8 & 0x0F)) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag
    (reg_a < n8) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag
    set_cpu_flag(gb, FLAG_N);  // N Flag (Subtraction) Always SET on SUB/SBC

    gb->cpu.reg.A = final_8bit;
    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // bytes = 2
    // t-cycles = 8
}
// SBC (Sub with the cary flag):
static void SBC_A_r8(GB *gb, CPU *cpu, instruction_T instruction) {     // Subtract the value in r8 and the carry flag from A.
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t op_index = (instruction.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.

    uint8_t carry_val = (gb->cpu.reg.F & FLAG_C) ? 1 : 0;

    // Use 16bit for Flag checks. 8bit will truncate results
    uint16_t sub_16bit  = (gb->cpu.reg.A - op_r8 - carry_val);
    uint8_t final_8bit = (uint8_t)sub_16bit;
    uint8_t reg_a = gb->cpu.reg.A;


    (final_8bit == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    (((reg_a & 0x0F) - (op_r8 & 0x0F) - carry_val) < 0) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag

    // This has + carry, as it checks if the whole result underflowed
    // If register a is less than the TOTAL amount of in op_r8. Then set the flag. (Hence the + carry_val)
    (reg_a < (op_r8 + carry_val)) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag
    set_cpu_flag(gb, FLAG_N);  // N Flag (Subtraction) Always SET on SUB/SBC

    gb->cpu.reg.A = final_8bit;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // bytes = 1
    // t-cycle = 4

    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (i.e. if (r8 + carry) > A)
    */
}
static void SBC_A_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {   // Subtract the byte pointed to by HL and the carry flag from A.
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t carry_val = (gb->cpu.reg.F & FLAG_C) ? 1 : 0;

    // Use 16bit for Flag checks. 8bit will truncate results
    uint16_t sub_16bit  = (gb->cpu.reg.A - hl_val - carry_val);
    uint8_t final_8bit = (uint8_t)sub_16bit;
    uint8_t reg_a = gb->cpu.reg.A;


    (final_8bit == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    (((reg_a & 0x0F) - (hl_val & 0x0F) - carry_val) < 0) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag

    // This has + carry, as it checks if the whole result underflowed
    (reg_a < (hl_val + carry_val)) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag
    set_cpu_flag(gb, FLAG_N);  // N Flag (Subtraction) Always SET on SUB/SBC

    gb->cpu.reg.A = final_8bit;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // bytes = 1
    // t-cycles = 8
    // FLAGS: see sbc_a_r8
}
static void SBC_A_n8(GB *gb, CPU *cpu, instruction_T instruction) {     // Subtract the value n8 and the carry flag from A.
    uint8_t n8 = instruction.operand1;
    uint8_t carry_val = (gb->cpu.reg.F & FLAG_C) ? 1 : 0;

    // Use 16bit for Flag checks. 8bit will truncate results
    uint16_t sub_16bit  = (gb->cpu.reg.A - n8 - carry_val);
    uint8_t final_8bit = (uint8_t)sub_16bit;
    uint8_t reg_a = gb->cpu.reg.A;


    (final_8bit == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z Flag
    (((reg_a & 0x0F) - (n8 & 0x0F) - carry_val) < 0) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag

    // This has + carry, as it checks if the whole result underflowed
    (reg_a < (n8 + carry_val)) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag
    set_cpu_flag(gb, FLAG_N);  // N flag always set for Decrement.

    gb->cpu.reg.A = final_8bit;
    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 4;

    // Bytes = 2
    // t-cycles = 4
}

static void INC_r8(GB *gb, CPU *cpu, instruction_T instruction) {    // Increment Register r8
    uint8_t op_index = (instruction.opcode >> 3) & 0x07;
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t r8_val = *reg_table[op_index];

    // Verify if H overflows before increment
    ((r8_val & 0x0F) == 0x0F) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H);

    r8_val ++;
    (r8_val == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);  // N flag always cleared for increment

    *reg_table[op_index] = r8_val;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // Bytes = 1
    // t-cycles = 4;
    // Flags:
    // Z 0 H -
}
static void DEC_r8(GB *gb, CPU *cpu, instruction_T instruction) {    // Decrement High bit Register  (-- => B, D, H)
    uint8_t op_index = (instruction.opcode >> 3) & 0x07;
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t r8_val = *reg_table[op_index];

    // Verify if H underflows before decrement
    ((r8_val & 0x0F) == 0) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H);

    r8_val --;
    (r8_val == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    set_cpu_flag(gb, FLAG_N);   // N flag always set for Decrement.

    *reg_table[op_index] = r8_val;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;
    // Bytes = 1
    // t-cycles = 4
    // Flags:
    // Z 1 H -
}

static void INC_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {  // Increment 16 bit HL register, The Value In Pointer ++ HL
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);

    // Verify if H overflows before increment:
    ((hl_val & 0x0F) == 0x0F) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H);

    hl_val ++;
    (hl_val == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);  // N Flag always cleared

    external_write(gb, gb->cpu.reg.HL, hl_val);

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 12;

    // Bytes = 1
    // t-cycles = 12
    // Flags:
    // Z 0 H -
}
static void DEC_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {      // Decrement the Byte insde the location pointed by [HL]
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);

    // Verify if H underflows before decrement:
    ((hl_val & 0x0F) == 0x0) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H);

    hl_val --;
    (hl_val == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    set_cpu_flag(gb, FLAG_N); // N flag always set for Decrement.

    external_write(gb, gb->cpu.reg.HL, hl_val);

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 12;

    // Bytes = 1
    // t-cycles = 12
    // Flags:
    // Z 1 H -
}

// CP. ComPARE Instructions:
static void CP_A_r8(GB *gb, CPU *cpu, instruction_T instruction) {  // This does keep set the flags??
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t op_index = (instruction.opcode & 0x07);
    uint8_t op_r8 = *reg_table[op_index];                   // The calculated "Source" Register, from the OPCODE.

    uint8_t result = (gb->cpu.reg.A - op_r8);

    (result == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    set_cpu_flag(gb, FLAG_N);
    ((gb->cpu.reg.A & 0x0F) < (op_r8 & 0x0F)) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H);
    (gb->cpu.reg.A < op_r8) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);

    gb->cpu.reg.PC++;
    gb->cpu.cycle = 4;

    // Bytes = 1
    // t-cycles = 4
    // Flags:
    // Z 1 H C
}
static void CP_A_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {     // ComPare -> value in pointer HL to A
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t result = (gb->cpu.reg.A - hl_val);


    (result == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    set_cpu_flag(gb, FLAG_N);
    ((gb->cpu.reg.A & 0x0F) < (hl_val & 0x0F)) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H);
    (gb->cpu.reg.A < hl_val) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // bytes = 2
    // t-cycles = 8
    // Flags:
    // Z 1 H C
}
static void CP_A_n8(GB *gb, CPU *cpu, instruction_T instruction) {       // ComPare -> value in n8 to A
    uint8_t n8_val = instruction.operand1;
    uint8_t result = (gb->cpu.reg.A - instruction.operand1);

    set_cpu_flag(gb, FLAG_N);    // N Flag - Always Set (Subtraction Flag)

    (result == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);                        // Z Flag
    ((gb->cpu.reg.A & 0x0F) < (n8_val & 0x0F)) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H);  // H Flag
    (gb->cpu.reg.A < n8_val) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);                    // C Flag

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;
    // Bytes = 2
    // t-cylces = 8
    // Flags:
    // Z 1 H C
}


// -----------------------------------------------
/// SECTION:
// Stack Manipulation, ADD, INC, DEC, LD, POP, PUSH

// ADD Stack manipulation Instructions:
// Special SP / e8 ADD function.
static void ADD_SP_e8(GB *gb, CPU *cpu, instruction_T instruction) {     // e8 = SIGNED int.
    int8_t e_val = (int8_t)instruction.operand1;       // NOTICE int8_t = signed, because e = signed 8bit register. Because it's relative a: +-
    uint8_t u8 = instruction.operand1;
    uint16_t sp_val = gb->cpu.reg.SP;
    uint16_t combined_result = (uint16_t)(sp_val + e_val);

    // NOTE: ADD_SP_e8  Z and N aare always cleared.
    clear_cpu_flag(gb, FLAG_Z); // Z Flag Cleared
    clear_cpu_flag(gb, FLAG_N); // N Flag Cleared (Subtraction)
    (((sp_val & 0x000F) + (u8 & 0x000F)) > 0x000F) ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag
    (((sp_val & 0x00FF) + u8) > 0x00FF) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag


    gb->cpu.reg.SP = combined_result;
    gb->cpu.cycle = 16;
    gb->cpu.reg.PC += 2;

    // Bytes = 2
    // t-cycles = 16
    // Flags:
    // 0 0 H C
}
static void ADD_HL_r16(GB *gb, CPU *cpu, instruction_T instruction) {
    uint16_t *reg16_table[4] = {
        &gb->cpu.reg.BC, &gb->cpu.reg.DE, &gb->cpu.reg.HL, &gb->cpu.reg.SP
    };
    uint8_t register_index = (instruction.opcode >> 4) & 0x03;
    uint16_t op_r16 = *reg16_table[register_index];                   // The calculated "Source" Register, from the OPCODE.
    uint16_t hl_val = gb->cpu.reg.HL;

    uint32_t add_result = (hl_val + op_r16);
    uint16_t final_result = (uint16_t)add_result;

    // Z Unchanged
    clear_cpu_flag(gb, FLAG_N);  // N Flag Cleared (Subtraction)
    ((hl_val & 0x0FFF) + (op_r16 & 0x0FFF) > 0x0FFF)    // NOTE: 16byte half carry goes off of bit 11 instead of bit 4 for 8byte.
        ? set_cpu_flag(gb, FLAG_H) : clear_cpu_flag(gb, FLAG_H); // H Flag
    (add_result > 0xFFFF)
        ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C); // C Flag

    gb->cpu.reg.HL = final_result;
    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycle = 8
    // Flags:
    // - 0 H C
}

// Full INC/DEC 16 bit Registers (BC, DE, HL):
static void INC_r16(GB *gb, CPU *cpu, instruction_T instruction) {
    switch (instruction.opcode) {
        case 0x03: gb->cpu.reg.BC ++; break;
        case 0x13: gb->cpu.reg.DE ++; break;
        case 0x23: gb->cpu.reg.HL ++; break;
        case 0x33: gb->cpu.reg.SP ++; break;       // Notice this is technically STACK MANIPULATION. INC_SP
    }

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycles = 8
    // FLAGS: NONE AFFECTED
}
static void DEC_r16(GB *gb, CPU *cpu, instruction_T instruction) {
    switch (instruction.opcode) {
        case 0x0B: gb->cpu.reg.BC --; break;
        case 0x1B: gb->cpu.reg.DE --; break;
        case 0x2B: gb->cpu.reg.HL --; break;
        case 0x3B: gb->cpu.reg.SP --; break;       // Notice this is technically STACK MANIPULATION. DEC_SP
    }

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycles = 8
    // FLAGS: NONE AFFECTED
}

static void POP_AF(GB *gb, CPU *cpu, instruction_T instruction) {        // Pop register AF from the stack.
    uint8_t low = external_read(gb, gb->cpu.reg.SP);
    gb->cpu.reg.SP++;
    uint8_t high = external_read(gb, gb->cpu.reg.SP);
    gb->cpu.reg.SP++;

    gb->cpu.reg.A = high;
    gb->cpu.reg.F = low & 0xF0;

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 12;

    // Bytes = 1
    // t-cycles = 12
    // Flags:
    // The low 8bit, with upper of 8 bit. Is used for flag. So: 1111 xxxx.
}
static void POP_r16(GB *gb, CPU *cpu, instruction_T instruction) {       // Pop register r16 from the stack.
    uint8_t op_index = (instruction.opcode >> 4) & 0x03;
    uint16_t *reg_table[3] = {
        &gb->cpu.reg.BC, &gb->cpu.reg.DE, &gb->cpu.reg.HL
    };

    uint8_t low_byte = external_read(gb, gb->cpu.reg.SP);
    gb->cpu.reg.SP ++;
    uint8_t high_byte = external_read(gb, gb->cpu.reg.SP);
    gb->cpu.reg.SP ++;

    // Depending on the OPCODE called, will set the specific Register to the Opcode.
    *reg_table[op_index] = cnvrt_lil_endian(low_byte, high_byte);

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 12;

    // Bytes = 1
    // t-cycles = 12
    // FLAGS: None affected
}
static void PUSH_AF(GB *gb, CPU *cpu, instruction_T instruction) {       // Push register AF onto the stack.
    gb->cpu.reg.SP --;
    external_write(gb, gb->cpu.reg.SP, gb->cpu.reg.A);
    gb->cpu.reg.SP --;
    external_write(gb, gb->cpu.reg.SP, gb->cpu.reg.F & 0xF0);

    gb->cpu.reg.PC += 1;     // PUSH_AF is only 1 byte.
    gb->cpu.cycle = 16;

    // Bytes = 1
    // t-cycles = 16
    // FLAGS: None affected
}
static void PUSH_r16(GB *gb, CPU *cpu, instruction_T instruction) {      // Pushes r16 register onto the Stack.
    switch (instruction.opcode) {
        case 0xC5:    // BC to stack
            gb->cpu.reg.SP --;
            external_write(gb, gb->cpu.reg.SP, gb->cpu.reg.B);
            gb->cpu.reg.SP --;
            external_write(gb, gb->cpu.reg.SP, gb->cpu.reg.C);
            break;
        case 0xD5:    // DE to stack
            gb->cpu.reg.SP --;
            external_write(gb, gb->cpu.reg.SP, gb->cpu.reg.D);
            gb->cpu.reg.SP --;
            external_write(gb, gb->cpu.reg.SP, gb->cpu.reg.E);
            break;
        case 0xE5:    // HL to stack
            gb->cpu.reg.SP --;
            external_write(gb, gb->cpu.reg.SP, gb->cpu.reg.H);
            gb->cpu.reg.SP --;
            external_write(gb, gb->cpu.reg.SP, gb->cpu.reg.L);
            break;
    }

    gb->cpu.cycle = 16;
    gb->cpu.reg.PC += 1;
    // Bytes = 1
    // t-cyles = 16
    // FLAGS: None affected
}


/// SECTION:
// Subroutine instrucitons.

static void CALL_a16(GB *gb, CPU *cpu, instruction_T instruction) {      // Pushes the PC address of the instruction after the CALL, on the stack. Then it executes an implicit JP n16. (Later RET can POP it later)
    uint16_t pc_loc = (gb->cpu.reg.PC + 3);    // Advance the PC, as this is the location wanted to go AFTER.

    uint8_t split_addr_LOW = pc_loc & 0xFF;
    uint8_t split_addr_HIGH = (pc_loc >> 8) & 0xFF;

    gb->cpu.reg.SP --;
    external_write(gb, gb->cpu.reg.SP, split_addr_HIGH);
    gb->cpu.reg.SP --;
    external_write(gb, gb->cpu.reg.SP, split_addr_LOW);

    // Next jump to Location (This is basically JP a16):
    gb->cpu.reg.PC = cnvrt_lil_endian(instruction.operand1, instruction.operand2);
    gb->cpu.cycle = 24;

    // Bytes: 3
    // t-cycles = 24
}
static void CALL_cc_a16(GB *gb, CPU *cpu, instruction_T instruction) {   // Call address n16 if condition cc is met.
    int proceed = 0;
    gb->cpu.cycle += 12; // +12 t-cycles default untaken cost.

    switch (instruction.opcode) {
        case 0xC4:
            // Z flag is NOT set.
            if (!(gb->cpu.reg.F & FLAG_Z)) proceed = 1;
            break;
        case 0xD4:
            // C flag is NOT set.
            if (!(gb->cpu.reg.F & FLAG_C)) proceed = 1;
            break;
        case 0xCC:
            // Z flag IS set.
            if ((gb->cpu.reg.F & FLAG_Z)) proceed = 1;
            break;
        case 0xDC:
            // C flag IS set.
            if ((gb->cpu.reg.F & FLAG_C)) proceed = 1;
            break;
    }

    if (proceed) {
        uint16_t pc_loc = (gb->cpu.reg.PC + 3);

        uint8_t split_addr_LOW = pc_loc & 0xFF;
        uint8_t split_addr_HIGH = (pc_loc >> 8) & 0xFF;

        gb->cpu.reg.SP --;
        external_write(gb, gb->cpu.reg.SP, split_addr_HIGH);
        gb->cpu.reg.SP --;
        external_write(gb, gb->cpu.reg.SP, split_addr_LOW);

        gb->cpu.reg.PC = cnvrt_lil_endian(instruction.operand1, instruction.operand2);

        gb->cpu.cycle += 12; // +12 taken cost (12 t-cycle. 3 m-cycles)
    }
    else {
        gb->cpu.reg.PC += 3;       // Skip over the entire CALL instruction (Which is 3 bytes in Length)
    }

    // t-cycles: 24 taken / 12 untaken
    // Bytes: 3
    // Flags: None affected.
}

static void RET(GB *gb, CPU *cpu, instruction_T instruction) {           // RETurn from subroutine / CALL. POPs the PC from the Stack pointer (SP)
    uint8_t low_byte = external_read(gb, gb->cpu.reg.SP);
    gb->cpu.reg.SP ++;
    uint8_t high_byte = external_read(gb, gb->cpu.reg.SP);
    gb->cpu.reg.SP ++;
    gb->cpu.reg.PC = cnvrt_lil_endian(low_byte, high_byte);
    gb->cpu.cycle = 16;

    // t-cycles = 16
    // Bytes: 1
    // FLAGS: None affected
}
static void RET_cc(GB *gb, CPU *cpu, instruction_T instruction) {        // RETurn from subroutine if condition CC is met
    int proceed = 0;
    gb->cpu.cycle += 8; // +8 t-cycles default untaken cost

    switch (instruction.opcode) {
        case 0xC0:
            // Z flag is NOT set.
            if (!(gb->cpu.reg.F & FLAG_Z)) proceed = 1;
            break;
        case 0xD0:
            // C flag is NOT set.
            if (!(gb->cpu.reg.F & FLAG_C)) proceed = 1;
            break;
        case 0xC8:
            // Z flag IS set.
            if ((gb->cpu.reg.F & FLAG_Z)) proceed = 1;
            break;
        case 0xD8:
            // C flag IS set.
            if ((gb->cpu.reg.F & FLAG_C)) proceed = 1;
            break;
    }

    if (proceed) {
        // TL;DR : Populate the PC from the SP.
        uint8_t low_byte = external_read(gb, gb->cpu.reg.SP);
        gb->cpu.reg.SP ++;
        uint8_t high_byte = external_read(gb, gb->cpu.reg.SP);
        gb->cpu.reg.SP ++;

        gb->cpu.reg.PC = cnvrt_lil_endian(low_byte, high_byte);
        gb->cpu.cycle += 12; // +12 t-cylce taken cost
    }
    else {
        gb->cpu.reg.PC += 1;       // Skip over RET instruction (1 Byte)
    }

    // M-Cycles: 5 taken / 2 untaken
    // Bytes: 1
    // FLAGS: None affected
}
static void RETI(GB *gb, CPU *cpu, instruction_T instruction) {          // RETI => RETurn from subroutine and set IME (NO Delay)
    RET(gb, cpu, instruction);  // Execute a normal ret.
    cpu->state.IME = 1;         // Set IME flag to 1 - enabled (No Delay)

    // Cycles: 16
    // Bytes: 1
    // Flags: None affected.
}
static void RST_vec(GB *gb, CPU *cpu, instruction_T instruction) {       // Saves PC to Stack, then jumps to hardcoded vector address.
    uint8_t vec_table[8] = {
        0x00, 0x08, 0x10, 0x18,
        0x20, 0x28, 0x30, 0x38
    };
    uint8_t op_index = (instruction.opcode >> 3) & 0x07;
    uint16_t jump_addr = vec_table[op_index];   // Vec table is 8bit, which means high bits are truncated to 0

    gb->cpu.reg.PC += 1; // RST_vec is 1 byte, advance it before storing.

    uint16_t pc_loc = gb->cpu.reg.PC;

    uint8_t split_addr_LOW = pc_loc & 0xFF;
    uint8_t split_addr_HIGH = (pc_loc >> 8) & 0xFF;

    // Push return address onto stack (high byte first)
    gb->cpu.reg.SP --;
    external_write(gb, gb->cpu.reg.SP, split_addr_HIGH);
    gb->cpu.reg.SP --;
    external_write(gb, gb->cpu.reg.SP, split_addr_LOW);
    gb->cpu.reg.PC = jump_addr;
    gb->cpu.cycle = 16;

    // Bytes = 1
    // t-cycles = 16
    // FLAGS: None affected
}





// -----------------------------------------------
/// SECTION:
// Bitwise, AND, OR XOR instructions.

static void AND_A_r8(GB *gb, CPU *cpu, instruction_T instruction) {      // Set A to the bitwise AND between the value in r8 and A
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };

    uint8_t op_index = (instruction.opcode & 0x07);
    uint8_t AND_result = (gb->cpu.reg.A & *reg_table[op_index]);    // Bitwise AND.

    (AND_result == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);  // ALways cleared
    set_cpu_flag(gb, FLAG_H);    // Always set
    clear_cpu_flag(gb, FLAG_C);  // Always cleared.

    gb->cpu.reg.A = AND_result;

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;
    // Bytes = 1
    // t-cycle = 4
    // Flags:
    // Z 0 1 0
}
static void AND_A_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {    // Set A to the bitwise AND between the byte pointed to by HL and A
    uint8_t AND_result = (gb->cpu.reg.A & external_read(gb, gb->cpu.reg.HL));
    (AND_result == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);  // ALways cleared
    set_cpu_flag(gb, FLAG_H);    // Always set
    clear_cpu_flag(gb, FLAG_C);  // Always cleared

    gb->cpu.reg.A = AND_result;

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;
    // Bytes = 1
    // t-cycles = 8
}
static void AND_A_n8(GB *gb, CPU *cpu, instruction_T instruction) {      // Set A to the bitwise AND between the value n8 and A
    uint8_t AND_result = (gb->cpu.reg.A & instruction.operand1);
    (AND_result == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);  // ALways cleared
    set_cpu_flag(gb, FLAG_H);    // Always set
    clear_cpu_flag(gb, FLAG_C);  // Always cleared

    gb->cpu.reg.A = AND_result;

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2;
    // t-cycles = 8
}
// OR Instructions:
static void OR_A_r8(GB *gb, CPU *cpu, instruction_T instruction) {       // Set A to the bitwise OR between the value in r8 and A
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t op_index = (instruction.opcode & 0x07);
    uint8_t OR_result = (gb->cpu.reg.A | *reg_table[op_index]);

    (OR_result == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);     // Set if Zero (Z Flag)
    clear_cpu_flag(gb, FLAG_N);  // ALways cleared
    clear_cpu_flag(gb, FLAG_H);  // Always cleared
    clear_cpu_flag(gb, FLAG_C);  // Always cleared

    gb->cpu.reg.A = OR_result;

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // Bytes = 1
    // t-cycles = 4
    // Flags:
    // Z 0 0 0
}
static void OR_A_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {     // Set A to the bitwise OR between the byte pointed to by HL and A
    uint8_t OR_result = (gb->cpu.reg.A | external_read(gb, gb->cpu.reg.HL));
    (OR_result == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);  // ALways cleared
    clear_cpu_flag(gb, FLAG_H);  // Always cleared
    clear_cpu_flag(gb, FLAG_C);  // Always cleared

    gb->cpu.reg.A = OR_result;

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycles = 8
}
static void OR_A_n8(GB *gb, CPU *cpu, instruction_T instruction) {       // Set A to the bitwise OR between the value n8 and A
    uint8_t OR_result = (gb->cpu.reg.A | instruction.operand1);
    (OR_result == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);  // ALways cleared
    clear_cpu_flag(gb, FLAG_H);  // Always cleared
    clear_cpu_flag(gb, FLAG_C);  // Always cleared

    gb->cpu.reg.A = OR_result;

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycles = 8
}
// XOR Instructions:
static void XOR_A_r8(GB *gb, CPU *cpu, instruction_T instruction) {      // Set A to the bitwise XOR between the value in r8 and A
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t op_index = (instruction.opcode & 0x07);
    uint8_t XOR_result = (gb->cpu.reg.A ^ *reg_table[op_index]);

    (XOR_result == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Set if Zero (Z Flag)
    clear_cpu_flag(gb, FLAG_N);  // ALways cleared
    clear_cpu_flag(gb, FLAG_H);  // Always cleared
    clear_cpu_flag(gb, FLAG_C);  // Always cleared

    gb->cpu.reg.A = XOR_result;

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;

    // Bytes = 1
    // t-cycles = 4
    // Flags:
    // Z 0 0 0
}
static void XOR_A_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {    // Set A to the bitwise XOR between the byte pointed to by HL and A
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t XOR_result = (gb->cpu.reg.A ^ hl_val);
    (XOR_result == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);  // ALways cleared
    clear_cpu_flag(gb, FLAG_H);  // Always cleared
    clear_cpu_flag(gb, FLAG_C);  // Always cleared

    gb->cpu.reg.A = XOR_result;

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 8;

    // Bytes = 1
    // t-cycle = 8
}
static void XOR_A_n8(GB *gb, CPU *cpu, instruction_T instruction) {      // Set A to the bitwise XOR between the value n8 and A
    uint8_t XOR_result = (gb->cpu.reg.A ^ instruction.operand1);
    (XOR_result == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);    // Z condition.
    clear_cpu_flag(gb, FLAG_N);  // N ALways cleared
    clear_cpu_flag(gb, FLAG_H);  // H Always cleared
    clear_cpu_flag(gb, FLAG_C);  // C Always cleared

    gb->cpu.reg.A = XOR_result;

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycle = 8
}

// CPL Instruction. Like a bitwse NOT
static void CPL(GB *gb, CPU *cpu, instruction_T instruction) {           // ComPLement accumulator (A = ~A); also called bitwise NOT a
    gb->cpu.reg.A = ~gb->cpu.reg.A;   // A = NOT A. Basically flip the bits around.

    set_cpu_flag(gb, FLAG_N);
    set_cpu_flag(gb, FLAG_H);   // Technically not true, but connsistant with GB

    gb->cpu.reg.PC += 1;
    gb->cpu.cycle = 4;
    // Bytes = 1
    // t-cycles = 4
    // Flags:
    // - 1 1 -
}




// -----------------------------------------------
/// SECTION:
// Bitshift and ROTATE instructions

// NON-PREFIXED
static void RRA(GB *gb, CPU *cpu, instruction_T instruction) {           // Rotate register A -> Right. Through the carry flag. b8 ---> b0 <> [carry flag]
    uint8_t a_reg = gb->cpu.reg.A;
    uint8_t carry_in = (gb->cpu.reg.F & FLAG_C) ? 1 : 0;
    uint8_t carry_out = (gb->cpu.reg.A & 0x1);
    uint8_t rotated_a = (a_reg >> 1) | (carry_in << 7);     // Rotate Right, Set bit 7
    gb->cpu.reg.A = rotated_a;

    clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);
    (carry_out) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);

    gb->cpu.reg.PC++;
    gb->cpu.cycle = 4;
    // Bytes = 1
    // t-cycles = 4
    // Flags:
    // 0 0 0 C
}

static void RRCA(GB *gb, CPU *cpu, instruction_T instruction) {          // Rotate Register A right. (WITHOUT CARRY)
    uint8_t a_reg = gb->cpu.reg.A;
    uint8_t carry_out = (a_reg & 0x1);
    uint8_t rotated_a = (a_reg >> 1) | (carry_out << 7);    // Rotate Right, Set bit 7
    gb->cpu.reg.A = rotated_a;

    clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);
    (carry_out) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);

    gb->cpu.reg.PC++;
    gb->cpu.cycle = 4;
    // Bytes = 1
    // t-cylces = 4
    // Flags:
    // 0 0 0 C
}

static void RLA(GB *gb, CPU *cpu, instruction_T instruction) {           // Rotate Register A Left. Through the carry flag <---
    uint8_t a_reg = gb->cpu.reg.A;
    uint8_t carry_in = (gb->cpu.reg.F & FLAG_C) ? 1 : 0;
    uint8_t carry_out = ((a_reg >> 7) & 0x1);               // Isolate and extract bit 7
    uint8_t rotated_a = (a_reg << 1) | carry_in;            // Shift left, set bit 0
    gb->cpu.reg.A = rotated_a;

    clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);
    (carry_out) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);

    gb->cpu.reg.PC++;
    gb->cpu.cycle = 4;
    // Bytes = 1
    // t-cylces = 4
    // Flags:
    // 0 0 0 C
}

static void RLCA(GB *gb, CPU *cpu, instruction_T instruction) {          // Rotate Register A left. (without carry flag input)
    uint8_t a_reg = gb->cpu.reg.A;
    uint8_t carry_out = ((a_reg >> 7) & 0x1);           // Isolate and extract bit 7
    uint8_t rotated_a = (a_reg << 1) | carry_out;       // Shift left, set bit 0
    gb->cpu.reg.A = rotated_a;

    clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);
    (carry_out) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);

    gb->cpu.reg.PC++;
    gb->cpu.cycle = 4;
    // Bytes = 1
    // m-Cylces = 1
    // t-cycles = 4
    // Flags:
    // 0 0 0 C
}







// ------------------------------------------------------
//
///                    NOTICE:
// ==---- Starting PREFIXED OP Code instructions ----==
//
// ------------------------------------------------------









static void RL_r8(GB *gb, CPU *cpu, instruction_T instruction) {         // Rotate Byte in Register r8 left, through the carry flag. <---
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E, &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t reg_index = instruction.operand1 & 0x07;
    uint8_t r8_reg = *reg_table[reg_index];

    uint8_t carry_in = (gb->cpu.reg.F & FLAG_C) ? 1 : 0;
    uint8_t carry_out = ((r8_reg >> 7) & 0x1);              // Isolate and extract bit 7
    uint8_t rotated_r8 = (r8_reg << 1) | carry_in;          // Shift left, set bit 0

    *reg_table[reg_index] = rotated_r8;                      // Set register to value.

    (carry_out != 0) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);      // C Flag
    (rotated_r8 == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);     // Z Flag
    clear_cpu_flag(gb, FLAG_N);  // N (Subtraction) Flag
    clear_cpu_flag(gb, FLAG_H);  // H (Half Carry) Flag

    gb->cpu.reg.PC +=2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycle = 8
    // Flags:
    // Z 0 0 C
}
static void RL_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {       // Rotate the byte pointed to by HL left, through the carry flag. <---
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t carry_in = (gb->cpu.reg.F & FLAG_C) ? 1 : 0;
    uint8_t carry_out = ((hl_val >> 7) & 0x1);              // Isolate and extract bit 7
    uint8_t rotated_hl = (hl_val << 1) | carry_in;          // Shift byte left, set bit 0 value.

    external_write(gb, gb->cpu.reg.HL, rotated_hl);    // Set [HL] value

    (carry_out != 0)  ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);     // C Flag
    (rotated_hl == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);     // Z Flag
    clear_cpu_flag(gb, FLAG_N);  // N (Subtraction) Flag
    clear_cpu_flag(gb, FLAG_H);  // H (Half Carry) Flag

    gb->cpu.reg.PC +=2;
    gb->cpu.cycle = 16;

    // Bytes = 2
    // t-cycle = 16
    // FLAGS: See RL_r8
}

static void RLC_r8(GB *gb, CPU *cpu, instruction_T instruction) {        // Rotate Registers r8 Left. <--- (without Carry flag input)
    // printf("PFX: RLC r8. Called.                   ; Rotate r8 Left (without carry flag input).\n");
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E, &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t reg_index = instruction.operand1 & 0x07;
    uint8_t r8_reg = *reg_table[reg_index];
    uint8_t carry_out = ((r8_reg >> 7) & 0x1);          // Isolate and extract bit 7
    uint8_t rotated_r8 = (r8_reg << 1) | carry_out;     // Shift left, set bit 0 value.
    *reg_table[reg_index] = rotated_r8;                 // Set r8 Register.

    (rotated_r8 == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);          // Z Flag
    (carry_out != 0)  ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);          // C Flag
    clear_cpu_flag(gb, FLAG_N);  // N (Subtraction) Flag
    clear_cpu_flag(gb, FLAG_H);  // H (Half Carry) Flag

    gb->cpu.reg.PC +=2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycle = 8
    // Flags:
    // Z 0 0 C
}
static void RLC_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {      // Rotate the byte pointed to by [HL] left. <--- (without Carry flag input)
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t carry_out = ((hl_val >> 7) & 0x1);              // Isolate and extract bit 7
    uint8_t rotated_hl = (hl_val << 1) | carry_out;         // Shift Byte Left, set bit 0

    external_write(gb, gb->cpu.reg.HL, rotated_hl);    // Set [HL] value

    (carry_out != 0)  ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);          // C Flag
    (rotated_hl == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);          // Z Flag
    clear_cpu_flag(gb, FLAG_N);  // N (Subtraction) Flag
    clear_cpu_flag(gb, FLAG_H);  // H (Half Carry) Flag

    gb->cpu.reg.PC +=2;
    gb->cpu.cycle = 16;

    // Bytes = 2
    // t-cycle = 16
    // FLAGS: See RLC_r8
}




// PREFIXED Rotate Right Instructions:
static void RR_r8(GB *gb, CPU *cpu, instruction_T instruction) {         // Rotate Register r8 Right. Through the carry flag. -->
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E, &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t reg_index = instruction.operand1 & 0x07;
    uint8_t r8_reg = *reg_table[reg_index];

    uint8_t carry_in = (gb->cpu.reg.F & FLAG_C) ? 1 : 0;
    uint8_t carry_out = (r8_reg & 0x1);
    uint8_t rotated_r8 = (r8_reg >> 1) | (carry_in << 7);   // Shift Byte right, set bit 7
    *reg_table[reg_index] = rotated_r8;                     // Set register to value.

    (rotated_r8 == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);   // Z Flag
    (carry_out != 0)  ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);   // C Flag
    clear_cpu_flag(gb, FLAG_N);  // N (Subtraction) Flag
    clear_cpu_flag(gb, FLAG_H);  // H (Half Carry) Flag

    gb->cpu.reg.PC +=2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycle = 8
    // Flags:
    // Z 0 0 C
}
static void RR_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {       // Rotate the byte pointed to by [HL] Right. Through the carry flag. -->
    // printf("PFX: RR [HL]. Called         ; Rotate value pointed by [HL] Right through carry flag.\n");

    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t carry_in = (gb->cpu.reg.F & FLAG_C) ? 1 : 0;
    uint8_t carry_out = (hl_val & 0x1);
    uint8_t rotated_hl = (hl_val >> 1) | (carry_in << 7);   // Shift Byte right, set bit 7

    external_write(gb, gb->cpu.reg.HL, rotated_hl);    // Set [HL] value.

    (rotated_hl == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);  // Z Flag
    (carry_out != 0)  ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);  // C Flag
    clear_cpu_flag(gb, FLAG_N);  // N (Subtraction) Flag
    clear_cpu_flag(gb, FLAG_H);  // H (Half Carry) Flag

    gb->cpu.reg.PC +=2;
    gb->cpu.cycle = 16;

    // Bytes = 2
    // t-cycle = 16
    // FLAGS: see RR_r8
}
static void RRC_r8(GB *gb, CPU *cpu, instruction_T instruction) {        // Rotate Register r8 Right. --> (Without carry flag input)
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E, &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t reg_index = instruction.operand1 & 0x07;
    uint8_t r8_reg = *reg_table[reg_index];
    uint8_t carry_out = (r8_reg & 0x1);
    uint8_t rotated_r8 = (r8_reg >> 1) | (carry_out << 7);      // Shift Byte Right, set bit 7
    *reg_table[reg_index] = rotated_r8;                          // Set r8 Register.

    (rotated_r8 == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);  // Z Flag
    (carry_out != 0)  ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);  // C Flag
    clear_cpu_flag(gb, FLAG_N);  // N (Subtraction) Flag
    clear_cpu_flag(gb, FLAG_H);  // H (Half Carry) Flag

    gb->cpu.reg.PC +=2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycle = 8
    // Flags:
    // Z 0 0 C
}
static void RRC_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {      // Rotate the value pointed to by [HL] Right. -->  (Without carry flag input)
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t carry_out = (hl_val & 0x1);
    uint8_t rotated_hl = (hl_val >> 1) | (carry_out << 7);              // Shift Byte Right, set bit 7
    external_write(gb, gb->cpu.reg.HL, rotated_hl);     // Set [HL] value

    (carry_out != 0)  ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);     // C Flag
    (rotated_hl == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);     // Z Flag
    clear_cpu_flag(gb, FLAG_N);  // N (Subtraction) Flag
    clear_cpu_flag(gb, FLAG_H);  // H (Half Carry) Flag

    gb->cpu.reg.PC +=2;
    gb->cpu.cycle = 16;

    // Bytes = 2
    // t-cycle = 16
    // FLAGS: See RRC r8
}




// PREFIXED SHIFT left & right Arithmetically.
static void SLA_r8(GB *gb, CPU *cpu, instruction_T instruction){         // Shift Left Arithmetically Register r8.  <--
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };

    uint8_t reg_index = instruction.operand1 & 0x07;
    uint8_t r8_val = *reg_table[reg_index];
    uint8_t carry_out = (r8_val >> 7) & 0x01;
    uint8_t shifted = (uint8_t)(r8_val << 1);
    *reg_table[reg_index] = shifted;

    (shifted == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);     // Z Flag
    (carry_out != 0) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);   // C Flag
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycle = 8
    // Flags:
    // Z 0 0 C
}
static void SLA_P_HL(GB *gb, CPU *cpu, instruction_T instruction) {      //  Shift Left Arithmetically the byte pointed to by [HL]. <--
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t carry_out = (hl_val >> 7) & 0x01;
    uint8_t shifted = (uint8_t)(hl_val << 1);

    external_write(gb, gb->cpu.reg.HL, shifted);

    (shifted == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);     // Z Flag
    (carry_out != 0) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);   // C Flag
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 16;

    // Bytes = 2
    // t-cycle = 16
    // Flags: SEE SLA r8
}
static void SRA_r8(GB *gb, CPU *cpu, instruction_T instruction) {        // Shift Right Arithmetically Register r8. -->
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };

    uint8_t reg_index = instruction.operand1 & 0x07;
    uint8_t r8_val = *reg_table[reg_index];
    uint8_t carry_out = r8_val & 0x01;
    uint8_t old_bit7 = r8_val & 0x80;
    uint8_t shifted = (uint8_t)((r8_val >> 1) | old_bit7);

    *reg_table[reg_index] = shifted;

    (shifted == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);     // Z Flag
    (carry_out != 0) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);   // C Flag
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycle = 8
    // Flags:
    // Z 0 0 C
}
static void SRA_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {      // Shift Right Arithmetically the byte pointed to by HL. -->
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t carry_out = hl_val & 0x01;
    uint8_t old_bit7 = hl_val & 0x80;
    uint8_t shifted = (uint8_t)((hl_val >> 1) | old_bit7);

    external_write(gb, gb->cpu.reg.HL, shifted);

    (carry_out != 0) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);      // C Flag
    (shifted == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);        // Z Flag
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);


    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 16;

    // Bytes = 2
    // t-cycle = 16
    // Flags: See SRA_r8
}
static void SRL_r8(GB *gb, CPU *cpu, instruction_T instruction) {        // Shift Right Logically Register r8. -->
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E,
        &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };

    uint8_t reg_index = instruction.operand1 & 0x07;
    uint8_t r8_val = *reg_table[reg_index];
    uint8_t carry_out = r8_val & 0x01;
    uint8_t shifted = (uint8_t)(r8_val >> 1);

    *reg_table[reg_index] = shifted;

    (shifted == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);     // Z Flag
    (carry_out != 0) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);   // C Flag
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycle = 8
    // Flags:
    // Z 0 0 C
}
static void SRL_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {      // Shift Right Logically the byte pointed to by [HL]. -->
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t carry_out = hl_val & 0x01;
    uint8_t shifted = (uint8_t)(hl_val >> 1);

    external_write(gb, gb->cpu.reg.HL, shifted);

    (shifted == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);     // Z Flag
    (carry_out != 0) ? set_cpu_flag(gb, FLAG_C) : clear_cpu_flag(gb, FLAG_C);   // C Flag
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 16;

    // Bytes = 2
    // t-cycle = 16
    // FLAGS: See SRL_r8
}


// PREFIXED Swap instructions
static void SWAP_r8(GB *gb, CPU *cpu, instruction_T instruction) {       // Swap the upper 4 bits in register r8 and the lower 4 ones. X::Y == Y::X
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E, &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t reg_index = instruction.operand1 & 0x07;  // Provides 0-7 Index to Match Register
    uint8_t r8_val = *reg_table[reg_index];
    uint8_t swapped = (uint8_t)((r8_val << 4) | (r8_val >> 4));

    *reg_table[reg_index] = swapped;

    (swapped == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);
    clear_cpu_flag(gb, FLAG_C);

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycle = 8
    // Flags:
    // Z 0 0 0
}
static void SWAP_p_HL(GB *gb, CPU *cpu, instruction_T instruction) {     // Swap the upper 4 bits in the byte pointed by HL and the lower 4 ones.
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    uint8_t swapped = (uint8_t)((hl_val << 4) | (hl_val >> 4));

    external_write(gb, gb->cpu.reg.HL, swapped);

    (swapped == 0) ? set_cpu_flag(gb, FLAG_Z) : clear_cpu_flag(gb, FLAG_Z);
    clear_cpu_flag(gb, FLAG_N);
    clear_cpu_flag(gb, FLAG_H);
    clear_cpu_flag(gb, FLAG_C);

    gb->cpu.reg.PC += 2;   // CB sub-opcode byte
    gb->cpu.cycle = 16;

    // Bytes = 2
    // t-cycle = 16

    // FLAGS: See SWAP_r8
}

// PREFIXED Bit Flag instructions
static void BIT_u3_r8(GB *gb, CPU *cpu, uint8_t cb_opcode) {     // Test bit u3 in register r8 set the zero flag if bit not set
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E, &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t u3_num = (cb_opcode >> 3) & 0x07;   // Acts as a divide by 8
    uint8_t reg_index = (cb_opcode & 0x07);     // Provides 0-7 Index to Match Register
    uint8_t r8_reg = *reg_table[reg_index];

    uint8_t get_state = ((r8_reg >> u3_num) & 1);
    (get_state) ? clear_cpu_flag(gb, FLAG_Z) : set_cpu_flag(gb, FLAG_Z); // Z flag, Set if specific r8 bit NOT set.

    clear_cpu_flag(gb, FLAG_N);  // Always clear N Flag. (Just GB CPU logic)
    set_cpu_flag(gb, FLAG_H);    // Always SET h Flag.   (Just GB CPU logic)
    // C flag unaffected.

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycle = 8

}
static void BIT_u3_p_HL(GB *gb, CPU *cpu, uint8_t cb_opcode) {   // Test bit u3 in the byte pointed by HL, set the flag if bit not set
    uint8_t u3_num = (cb_opcode >> 3) & 0x07;                  // Acts as a divide by 8
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);

    uint8_t get_state = ((hl_val >> u3_num) & 1);
    (get_state) ? clear_cpu_flag(gb, FLAG_Z) : set_cpu_flag(gb, FLAG_Z); // Z flag, Set if specific r8 bit NOT set.

    clear_cpu_flag(gb, FLAG_N);  // Always clear N Flag.
    set_cpu_flag(gb, FLAG_H);    // Always SET h Flag.
    // C flag unaffected.

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 12;

    // Bytes = 2
    // t-cycle = 12
}

// PREFIXED RES Instructions (Set a specific bit to 0?)
static void RES_u3_r8(GB *gb, CPU *cpu, uint8_t cb_opcode) {     // Set bit u3 in register r8 to 0. Bit 0 is the rightmost one, bit 7 the leftmost one
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E, &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t u3_num = (cb_opcode >> 3) & 0x07;   // Acts as a divide by 8
    uint8_t reg_index = (cb_opcode & 0x07);     // Provides 0-7 Index to Match Register

    *reg_table[reg_index] &= ~(1 << u3_num);     // Set bit at u3 index to 0.

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycle = 8
    // FLAGS: None affected
}
static void RES_u3_p_HL(GB *gb, CPU *cpu, uint8_t cb_opcode) {   // Set bit u3 in the byte pointed by HL to 0. Bit 0 is the rightmost one, bit 7 the leftmost one
    uint8_t u3_num = (cb_opcode >> 3) & 0x07;       // Acts as a divide by 8
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    hl_val &= ~(1 << u3_num);                   // Set bit at u3 index to 0.

    external_write(gb, gb->cpu.reg.HL, hl_val);

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 16;

    // Bytes = 2
    // t-cycle = 16
    // FLAGS: None affected
}
// PREFIXED SET instructions (Set bit to 1, at u3 Index?)
static void SET_u3_r8(GB *gb, CPU *cpu, uint8_t cb_opcode)  {     // Set bit u3 in register r8 to 1. Bit 0 is the rightmost one, bit 7 the leftmost one
    uint8_t *reg_table[8] = {
        &gb->cpu.reg.B, &gb->cpu.reg.C, &gb->cpu.reg.D, &gb->cpu.reg.E, &gb->cpu.reg.H, &gb->cpu.reg.L, NULL, &gb->cpu.reg.A
    };
    uint8_t u3_num = (cb_opcode >> 3) & 0x07;   // Acts as a divide by 8
    uint8_t reg_index = (cb_opcode & 0x07);     // Provides 0-7 Index to Match Register

    *reg_table[reg_index] |= (uint8_t)(1 << u3_num);     // Sets a single bit to 1, in the Index of r8 Register.

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 8;

    // Bytes = 2
    // t-cycle = 8
    // FLAGS: None affected
}
static void SET_u3_p_HL(GB *gb, CPU *cpu, uint8_t cb_opcode) {   // Set bit u3 in the byte pointed by HL to 1. Bit 0 is the rightmost one, bit 7 the leftmost one
    uint8_t u3_num = (cb_opcode >> 3) & 0x07;       // Acts as a divide by 8
    uint8_t hl_val = external_read(gb, gb->cpu.reg.HL);
    hl_val |= (1 << u3_num);

    external_write(gb, gb->cpu.reg.HL, hl_val);

    gb->cpu.reg.PC += 2;
    gb->cpu.cycle = 16;

    // Bytes = 2
    // t-cycle = 16
    // FLAGS: None affected
}






/// NOTICE:
// CB Prefixed BIT, RES, SET, Handlers.

// Handlers help point to the correct function (RES X, r8), (RES X, [HL])..
// AND which parameters for each Function:
// BIT 0, B. or BIT 4, H.
// RES 4, C. or Set 7 C.
static void CB_BIT_Handler(GB *gb, CPU *cpu, instruction_T instruction){
    //printf("CB BIT handler called.                      ; Calling Sub-Instruction\n");
    uint8_t cb_opcode = instruction.operand1;
    ((cb_opcode & 0x07) == 0x06) ? BIT_u3_p_HL(gb, cpu, cb_opcode)
                                 : BIT_u3_r8(gb, cpu, cb_opcode);
}

static void CB_RES_Handler(GB *gb, CPU *cpu, instruction_T instruction){
    //printf("CB RES handler called.                      ; Calling Sub-Instruction\n");
    uint8_t cb_opcode = instruction.operand1;
    ((cb_opcode & 0x07) == 0x06) ? RES_u3_p_HL(gb, cpu, cb_opcode)
                                 : RES_u3_r8(gb, cpu, cb_opcode);
}

static void CB_SET_Handler(GB *gb, CPU *cpu, instruction_T instruction){
    //printf("CB SET handler called.                      ; Calling Sub-Instruction\n");
    uint8_t cb_opcode = instruction.operand1;
    ((cb_opcode & 0x07) == 0x06) ? SET_u3_p_HL(gb, cpu, cb_opcode)
                                 : SET_u3_r8(gb, cpu, cb_opcode);
}

// Prefixed Opcodes.
static opcode_t *cb_opcodes[256] = {
    [0x00 ... 0x05] = RLC_r8, [0x06] = RLC_p_HL, [0x07] = RLC_r8, [0x08 ... 0x0D] = RRC_r8, [0x0E] = RRC_p_HL, [0x0F] = RRC_r8,
    [0x10 ... 0x15] = RL_r8,  [0x16] = RL_p_HL,  [0x17] = RL_r8,  [0x18 ... 0x1D] = RR_r8,  [0x1E] = RR_p_HL,  [0x1F] = RR_r8,
    [0x20 ... 0x25] = SLA_r8, [0x26] = SLA_P_HL, [0x27] = SLA_r8, [0x28 ... 0x2D] = SRA_r8, [0x2E] = SRA_p_HL, [0x2F] = SRA_r8,
    [0x30 ... 0x35] = SWAP_r8,[0x36] = SWAP_p_HL,[0x37] = SWAP_r8,[0x38 ... 0x3D] = SRL_r8, [0x3E] = SRL_p_HL, [0x3F] = SRL_r8,
    [0x40 ... 0x7F] = CB_BIT_Handler,
    [0x80 ... 0xBF] = CB_RES_Handler,
    [0xC0 ... 0xFF] = CB_SET_Handler,
};

static void CB_PREFIX(GB *gb, CPU *cpu, instruction_T instruction) {
    // RLC, RL, RRC, RR, SRA, SRL, SWAP, BIT, RES, SET
    uint8_t cb_opcode = external_read(gb, gb->cpu.reg.PC +1);  // Advanced PC by 1!
    instruction.operand1 = cb_opcode;

    cb_opcodes[cb_opcode](gb, cpu, instruction);
}


// Individual CPU instructions/ OP Codes
static opcode_t *opcodes[256] = {
/*  ---> X0, X1, X2, X3, X4 ... XB .. XF etc */
/* 0X */ NOP,        LD_r16_n16,    LD_p_r16_A, INC_r16,  INC_r8,     DEC_r8,  LD_r8_n8,   RLCA,     /* || */ LD_p_a16_SP,  ADD_HL_r16, LD_A_p_r16,  DEC_r16,   INC_r8,     DEC_r8,   LD_r8_n8,   RRCA,
/* 1X */ STOP,       LD_r16_n16,    LD_p_r16_A, INC_r16,  INC_r8,     DEC_r8,  LD_r8_n8,   RLA,      /* || */ JR_e8,        ADD_HL_r16, LD_A_p_r16,  DEC_r16,   INC_r8,     DEC_r8,   LD_r8_n8,   RRA,
/* 2X */ JR_cc_e8,   LD_r16_n16,    LD_p_HLI_A, INC_r16,  INC_r8,     DEC_r8,  LD_r8_n8,   DAA,      /* || */ JR_cc_e8,     ADD_HL_r16, LD_A_p_HLI,  DEC_r16,   INC_r8,     DEC_r8,   LD_r8_n8,   CPL,
/* 3X */ JR_cc_e8,   LD_r16_n16,    LD_p_HLD_A, INC_r16,  INC_p_HL,    DEC_p_HL, LD_p_HL_n8, SCF,      /* || */ JR_cc_e8,     ADD_HL_r16, LD_A_p_HLD,  DEC_r16,   INC_r8,     DEC_r8,   LD_r8_n8,   CCF,
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


static void interrupt_stack_push(GB *gb, uint16_t write_val) {
    gb->cpu.reg.SP --;
    mmu_write(gb, gb->cpu.reg.SP, (write_val >>8) & 0xFF);

    gb->cpu.reg.SP --;
    mmu_write(gb, gb->cpu.reg.SP, write_val & 0xFF);
}

static void cpu_service_interrupt(GB *gb, uint8_t interrupt_bit, uint16_t vector){
    // Disable any further interrupts.
    gb->cpu.state.IME = 0;

    // CPU exit halt if halted.
    gb->cpu.state.halt = 0;

    // Clear Interrupt Flag (Request flag)
    gb->interrupts.IF &= (uint8_t)~(1 << interrupt_bit);

    // Push Current PC into SP stack.
    interrupt_stack_push(gb, gb->cpu.reg.PC);

    gb->cpu.reg.PC = vector;
}

uint8_t cpu_interrupt_handling(GB *gb) {
    /*
    Interrupt handling quirks:
    Check if a bit matches in IE and IF.
        If there is NO pending interrupts:
            => Just return, nothing to do.
        If CPU is halted & there is NO pending interrupts:
            => Return. STAY HALTED, Burn CPU Cycles.
        If CPU is Halted & there IS pending interrupts:
            => Clear Halt. (Regardless clear halt)
            Case:
                IME = 0 => Return
                IME = 1 => Process interrupt.
    */

    // BOTH IE and IF must have at least 1 bit that match to qualify as a 'pending interrupt'.
    uint8_t pending_interrupt = (gb->interrupts.IE & gb->interrupts.IF) & 0x1F;

    if (pending_interrupt == 0) {
        // There is no pending interrupts. If halted, do not clear just return.
        return 0;
    }
    // === There is a Pending Interrupt ===
    if (gb->cpu.state.halt) {   // Clear the halt state, as the 'pending interrupt'
        gb->cpu.state.halt = 0;
    }
    if (!gb->cpu.state.IME) { // Process Interrupt only if IME is set (IME = 1).
        return 0; // IME is 0, just return.
    }

    // VBLANK = 0, LCD_STAT = 1, TIMER = 2, SERIAL = 3, JOYPAD = 4
    // Pending interrupts need to be processed in order (lowest to highest)
    if (pending_interrupt & (1u << 0)) {
        cpu_service_interrupt(gb, 0, 0x40);
        return 1;
    }
    if (pending_interrupt & (1u << 1)) {
        cpu_service_interrupt(gb, 1, 0x48);
        return 1;
    }
    if (pending_interrupt & (1u << 2)) {
        cpu_service_interrupt(gb, 2, 0x50);
        return 1;
    }
    if (pending_interrupt & (1u << 3)) {
        cpu_service_interrupt(gb, 3, 0x58);
        return 1;
    }
    if (pending_interrupt & (1u << 4)) {
        cpu_service_interrupt(gb, 4, 0x60);
        return 1;
    }
    printf("Problem servicing interrupt, returning 0\n");
    return 0;
}

static CpuSnapshot cpu_snapshot(GB *gb) {
    CpuSnapshot snap;

    snap.a = gb->cpu.reg.A;
    snap.f = gb->cpu.reg.F;
    snap.b = gb->cpu.reg.B;
    snap.c = gb->cpu.reg.C;
    snap.d = gb->cpu.reg.D;
    snap.e = gb->cpu.reg.E;
    snap.h = gb->cpu.reg.H;
    snap.l = gb->cpu.reg.L;
    snap.pc = gb->cpu.reg.PC;
    snap.sp = gb->cpu.reg.SP;

    snap.mem_sp = external_read(gb, snap.sp);
    snap.mem_sp_plus_1 = external_read(gb, (uint16_t)(snap.sp + 1));

    return snap;
}



void add_crash_entry(GB *gb) {
    TraceEntry new_entry;

    new_entry.opcode = gb->instruction.opcode;
    new_entry.operand1 = gb->instruction.operand1;
    new_entry.operand2 = gb->instruction.operand2;

    new_entry.ime = gb->cpu.state.IME;
    new_entry.ime = gb->cpu.state.IME;
    new_entry.iE = gb->interrupts.IE;
    new_entry.iF = gb->interrupts.IE;

    new_entry.instruction_count = gb->step_count;

    // Registries:
    new_entry.af = gb->cpu.reg.AF;
    new_entry.bc = gb->cpu.reg.AF;
    new_entry.de = gb->cpu.reg.AF;
    new_entry.hl = gb->cpu.reg.AF;
    new_entry.pc = gb->cpu.reg.PC;
    new_entry.sp = gb->cpu.reg.SP;


    trace_buffer_push(&gb->trace_buffer, new_entry);
}


int execute_instruction(GB *gb, CPU *cpu, instruction_T instruction){

    void add_crash_entry(GB *gb);
    opcodes[instruction.opcode](gb, &gb->cpu, instruction);

    if (gb->cpu.cycle == 0) {
        if (instruction.opcode != 0x76) {
            printf("Op-code has no cycle: %02X abort.\n", instruction.opcode);
            gb->panic = 1;
        }
    }
    return 0;
}







int execute_test(GB *gb, CPU *cpu, instruction_T instruction) {
    //printf("(EXTRA DETAIL) PC=%04X, OPCODE=%02X, OP1=0x%02X, OP2=0x%02X\n", gb->cpu.reg.PC, instruction.opcode, instruction.operand1, instruction.operand2);

    //run_test_debug(cpu);
    opcodes[instruction.opcode](gb, cpu, instruction);

    if (cpu->state.panic == 1) { printf("ENCOUNTERED PANIC, Instruction not yet ready.\n"); return -1; }
    else {
        printf("%sExecuted Test Instruction.%s\n", KYEL, KNRM);
        //run_test_debug(cpu);
        return 0;
    }
    return 0;
}