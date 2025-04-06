#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cpu_instructions.h"



//typedef void opcode_t(CPU *cpu, instruction_T instrc);
//extern CPU *loc_cpu;

//extern instruction_T op_instruction;


typedef void opcode_t(CPU *cpu, instruction_T instrc);




// INTERUPT Instructions:
// HALT!
static void HALT(CPU *cpu, instruction_T instrc) {      // Likely completely HALT / kill the system.
    printf("HALT Called. Exit Now.. (Guessing)\n");
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
    printf("NOP Called. DO Nothing..\n");
}

// STOP
static void STOP(CPU *cpu, instruction_T instrc) {      // Unsure, might be like Pause.

}

// BLANK
static void BLANK(CPU *cpu, instruction_T instrc) {      // Do nothing, basically NOP, but for clarity don't write it like that.
    // DO NOTHING - Not even any command.
}

static void CB_PREFIX(CPU *cpu, instruction_T instrc) {  // Ummmm Maybe points to Table 2 in the OP_CODE Map

    printf("CB Prefix call.... blehhhhh was hoping this would be later... bleh\nI don't know how this works or data works. \n");
    // Entry point to Table 2 (Prefix) OP_CODES?

    // Should be only 1byte code. so.. umm huh..
    


    // IE: RLC, RL, RRC, RR, SRA, SRL, SWAP, BIT, RES, SET

}









// Load instructions
/// NOTICE: Moved LD r8 r8 to the macro function
// static void LD_r8_r8(uint8_t *gb, uint8_t opcode) {     // This is insanely Common instruction. B => C, A=> L, C=>D etc. Which is copied 40 + times in CPU instructions. 

// }

// THIS 1 Instruction don't actually use. Cause it needs extra logic. (I think...)
// static void ld_r8_n8(uint8_t *gb, uint8_t opcode) {
    
// }

static void LD_hr_n8(CPU *cpu, instruction_T instrc) {      // Copy Byte Value into HR (High) 8 byte Register IE: B, D, H
    printf("copy n8 Value (8bit rom/ram data), into HR (HIGH Range Byte, B, D, H.. etc)\n");
    switch (instrc.opcode) {
        case 0x06:
            cpu->B = instrc.operand1;
            break;
        case 0x07:
            cpu->C = instrc.operand1;
            break;
        case 0x08:
            cpu->D = instrc.operand1;
            break;
    }
    cpu->PC += 2;
    // No flags affected.
}

static void LD_lr_n8(CPU *cpu, instruction_T instrc) {      // Copy Byte Value into LR (Low) 8 byte Register IE: C, E, L
    
}


// static void LD_r16_r16(CPU *cpu, instruction_T instrc) {       // Simple Instruction. Copy Data from 16byte Register into 16Byte Register. BC => HL. HL => BC etc
    
// }

static void LD_r16_n16(CPU *cpu, instruction_T instrc) {
    printf("Copy n16 value into r16 Register\n");
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
    
}


static void LD_p_HL_n8(CPU *cpu, instruction_T instrc) {       // Copy data from n8, into where HL is being pointed to
    
}

static void LD_r8_p_HL(CPU *cpu, instruction_T instrc) {       // Copy data inside HL (that is being pointed to it) Into Register r8 (Needs to know range. HR, or LR (B, D, H) or (C, E, L))
    
}





//// Accumulator related LOAD instructions:

static void LD_p_r16_A(CPU *cpu, instruction_T instrc) {
    
}
static void LD_p_n16_A(CPU *cpu, instruction_T instrc) {
    
}
static void LD_A_p_r16(CPU *cpu, instruction_T instrc) {
    
}
static void LD_A_p_n16(CPU *cpu, instruction_T instrc) {
    
}
// LDH (A) Load Instructions
static void LDH_p_n16_A(CPU *cpu, instruction_T instrc) {
    
}
static void LDH_p_C_A(CPU *cpu, instruction_T instrc) {
    
}
static void LDH_A_p_n16(CPU *cpu, instruction_T instrc) {
    
}
static void LDH_A_p_C(CPU *cpu, instruction_T instrc) {
    
}
// LD/ Load (A) with Increment and Decrement Instructions:
static void LD_p_HLI_A(CPU *cpu, instruction_T instrc) {
    
}
static void LD_p_HLD_A(CPU *cpu, instruction_T instrc) {
    
}
static void LD_A_p_HLD(CPU *cpu, instruction_T instrc) {
    
}
static void LD_A_p_HLI(CPU *cpu, instruction_T instrc) {
    
}



// LD/Load (weird) SP instructions:
static void LD_SP_n16(CPU *cpu, instruction_T instrc) {
    
}
static void LD_p_n16_SP(CPU *cpu, instruction_T instrc) {
    
}
static void LD_HL_SP_Pe8(CPU *cpu, instruction_T instrc) {     // Load value in SP + (8bit (e) SIGNED int) into HL Register
    
}
static void LD_SP_HL(CPU *cpu, instruction_T instrc) {
    
}


// Jump instructions
static void JP_HL(CPU *cpu, instruction_T instrc) {    // Copy Address in HL to PC
    printf("Jump to Address in HL\n");

    cpu->PC = cpu->HL;      // Not sure if this is in wrong order.. Might be..


    // Bytes 1
    // Cycles 1
    // Flags Changed, none

    // Jump to address in HL; effectively, copy the value in register HL into PC.
    // COPY HL into PC
}
static void JP_n16(CPU *cpu, instruction_T instrc) {                // Copy Address into PC from n16 value

}

// JR (Relative Jump) Instructions:                 //  NOTE: Must be within -128 bytes and 127 bytes from current address in PC
static void JP_cc_n16(CPU *cpu, instruction_T instrc) {             // Copy address into PC from n16 value.. IF conditions met.

}
static void JR_e8(CPU *cpu, instruction_T instrc) {
    // NOTICE values like e8, needs to have the bit retreaved, from the memory. THEN CAST! Into an int8_t value (Not uint!) So it can have -128 to +127 memory offset.
    // NOTE: int8_t != uint8_t  THESE ARE VERY DIFFERNT! (Alows for -negative and +positive values)

    int8_t e_address;       // e = signed 8bit register. This is required. Becaues it needs to be able to have Negative or positive Values.

    // Note. This is basically the same as jr_n16 ... but... For clarity I'm spliting up the functions.


    // Cycles 3
    // Bytes 2
    // Flags Changed, none

}
static void JR_cc_e8(CPU *cpu, instruction_T instrc) {
    // NOTICE values like e8, needs to have the bit retreaved, from the memory. THEN CAST! Into an int8_t value (Not uint!) So it can have -128 to +127 memory offset.
    // NOTE: int8_t != uint8_t  THESE ARE VERY DIFFERNT! (Alows for -negative and +positive values)

    printf("CPU_INSTRUCTIONS: Relative Jump\n");

    int8_t e_signed_offset;       // e = signed 8bit register. This is required. Becaues it needs to be able to have Negative or positive Values.
    e_signed_offset = (int8_t)instrc.operand1;


    switch (instrc.opcode) {
        case 0x20:                  // JR, NZ e8
            // (cpu->F & FLAG_Z) ? /* NOP */  : /* Jump */ 
            if (!(cpu->F & FLAG_Z)) { 
                printf("Z flag not set, JR NZ e8 Condition Met -> Relative jump %02X\n", instrc.operand1);
                cpu->PC += e_signed_offset;
            }
            break;
        case 0x28:
            if (cpu->F & FLAG_Z) { 
                printf("Z Set, JR Z e8 Condition Met -> Relative jump %02X\n", instrc.operand1);
                cpu->PC += e_signed_offset;
            }
            break;
        case 0x30:
            if (!(cpu->F & FLAG_C)) { 
                printf("C Not Set, JR NC e8 Condition Met -> Relative jump %02X\n", instrc.operand1);
                cpu->PC += e_signed_offset;
            }
            break;
        case 0x38:
            if (cpu->F & FLAG_C) { 
                printf("C Set, JR C e8 Condition Met -> Relative jump %02X\n", instrc.operand1);
                cpu->PC += e_signed_offset;
            }
            break;
    }

    // Note. This is basically the same as jr_cc_n16 ... but... For clarity I'm spliting up the functions.

    // Cycles: 3 taken / 2 untaken
    // Bytes: 2
    // Flags Changed, None
}
static void JR_n16(CPU *cpu, instruction_T instrc) {   // Relative Jump to 16 byte address (Must be close enough)
    // Cycles 3
    // Bytes 2
    // Flags Changed, none

    // The address is encoded as a signed 8-bit offset from the address immediately following the JR instruction, so the target address n16 must be between -128 and 127 bytes away. For example:
    // Example: 
    // JR Label  ; no-op; encoded offset of 0
    // JR Label  ; infinite loop; encoded offset of -2
}
static void JR_cc_n16(CPU *cpu, instruction_T instrc) {    // Relative Jump to 16 Byte Address (Must be close enough). As long as CC Conditions met.
    // Relative Jump to address n16 if condition cc is met.
    // Cycles: 3 taken / 2 untaken
    // Bytes: 2
    // Flags Changed, None
}



// Subroutine Instructions:
static void CALL_n16(CPU *cpu, instruction_T instrc) {      // Pushes the address of the instruction after the CALL, on the stack. Such that RET can pop it later; Then it executes implicit JP n16

}
static void CALL_cc_n16(CPU *cpu, instruction_T instrc) {   // Call address n16 if condition cc is met.

}
static void RET(CPU *cpu, instruction_T instrc) {           // RETurn from subroutine.
    // This is basically a POP PC instruction (If such an instruction existed). See POP r16 for an explanation of how POP works.

    // FLAGS: None affected
}
static void RET_cc(CPU *cpu, instruction_T instrc) {        // RETurn from subroutine if condition CC is met
    
    // FLAGS: None affected
}
static void RETI(CPU *cpu, instruction_T instrc) {          // RETurn from subroutine and enable I-nterupts.
    // This is basically equivalent to executing EI then RET, meaning that IME is set right after this instruction.
    
    // FLAGS: None affected
}
static void RST_vec(CPU *cpu, instruction_T instrc) {       // Call address vec. This is a shorter and faster equivalent to CALL for suitable values of vec.

    // FLAGS: None affected
}











// Increment / Decrement Registers:
static void INC_hr8(CPU *cpu, instruction_T instrc) {    // Increment High bit Register  (++ => B, D, H)
    printf("Increment High Register 8bit val\n");
    uint8_t og_reg_val = 0;

    switch (instrc.opcode) {
        case 0x04:
            ((cpu->B & 0x0F) == 0x0F) ? set_flag(2) : clear_flag(2);    // Set H Flag for overlow from bit 3    
            cpu->B ++;
            (cpu->B == 0) ? set_flag(0) : clear_flag(0);                // Set (Z) Zero Flag
            break;
        case 0x14:
            ((cpu->D & 0x0F) == 0x0F) ? set_flag(2) : clear_flag(2);    // Set H Flag for overlow from bit 3
            cpu->D ++;
            (cpu->D == 0) ? set_flag(0) : clear_flag(0);                // Set (Z) Zero Flag
            break;
        case 0x24:
            ((cpu->H & 0x0F) == 0x0F) ? set_flag(2) : clear_flag(2);    // Set H Flag for overlow from bit 3
            cpu->H ++;
            (cpu->H == 0) ? set_flag(0) : clear_flag(0);                // Set (Z) Zero Flag
            break;
    }
    clear_flag(1);     // Sub Flag always cleared for INC.

    // Flags: Z 0 H -
    // Z = Set if result is 0.
    // N = 0
    // H = Set if overflow from bit 3. 
}
static void INC_lr8(CPU *cpu, instruction_T instrc) {    // Increment lower bit Register (++ => C, E, L)
    printf("Increment Low Register 8bit val\n");
       
    switch (instrc.opcode) {
        case 0xC0:
            ((cpu->C & 0x0F) == 0x0F) ? set_flag(2) : clear_flag(2);    // Set H Flag for overlow from bit 3
            cpu->C ++;
            (cpu->C == 0) ? set_flag(0) : clear_flag(0);    // Set (Z) Zero Flag
            break;
        case 0xC1:
            ((cpu->E & 0x0F) == 0x0F) ? set_flag(2) : clear_flag(2);    // Set H Flag for overlow from bit 3
            cpu->E ++;
            (cpu->E == 0) ? set_flag(0) : clear_flag(0);    // Set (Z) Zero Flag
            break;
        case 0xC2:
            ((cpu->L & 0x0F) == 0x0F) ? set_flag(2) : clear_flag(2);    // Set H Flag for overlow from bit 3
            cpu->L ++;
            (cpu->L == 0) ? set_flag(0) : clear_flag(0);    // Set/ Clear (Z) Zero Flag
            break;
        case 0xC3:
            ((cpu->A & 0x0F) == 0x0F) ? set_flag(2) : clear_flag(2);    // Set H Flag for overlow from bit 3
            cpu->A ++;
            (cpu->A == 0) ? set_flag(0) : clear_flag(0);    // Set/ Clear (Z) Zero Flag
            break;
    }

    clear_flag(1);      // Set subtraction Flag always set.
    cpu->PC ++;         // this is only 1 Byte.

    // Flags: Z 0 H -
    // Z = Set if result is 0.
    // N = 0
    // H = Set if overflow from bit 3. 
}

static void DEC_hr8(CPU *cpu, instruction_T instrc) {    // Decrement High bit Register  (-- => B, D, H)
    printf("Decrement High Register 8bit val\n");
       
    switch (instrc.opcode) {
        case 0x05:
            (cpu->B == 0) ? set_flag(2) : clear_flag(2);     // Set/Clear H borrow flag from bit 4.
            cpu->B --;
            (cpu->B == 0) ? set_flag(0) : clear_flag(0);;    // Set/Clear (Z) Zero Flag
            break;
        case 0x15:
            (cpu->D <= 0) ? set_flag(2) : clear_flag(2);     // Set/Clear H borrow flag from bit 4.
            cpu->D --;
            (cpu->D == 0) ? set_flag(0) : clear_flag(0);;    // Set/Clear (Z) Zero Flag
            break;
        case 0x25:
            (cpu->H == 0) ? set_flag(2) : clear_flag(2);     // Set/Clear H borrow flag from bit 4.
            cpu->H --;
            (cpu->H == 0) ? set_flag(0) : clear_flag(0);;    // Set/Clear (Z) Zero Flag
            break;
    }

    set_flag(1);        // Set subtraction Flag always set.
    cpu->PC ++;         // this is only 1 Byte.


    // Flags: Flags: Z 1 H - 
    // Z = Set if result is 0.
    // N = 1    // Subtraction flag always set.
    // H = Set if borrow from bit 4.
}
static void DEC_lr8(CPU *cpu, instruction_T instrc) {    // Decrement lower bit Register (-- => C, E, L)
    printf("Decrement Low Register 8bit val\n");
       
    switch (instrc.opcode) {
        case 0xD0:
            (cpu->C == 0) ? set_flag(2) : clear_flag(2);    // Set/Clear H borrow flag from bit 4.
            cpu->C --;
            (cpu->C == 0) ? set_flag(0) : clear_flag(0);    // Set/Clear (Z) Zero Flag
            break;
        case 0xD1:
            (cpu->E == 0) ? set_flag(2) : clear_flag(2);    // Set/Clear H borrow flag from bit 4.
            cpu->E --;
            (cpu->E == 0) ? set_flag(0) : clear_flag(0);    // Set/Clear (Z) Zero Flag
            break;
        case 0xD2:
            (cpu->L == 0) ? set_flag(2) : clear_flag(2);    // Set/Clear H borrow flag from bit 4.
            cpu->L --;
            (cpu->L == 0) ? set_flag(0) : clear_flag(0);    // Set/Clear (Z) Zero Flag
            break;
        case 0xD3:
            (cpu->A == 0) ? set_flag(2) : clear_flag(2);    // Set/Clear H borrow flag from bit 4.
            cpu->A --;
            (cpu->A == 0) ? set_flag(0) : clear_flag(0);    // Set/Clear (Z) Zero Flag
            break;
    }

    set_flag(1);        // Set subtraction Flag always set.
    cpu->PC ++;         // this is only 1 Byte.


    // Flags: Flags: Z 1 H - 
    // Z = Set if result is 0.
    // N = 1    // Subtraction flag always set.
    // H = Set if borrow from bit 4.
}
// Inc/ Dec, HL value inside pointer [ ]
static void INC_p_HL(CPU *cpu, instruction_T instrc) {  // Increment 16 bit HL register, The Value In Pointer ++ HL
    printf("INC [HL] (Val pointed to by HL)\n");

    uint8_t hl_val = external_read(cpu->HL);
    ((hl_val & 0x0F) == 0x0F) ? set_flag(2) : clear_flag(2);    // Check Val before DEC. If 0, it will need to borrow from bit 4. (So, then set H flag)
    
    hl_val ++;
    (hl_val == 0) ? set_flag(0) : clear_flag(0);                // Set/Clear (Z) Zero Flag.
    external_write(cpu->HL, hl_val);

    clear_flag(1);      // Clear subtraction flag (N)
    cpu->PC ++;         // this is only 1 Byte.


    // Z = Set if result is 0.
    // N = 0
    // H = Set if borrow from bit 4.
}
static void DEC_p_HL(CPU *cpu, instruction_T instrc) {      // Decrement 16 bit HL in register, The Value In Pointer -- HL
    printf("INC [HL] (Val pointed to by HL)\n");

    uint8_t hl_val = external_read(cpu->HL);
    (hl_val == 0) ? set_flag(2) : clear_flag(2);            // Check Val before DEC. If 0, it will need to borrow from bit 4. (So, then set H flag)
    
    hl_val --;
    (hl_val == 0) ? set_flag(0) : clear_flag(0);            // Set/Clear (Z) Zero Flag.
    external_write(cpu->HL, hl_val);

    set_flag(1);        // Set subtraction flag (N)
    cpu->PC ++;         // this is only 1 Byte.

    // Z = Set if result is 0.
    // N = 1
    // H = Set if borrow from bit 4.
}
// Full INC/DEC 16 bit Registers (BC, DE, HL):
static void INC_r16(CPU *cpu, instruction_T instrc) {

    //uint8_t register_id = (opcode >> 4) + 1;        // Shift opcode value RIGHT by 4 places, and add one. 
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
static void DEC_r16(CPU *cpu, instruction_T instrc) {
    //uint8_t register_id = (opcode >> 4) + 1;        // This is doing the same as INC_r16. (Why is it shifting it over, to get the actual Register ID?)
    // cycle_oam_bug(gb, register_id);              // Does some junk, to cycle this bug, so it doesn't happen (I assume)
    //gb->registers[register_id]--;                 // The actual DEC, --.. (In the gb->registers[<registers_ids>] )

    // 
    

    // FLAGS: NONE AFFECTED
}
// INC / Dec SP:
static void INC_SP(CPU *cpu, instruction_T instrc) {
    
    // FLAGS: NONE AFFECTED
}
static void DEC_SP(CPU *cpu, instruction_T instrc) {

    // FLAGS: NONE AFFECTED
}


// ComPARE Instructios:
static void CP_A_r8(CPU *cpu, instruction_T instrc) {       // ComPare -> value in pointer HL to A
    // Depending on the OP Code, it changes WHICH instruction is compared.
    printf(":CPU_INSTRUCTION: ComPare, values in 8bit Register (r8) to A\n");

    uint8_t op_c_reg = instrc.opcode;
    uint8_t r8_reg = 0;
    
    switch (op_c_reg) {
        case 0xB8:
            r8_reg = cpu->B;        // B8 = CP A, B
            break;
        case 0xB9:
            r8_reg = cpu->C;        // B9 = CP A, C
            break;
        case 0xBA:
            r8_reg = cpu->D;        // BA = CP A, D
            break;
        case 0xBB:
            r8_reg = cpu->E;        // BB = CP A, E
            break;
        case 0xBC:
            r8_reg = cpu->H;        // BC = CP A, H
            break;
        case 0xBD:
            r8_reg = cpu->L;        // BC = CP A, L
            break;
        case 0xBF:
            r8_reg = cpu->A;        // I want to say this is NOP, but maybe it does something weird with the CPU Flags.
            break;
    }

    
    
    // A - r8
    
    uint8_t result = (cpu->A - r8_reg);
    //cpu->A

    printf("Result of ComPare? %02X", result);

    if (result == 0)
        set_flag(0);
    else 
        clear_flag(0);
    
    set_flag(1);    // Set Subtraction Flag

    if ((cpu->A & 0x0F) < (r8_reg & 0x0F))
        set_flag(2);
    else
        clear_flag(2);

    if (cpu->A < r8_reg)
        set_flag(3);
    else
        clear_flag(3);

    // Thiss should work. But.. maybe not.. we'll see...
    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (IE: if r8 > A)
    */

    cpu->PC++;  // Advance the PC by only 1 (As no immediates or operands are provided)
}
static void CP_A_p_HL(CPU *cpu, instruction_T instrc) {     // ComPare -> value in pointer HL to A

}
static void CP_A_n8(CPU *cpu, instruction_T instrc) {       // ComPare -> value in n8 to A
    printf(":CPU_INSTRUCTION: ComPare, values in 8bit value (n8) to A\n");
    // A - r8
    uint8_t n8_val = instrc.operand1;
    printf(":CPU_INSTRUCTION: What is in Immediate/ Operand? %02X\n", instrc.operand1);
    uint8_t result = (cpu->A - instrc.operand1);


    printf("Result of ComPare (a n8)? %02X\n", result);

    if (result == 0)
        set_flag(0);
    else 
        clear_flag(0);
    
    set_flag(1);    // Set Subtraction Flag

    if ((cpu->A & 0x0F) < (n8_val & 0x0F))
        set_flag(2);
    else
        clear_flag(2);

    if (cpu->A < n8_val)
        set_flag(3);
    else
        clear_flag(3);



    
    printf("Finished ComPare..\n");
    //cpu->A
    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (IE: if r8 > A)
    */
   cpu->PC += 2;  // Advance the PC by 2 (1 Byte OP, 1 Byte Operand)
}

// Special CPL instruction:
static void CPL(CPU *cpu, instruction_T instrc) {           // ComPLement accumulator (A = ~A); also called bitwise NOT

}


// DAA (WEIRD INSTRUCTION) -- VERY complicated what it actually does!
static void DAA(CPU *cpu, instruction_T instrc) {
    
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
static void ADD_A_r8(CPU *cpu, instruction_T instrc) {      // Add value of r8 into A           (NOTICE there is a lot of these instructions, Maybe simplfy it if possible.)
    
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = Set if overflow bit 3
        C = Set if overflow bit 7
    */
}
static void ADD_A_p_HL(CPU *cpu, instruction_T instrc) {    // Add value pointed by HL into A

    // FLAGS: SEE add_A_r8
}
static void ADD_A_n8(CPU *cpu, instruction_T instrc) {

    // FLAGS: SEE add_A_r8
}
static void ADD_HL_r16(CPU *cpu, instruction_T instrc) {

    /*
        FLAGS:
        Z = --
        N = 0
        H = Set if overflow bit 3
        C = Set if overflow bit 7
    */
}
static void ADD_HL_SP(CPU *cpu, instruction_T instrc) {     // Add the value in SP to HL

    // FLAGS: SEE ADD_HL_r16
}
static void ADD_SP_e8(CPU *cpu, instruction_T instrc) {     // e8 = SIGNED int. So technically same as sp_n16

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
static void ADC_A_r8 (CPU *cpu, instruction_T instrc) {

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = Set if overflow bit 3.
        C = Set if overflow bit 7
    */
}
static void ADC_A_p_HL (CPU *cpu, instruction_T instrc) {   // Subtract the byte pointed to by HL and the carry flag from A.

    // FLAGS: see adc_A_r8
}
static void ADC_A_n8 (CPU *cpu, instruction_T instrc) {     // Subtract the value n8 and the carry flag from A.

    // FLAGS: see adc_A_r8
}




// Subtraction Instructions:
static void SUB_A_r8 (CPU *cpu, instruction_T instrc) {     // Subtract values in a, by 8byte register

    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (i.e. if r8 > A).
    */
}
static void SUB_A_p_HL(CPU *cpu, instruction_T instrc) {

    // FLAGS: See sub_aAr8
}
static void SUB_A_n8(CPU *cpu, instruction_T instrc) {

    // FLAGS: See sub_A_r8
}


// Special SBC (Sub with the cary flag):
static void SBC_A_r8 (CPU *cpu, instruction_T instrc) {     // Subtract the value in r8 and the carry flag from A.

    /*
        FLAGS:
        Z = Set if result is 0
        N = 1
        H = Set if borrow from bit 4
        C = Set if borrow (i.e. if (r8 + carry) > A)
    */
}
static void SBC_A_p_HL (CPU *cpu, instruction_T instrc) {   // Subtract the byte pointed to by HL and the carry flag from A.

    // FLAGS: see sbc_a_r8    
}
static void SBC_A_n8 (CPU *cpu, instruction_T instrc) {     // Subtract the value n8 and the carry flag from A.

    // FLAGS: see sbc_a_r8    
}



/// STACK Manipulation Instructions:


static void POP_AF(CPU *cpu, instruction_T instrc) {        // Pop register AF from the stack.
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
    printf("POP r16 register. POP Is weird... need to do something thinking\n");
    /*
    This is roughly equivalent to the following imaginary instructions:
        LD LOW(r16), [SP]   ; C, E or L
        INC SP
        LD HIGH(r16), [SP]  ; B, D or H
        INC SP
    */

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

    /*
    This is roughly equivalent to the following imaginary instructions:
        DEC SP
        LD [SP], A
        DEC SP
        LD [SP], F.Z << 7 | F.N << 6 | F.H << 5 | F.C << 4    // NOTE: ONLY! if the FLAG is stored in Bools
    */

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

    cpu->PC ++; // Push r16 is only 1 byte.

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
static void AND_A_r8(CPU *cpu, instruction_T instrc) {      // Set A to the bitwise AND between the value in r8 and A

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = 0
    */
}
static void AND_A_p_HL(CPU *cpu, instruction_T instrc) {    // Set A to the bitwise AND between the byte pointed to by HL and A

    // FLAGS: see: AND A, r8
}
static void AND_A_n8(CPU *cpu, instruction_T instrc) {      // Set A to the bitwise AND between the value n8 and A

    // FLAGS: see: AND A, r8
}
// OR Instructions:
static void OR_A_r8(CPU *cpu, instruction_T instrc) {       // Set A to the bitwise OR between the value in r8 and A

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = 0
    */
}
static void OR_A_p_HL(CPU *cpu, instruction_T instrc) {     // Set A to the bitwise OR between the byte pointed to by HL and A

    // FLAGS: see: OR_A_r8
}
static void OR_A_n8(CPU *cpu, instruction_T instrc) {       // Set A to the bitwise OR between the value n8 and A

    // FLAGS: see: OR_A_r8   
}
// XOR Instructions:
static void XOR_A_r8(CPU *cpu, instruction_T instrc) {      // Set A to the bitwise XOR between the value in r8 and A

    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = 0
    */
}
static void XOR_A_p_HL(CPU *cpu, instruction_T instrc) {    // Set A to the bitwise XOR between the byte pointed to by HL and A

    // FLAGS: see: XOR_A_r8
}
static void XOR_A_n8(CPU *cpu, instruction_T instrc) {      // Set A to the bitwise XOR between the value n8 and A

    // FLAGS: see: XOR_A_r8
}

// Bit Flag instructions:
static void BIT_u3_r8(CPU *cpu, instruction_T instrc) {     // Test bit u3 in register r8m set the zero flag if bit not set
    /*
        FLAGS:
        Z = Set if the select bit is 0
        N = 0
        H = 1
        C = ---
    */
}
static void BIT_u3_p_HL(CPU *cpu, instruction_T instrc) {   // Test bit u3 in the byte pointed by HL, set the flag if bit not set
    // FLAGS: See BIT_u3_r8
}
static void RES_u3_r8(CPU *cpu, instruction_T instrc) {     // Set bit u3 in register r8 to 0. Bit 0 is the rightmost one, bit 7 the leftmost one
    // FLAGS: None affected
}
static void RES_u3_p_HL(CPU *cpu, instruction_T instrc) {   // Set bit u3 in the byte pointed by HL to 0. Bit 0 is the rightmost one, bit 7 the leftmost one
    // FLAGS: None affected
}
static void SET_u3_r8(CPU *cpu, instruction_T instrc) {     // Set bit u3 in register r8 to 1. Bit 0 is the rightmost one, bit 7 the leftmost one
    // FLAGS: None affected
}
static void SET_u3_p_HL(CPU *cpu, instruction_T instrc) {   // Set bit u3 in the byte pointed by HL to 1. Bit 0 is the rightmost one, bit 7 the leftmost one
    // FLAGS: None affected
}

// Weird Bitshift Instructions
static void RL_r8(CPU *cpu, instruction_T instrc) {         // Rotate bits in register r8 left, through the carry flag
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RL_p_HL(CPU *cpu, instruction_T instrc) {       // Rotate the byte pointed to by HL left, through the carry flag
    // FLAGS: See RL_r8
}
static void RLA(CPU *cpu, instruction_T instrc) {           // Rotate register A left, through the carry flag
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RLC_r8(CPU *cpu, instruction_T instrc) {        // Rotate registers r8 left
    /*
        FLAGS:
        Z = 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RLC_p_HL(CPU *cpu, instruction_T instrc) {      // Rotate the byte pointed to by HL left
    // FLAGS: See RLC_r8

}
static void RLCA(CPU *cpu, instruction_T instrc) {          //Rotate Register a left

}
static void RR_r8(CPU *cpu, instruction_T instrc) {         // Rotate Register r8 right, through the carry flag

    /*
        FLAGS:
        Z = 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RR_p_HL(CPU *cpu, instruction_T instrc) {       // Rotate the byte pointed to by HL right, through the carry flag

    // FLAGS: see RR_r8
}
static void RRA(CPU *cpu, instruction_T instrc) {           // Rotate register A <- Right. through the carry flag
    /*
        FLAGS:
        Z = 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void RRC_r8(CPU *cpu, instruction_T instrc) {        // Rotate register r8 <- Right
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
static void RRC_p_HL(CPU *cpu, instruction_T instrc) {      // Rotate the byte pointed to by HL <- Right
    // FLAGS: See RRC r8

}
static void RRCA(CPU *cpu, instruction_T instrc) {          // Rotate register A <- Right
    printf("RRCA Command, 'Rotate register A right?' hmmm\n");


}
static void SLA_r8(CPU *cpu, instruction_T instrc){         // Shift Left Arithmetically register r8
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void SLA_P_HL(CPU *cpu, instruction_T instrc) {      //  Shift Left Arithmetically the byte pointed to by HL.
    // Flags: SEE SLA r8
}
static void SRA_r8(CPU *cpu, instruction_T instrc) {        // Shift Right Arithmetically Register r8 (bit 7, of r8 is unchanged)
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */
}
static void SRA_p_HL(CPU *cpu, instruction_T instrc) {      // Shift Right Arithmetically the byte pointed to by HL (Bit 7 of the byte pointed to by HL is unchanged)
    // Flags: See SRA_r8
}
static void SRL_r8(CPU *cpu, instruction_T instrc) {        // Shift Right Logically register r8
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = Set according to result
    */    
}
static void SRL_p_HL(CPU *cpu, instruction_T instrc) {      // Shift Right Logically the byte pointed to by HL
    // FLAGS: See SRL_r8
}
static void SWAP_r8(CPU *cpu, instruction_T instrc) {       // Swap the upper 4 bits in register r8 and the lower 4 ones
    /*
        FLAGS:
        Z = Set if result is 0
        N = 0
        H = 0
        C = 0
    */    
}
static void SWAP_p_HL(CPU *cpu, instruction_T instrc) {     // Swap the upper 4 bits in the byte pointed by HL and the lower 4 ones.
    // FLAGS: See SWAP_r8

}

// Carry Flag Instructions:
static void CCF(CPU *cpu, instruction_T instrc) {           // Complement Carry Flag
    /*
        FLAGS:
        Z = --
        N = 0
        H = 0
        C = Inverted
    */     
}
static void SCF(CPU *cpu, instruction_T instrc) {           // Set Carry Flag
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
static void LD_p_a16_A(CPU *cpu, instruction_T instrc) {
    printf("LDH [a16] A, Called => Copy A Register into location of (a16) \n");
    
    uint16_t a16 = cnvrt_lil_endian(instrc.operand1, instrc.operand2);
    
    printf(":LDH: a16 Location val %04X\n", a16);
    external_write(a16, cpu->A);

    cpu->PC +=3;

}
static void LD_A_p_a16(CPU *cpu, instruction_T instrc) {
    
}
static void LD_p_a16_SP(CPU *cpu, instruction_T instrc) {

}
// LDH a8 instructions:
static void LDH_A_p_a8(CPU *cpu, instruction_T instrc) {
    printf("LDH A [a8] Called => 0xFF00 + a8 then.. copy into A register\n");
    // This one is wonky. Takes an [a8] value, converts it to 16bit and is Zero Extended
    // 0x21 => 0xFF21, This area is often HRAM, I/O. IE Register
    // [a8] brackets mean: loading value of a8, so 0xFF00 + a8

    uint8_t a8 = instrc.operand1;
    uint16_t combined_addr = 0xFF00 + a8;

    printf(":LDH: Combined val %04X\n", combined_addr);
    uint8_t load_h_range = external_read(combined_addr);

    cpu->A = load_h_range;

    cpu->PC +=2; // 
}
static void LDH_p_a8_A(CPU *cpu, instruction_T instrc) {
    printf("LDH [a8] A, Called => Copy A Register into (0xFF00 + a8) \n");
    
    uint8_t a8 = instrc.operand1;
    uint16_t combined_addr = 0xFF00 + a8;

    printf(":LDH: Combined val %04X\n", combined_addr);
    external_write(combined_addr, cpu->A);

    cpu->PC +=2;    
}

// Jump a16 instructions:
static void JP_cc_a16(CPU *cpu, instruction_T instrc) {         // This is first instruction in ROM (I am using).
    // JP NZ a16

    
}
static void JP_a16(CPU *cpu, instruction_T instrc) {
    printf(":CPU_INSTRUCTIONS: OpCode: C3 => Jump to Value\n");
    uint16_t jumpto_byte = cnvrt_lil_endian(instrc.operand1, instrc.operand2);
    cpu->PC = jumpto_byte;
}

// Calls a16:
static void CALL_cc_a16(CPU *cpu, instruction_T instrc) {

}
static void CALL_a16(CPU *cpu, instruction_T instrc) {

}





// TEST: This uses macros to make a function dynmatically.

// This is LD Register -> Register
#define LD_X_Y(X, Y) \
static void LD_##X##_##Y(CPU *cpu, instruction_T instrc) \
{ \
    printf(":CPU_INSTRUCTIONS: LD r8 to r8\n");\
    cpu->X = cpu-> Y;\
    cpu->PC += 1;\
    printf("x, y");\
}
//gb->x = gb->y;



#define LD_X_DHL(X) \
static void LD_##X##_##DHL(CPU *cpu, instruction_T instrc) \
{ \
    printf("b");\
}


#define LD_DHL_Y(Y) \
static void LD_##DHL##_##Y(CPU *cpu, instruction_T instrc) \
{ \
    printf("C");\
}


/* NOP */   LD_X_Y(B,C) LD_X_Y(B,D) LD_X_Y(B,E) LD_X_Y(B,H) LD_X_Y(B,L) LD_X_DHL(B) LD_X_Y(B,A) LD_X_Y(C,B) /* NOP */   LD_X_Y(C,D) LD_X_Y(C,E) LD_X_Y(C,H) LD_X_Y(C,L) LD_X_DHL(C) LD_X_Y(C,A)
LD_X_Y(D,B) LD_X_Y(D,C) /* NOP */   LD_X_Y(D,E) LD_X_Y(D,H) LD_X_Y(D,L) LD_X_DHL(D) LD_X_Y(D,A) LD_X_Y(E,B) LD_X_Y(E,C) LD_X_Y(E,D) /* NOP */   LD_X_Y(E,H) LD_X_Y(E,L) LD_X_DHL(E) LD_X_Y(E,A)
LD_X_Y(H,B) LD_X_Y(H,C) LD_X_Y(H,D) LD_X_Y(H,E) /* NOP */   LD_X_Y(H,L) LD_X_DHL(H) LD_X_Y(H,A) LD_X_Y(L,B) LD_X_Y(L,C) LD_X_Y(L,D) LD_X_Y(L,E) LD_X_Y(L,H) /* NOP */   LD_X_DHL(L) LD_X_Y(L,A)
LD_DHL_Y(B) LD_DHL_Y(C) LD_DHL_Y(D) LD_DHL_Y(E) LD_DHL_Y(H) LD_DHL_Y(L) /* NOP */   LD_DHL_Y(A) LD_X_Y(A,B) LD_X_Y(A,C) LD_X_Y(A,D) LD_X_Y(A,E) LD_X_Y(A,H) LD_X_Y(A,L) LD_X_DHL(A) /* NOP */









static opcode_t *opcodes[256] = {
/*  ---> X0, X1, X2, X3, X4 ... XB .. XF etc */
/* 0X */ NOP,        LD_r16_n16,    LD_p_r16_A, INC_r16,  INC_hr8,     DEC_hr8,  LD_hr_n8,   RLCA,     /* || */ LD_p_a16_SP,  ADD_HL_r16, LD_A_p_r16,  DEC_r16,   INC_lr8,     DEC_lr8,   LD_lr_n8,   RRCA,
/* 1X */ STOP,       LD_r16_n16,    LD_p_r16_A, INC_r16,  INC_hr8,     DEC_hr8,  LD_hr_n8,   RLA,      /* || */ JR_e8,        ADD_HL_r16, LD_A_p_r16,  DEC_r16,   INC_lr8,     DEC_lr8,   LD_lr_n8,   RRA,
/* 2X */ JR_cc_e8,   LD_r16_n16,    LD_p_HLI_A, INC_r16,  INC_hr8,     DEC_hr8,  LD_hr_n8,   DAA,      /* || */ JR_cc_e8,     ADD_HL_r16, LD_A_p_HLI,  DEC_r16,   INC_lr8,     DEC_lr8,   LD_lr_n8,   CPL,
/* 3X */ JR_cc_e8,   LD_r16_n16,    LD_p_HLI_A, INC_r16,  INC_p_HL,    DEC_p_HL, LD_p_HL_n8, SCF,      /* || */ JR_cc_e8,     ADD_HL_r16, LD_A_p_HLD,  DEC_r16,   INC_hr8,     DEC_hr8,   LD_hr_n8,   CCF,
/* 4X */ NOP,        LD_B_C,        LD_B_D,     LD_B_E,   LD_B_H,      LD_B_L,   LD_B_DHL,   LD_B_A,   /* || */ LD_C_B,       NOP,        LD_C_D,      LD_C_E,    LD_C_H,      LD_C_L,    LD_C_DHL,   LD_C_A,
/* 5X*/  LD_D_B,     LD_D_C,        NOP,        LD_D_E,   LD_D_H,      LD_D_L,   LD_D_DHL,   LD_D_A,   /* || */ LD_E_B,       LD_E_C,     LD_E_D,      NOP,       LD_E_H,      LD_E_L,    LD_E_DHL,   LD_E_A,
/* 6X */ LD_H_B,     LD_H_C,        LD_H_D,     LD_H_E,   NOP,         LD_H_L,   LD_H_DHL,   LD_H_A,   /* || */ LD_L_B,       LD_L_C,     LD_L_D,      LD_L_E,    LD_L_H,      NOP,       LD_L_DHL,   LD_L_A,
/* 7X */ LD_DHL_B,   LD_DHL_C,      LD_DHL_D,   LD_DHL_E, LD_DHL_H,    LD_DHL_L, HALT,       LD_DHL_A, /* || */ LD_A_B,       LD_A_C,     LD_A_D,      LD_A_E,    LD_A_H,      LD_A_L,    LD_A_DHL,   NOP,
/* 8X */ ADD_A_r8,   ADD_A_r8,      ADD_A_r8,   ADD_A_r8, ADD_A_r8,    ADD_A_r8, ADD_A_r8,   ADD_A_r8, /* || */ ADC_A_r8,     ADC_A_r8,   ADC_A_r8,    ADC_A_r8,  ADC_A_r8,    ADC_A_r8,  ADC_A_r8,   ADC_A_r8,  
/* 9X */ SUB_A_r8,   SUB_A_r8,      SUB_A_r8,   SUB_A_r8, SUB_A_r8,    SUB_A_r8, SUB_A_r8,   SUB_A_r8, /* || */ SBC_A_r8,     SBC_A_r8,   SBC_A_r8,    SBC_A_r8,  SBC_A_r8,    SBC_A_r8,  SBC_A_r8,   SBC_A_r8,
/* AX */ AND_A_r8,   AND_A_r8,      AND_A_r8,   AND_A_r8, AND_A_r8,    AND_A_r8, AND_A_r8,   AND_A_r8, /* || */ XOR_A_r8,     XOR_A_r8,   XOR_A_r8,    XOR_A_r8,  XOR_A_r8,    XOR_A_r8,  XOR_A_r8,   XOR_A_r8,
/* BX */ OR_A_r8,    OR_A_r8,       OR_A_r8,    OR_A_r8,  OR_A_r8,     OR_A_r8,  OR_A_r8,    OR_A_r8,  /* || */ CP_A_r8,      CP_A_r8,    CP_A_r8,     CP_A_r8,   CP_A_r8,     CP_A_r8,   CP_A_r8,    CP_A_r8,
/* CX */ RET_cc,     POP_r16,       JP_cc_a16,  JP_a16,   CALL_cc_a16, PUSH_r16, ADD_A_r8,   RST_vec,  /* || */ RET_cc,       RET,        JP_cc_a16,   CB_PREFIX, CALL_cc_a16, CALL_a16,  ADC_A_r8,   RST_vec,
/* DX */ RET_cc,     POP_r16,       JP_cc_a16,  BLANK,    CALL_cc_a16, PUSH_r16, SUB_A_r8,   RST_vec,  /* || */ RET_cc,       RETI,       JP_cc_a16,   BLANK,     CALL_cc_a16, BLANK,     SBC_A_r8,   RST_vec,
/* EX */ LDH_p_a8_A, POP_r16,      LDH_p_C_A,  BLANK,    BLANK,        PUSH_r16, AND_A_r8,   RST_vec,  /* || */ ADD_SP_e8,    JP_HL,      LD_p_a16_A,  BLANK,     BLANK,       BLANK,     XOR_A_r8,   RST_vec,
/* FX */ LDH_A_p_a8, POP_r16,      LDH_A_p_C,  DI,       BLANK,        PUSH_AF, OR_A_r8,     RST_vec,  /* || */ LD_HL_SP_Pe8,  LD_SP_HL,  LD_A_p_a16,  EI,        BLANK,       BLANK,     CP_A_n8,    RST_vec,
};


int execute_instruction(CPU *cpu, instruction_T instrc) {
    //printf(":CPU_INSTRUCTIONS: cpu->PC: %02X\n", cpu->PC);
    printf(":CPU_INSTRUCTIONS:\n------\n    Execute OPCODE: %02X, at PC: %04X\n", instrc.opcode, cpu->PC);
    printf("    OP1: %02X, OP2: %02X\n------\n", instrc.operand1, instrc.operand2);

    opcodes[instrc.opcode](cpu, instrc);

    //printf("Finished, Return to :CPU:\n");
    return 0;
}

