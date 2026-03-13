

// static const OpcodeInfo opcode_info[256] = {
//     [0x00] = { "NOP", "Does nothing, just advance PC",1, 4, 0},
//     [0x01] = { "LD BC,d16", "",3,12,0 },
//     [0x02] = { "LD (BC),A", "",1,8, 0 },
//     [0x03] = { "INC BC","",1,8, 0 },
//     [0x04] = { "INC B","",1,4, 0 },
//     [0x05] = { "DEC B","",1,4, 0 },
//     [0x06] = { "LD B,d8","",2,8, 0 },

//     [0x00] = {"CCF", "(Clear)Invert Carry Flag 0=1, 1=0", 1, 4, 0},
//     [0x00] = {"SCF", "Set Carry Flag (True)", 1, 4, 0},

//     // Some other commands like Blank, DAA, STOP

//     [0x07] = { "RLCA","Rotate Register A Left, wont use carry flag input.",1, 4, 0 },
//     [0x00] = { "RLA", "Rotate Register A Left (through carry flag).", 1, 4, 0},
//     [0x00] = { "RRCA", "Rotate Register A Right Without the carry flag input.", 1, 4, 0},
//     [0x00] = { "RRA", "Rotate Register A RIGHT --> through Flag C.", 1, 4, 0},
//     [0x00] = {"CPL", "ComPLement accumulator.", 1, 4, 0},
//     [0x00] = {"XOR A, r8", "Bitwise XOR 1^1=0, 1^0=1, 0^0=0. One or Other is 1. NOT BOTH!", 1, 4, 0},
//     [0x00] = {"XOR A, n8", "Bitwise XOR 1^1=0, 1^0=1, 0^0=0. One or Other is 1. NOT BOTH!", 1, 8, 0},
//     [0x00] = {"XOR A, [HL]", "Bitwise XOR 1^1=0, 1^0=1, 0^0=0. One or Other is 1. NOT BOTH!", 1, 8, 0},
//     [0x00] = {"OR A, r8", "Bitwise OR 1|1 = 1. Only 0 if both bits are Zero, 0&0=0", 1, 4, 0},
//     [0x00] = {"OR A, n8", "Bitwise OR 1|1 = 1. Only 0 if both bits are Zero, 0&0=0", 1, 8, 0},
//     [0x00] = {"OR A, [HL]", "Bitwise OR 1|1 = 1. Only 0 if both bits are Zero, 0&0=0", 1, 8, 0},
//     [0x00] = {"AND A, r8", "Bitwise AND. 1&1 = 1, rest =0. From register value (r8)", 1, 4, 0},
//     [0x00] = {"AND A, n8", "Bitwise AND. 1&1 = 1, rest =0. From immediate value (n8)", 1, 8, 0},
//     [0x00] = {"AND A, [HL]", "Bitwise AND. 1&1 = 1, rest =0. From Value in [HL]", 1, 8, 0},

//     [0x00] = {"RET", "RETurn from subroutine..' ... ' Populate the PC stored in the SP.", 1, 20, 0},
//     [0x00] = {"RET-CC", "Populate PC from SP, if CC Condition met.", 1, 8, 20},
//     [0x00] = {"RETI", "Immediately Set IME Flag (Interupt), Then RETurn / Pop PC from SP.", 1, 16, 0},
//     [0x00] = {"RST", "RST_VEC - Push PC to Stack Pointer (SP), then Jump to specific Vector location.", 1, 16, 0},

//     [0x00] = {"PUSH r16", "Push 16 Byte Regiser (BC, DE, HL) to the Stack Pointer SP.", 1, 16, 0},
//     [0x00] = {"PUSH AF", "Push 16 Byte AF Regiser into the Stack Pointer SP.", 1, 16, 0},
//     [0x00] = {"CALL-CC", "IF* condition is met 'Push PC into SP, so RET can POP later', then jump to n16 Address.", 3, 12, 24},
//     [0x00] = {"CALL a16", "'Push PC into SP, so RET can POP later', then jump to n16 Address.", 3, 24, 0},

//     [0x00] = {"POP AF", "Populate AF Register from SP.", 1, 12, 0},
//     [0x00] = {"POP r16", "Populate 16byte Register from SP.", 1, 12, 0},

//     [0x00] = {"INC r16", "Increments r16, IE: BC++;", 1, 8, 0},
//     [0x00] = {"DEC r16", "Decrements r16, IE: BC--;", 1, 8, 0},

//     [0x00] = {"ADD HL, r16.", "Adds value in r16 to HL.", 1, 8, 0},
//     [0x00] = {"ADD SP, e8", "Adds value in e8 to SP (stack pointer).", 2, 16, 0},

//     // CP and others:

//     [0x00] = {"CP A, r8", "ComPare - ComPare (cpu->reg.A - r8_reg) --> Set Flags.", 1, 4, 0},
//     [0x00] = {"CP A, n8", "ComPare - Subtracts value in n8 from A (Sets flags, disregards results).", 2, 8, 0},
//     [0x00] = {"CP A, [HL]", "ComPare - Subtracts pointered from [HL] from A (Sets flags, disregards results).", 1, 8, 0},


//     [0x00] = {"CP [HL]", "ComPare - Subtracts pointered from [HL] from A (Sets flags, disregards results).", 1, 12, 0},
//     [0x00] = {"INC [HL]", "Increment the value pointed by [HL].", 1, 12, 0},
//     [0x00] = {"DEC [HL]", "Decrement the value pointed by [HL].", 1, 12, 0},

//     [0x00] = {"INC r8", "Increment r8 register value.", 1, 4, 0},
//     [0x00] = {"DEC r8", "Decrement r8 register value.", 1, 4, 0},

//     [0x00] = {"SBC A, r8", "Subtract value in r8 (and the carry flag) from Register A.", 1, 4, 0},
//     [0x00] = {"SBC A, n8", "Subtract value in n8 (and the carry flag) from Register A.", 2, 8, 0},
//     [0x00] = {"SBC A, [HL]", "Subtract value in [HL] (and the carry flag) from Register A.", 1, 8, 0},

//     [0x00] = {"SUB A, r8", "Sub Value in Register A, by r8 value.", 1, 4, 0},
//     [0x00] = {"SUB A, n8", "Subtract Register A by value in n8.", 2, 8, 0},
//     [0x00] = {"SUB A, [HL]", "Sub Value in Register A, by value in [HL]", 1, 8, 0},

//     [0x00] = {"ADD A, r8", "Add value in r8 into Register A.", 1, 4, 0},
//     [0x00] = {"ADD A, n8", "Add immediate value to Register A.", 2, 8, 0},
//     [0x00] = {"ADD A, [HL]", "Add Value inside [HL] into Register A", 1, 8, 0},

//     [0x00] = {"ADC A, r8", "Add the value in r8 PLUS the carry flag to Register A.", 1, 4, 0},
//     [0x00] = {"ADC A, n8", "Add the value in n8 PLUS the carry flag to Register A.", 2, 8, 0},
//     [0x00] = {"ADC A, [HL]", "Add the value stored in [hl] PLUS the carry flag to Register A.", 1, 8, 0},


//     [0x00] = {"JR e8", "Relative Jump +- from Operand val.", 2, 12, 0},
//     [0x00] = {"JR CC, e8", "Relative Jump, Conditional.", 2, 8, 12},


//     [0x00] = {"JP HL", "Jump to Register HL location.", 1, 4, 0},
//     [0x00] = {"JP a16", "Jump to a16 location.", 3, 16, 0},
//     [0x00] = {"JP cc a16", "Jump to a16 location. Conditional.", 3, 12, 16},


//     // Maccroed:
//     [0x00] = {"LD r8, r8", "Load value in r8 to r8.", 1, 4, 0},
//     [0x00] = {"LD r8, n8", "Load value in n8 into r8", 2, 8, 0},
//     [0x00] = {"LD r8, [HL]", "Load value pointed by [HL] into r8.", 1, 8, 0},
//     [0x00] = {"LD [HL], r8", "Load value in r8 into value pointed by [HL].", 1, 8, 0},

//     [0x00] = {"LD A, [r16]", "Load value pointed to by [r16] into Register A", 1, 8, 0},
//     [0x00] = {"LD [r16] A", "Load value in Register A, into value pointed by [r16]", 1, 8, 0},

//     [0x00] = {"LD [HL] n8", "LD n8 Value into [HL]", 2, 12, 0},
//     [0x00] = {"LD r16 n16", "Load n16 value into r16 Register", 3, 12, 0},

//     [0x00] = {"LD SP, HL", "Load register value in HL into register SP", 1, 8, 0},
//     [0x00] = {"LD HL, (SP + e8)", "Load the address (SP + e8) into the HL Register.", 2, 12, 0},
//     [0x00] = {"LD [a16], SP", "Write SP_Low & SP_High Bytes, into the [a16] and [a16+1] memory locations.", 3, 20, 0},

//     [0x00] = {"LDH [a8], A", "LD Register A into value pointed by (High Range)[0xFF + a8].", 2, 12, 0},
//     [0x00] = {"LDH [a8], A", "LD value pointed by (High Range)[0xFF + a8] into A Register.", 2, 12, 0},

//     [0x00] = {"LDH [C], A", "LD Register A into value pointed by (High Range)[0xFF + C].", 1, 8, 0},
//     [0x00] = {"LDH A, [C]", "LD value pointed by (High Range)[0xFF + C] into A Register.", 1, 8, 0},

//     [0x00] = {"LD A, [a16]", "LD 8bit value in [a16] into Register A.", 3, 16, 0},
//     [0x00] = {"LD [a16], A", "LD 8bit A Register into 8bit value located in [a16].", 3, 16, 0},

//     [0x00] = {"LD A, [HLD]", "ALT_Name: LD A, [HL-]. Copy value pointed from HL, into A register, then Decrement HL.", 1, 8, 0},
//     [0x00] = {"LD A, [HLI]", "ALT_Name: LD A, [HL+]. Copy value pointed from HL, into A register, then Increment HL", 1, 8, 0},

//     [0x00] = {"LD [HLD], A", "ALT_Name: LD [HL-], A. Copy value in A register into value pointed from HL, then Decrement HL.", 1, 8, 0},
//     [0x00] = {"LD [HLI], A", "ALT_Name: LD [HL+], A. Copy value in A register into value pointed from HL, then Increment HL", 1, 8, 0},
// };



/*
printf("CB PREFIX Called.               ; Load next Byte, to call Prefixed OPCODE\n");

*/
