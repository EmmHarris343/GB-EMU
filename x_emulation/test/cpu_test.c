#include "cpu_test.h"
#ifdef ENABLE_TESTS
// This is for The build flag.

#include <stdio.h>
#include "../src/cpu.h"

#endif


#include <stdio.h>
#include "../src/cpu.h"
#include "../src/cpu_instructions.h"


#include <assert.h>

extern CPU cpu_reg_simple_tstate;


//static opcode_t *opcodes[256] = {
static uint32_t *opcodes[256] = {       // This is likely a bad idea. or you know not right at all...
    // Each "test" to do here?

};


void prefix_test() {
    /// NOTICE: This is important.
    // Any prefix CPU Instruction. Needs to be placed in the PC

    // So if I call a prefix instruction.
    // It will advance and read the NEXT PC. And use that to determin the opcode.

    // So I should WRITE to the PC (cpu->PC+1). The OPCODE I want to use
}



void complex_test() {
    /// NOTICE: 
    // The more complex tests. I should "write" to RAM, or change what the ROM returns specifically. 
    // That way I can block out say 10-20 commands. 
    // Jump commands, etc
    // Have them all stored in order in the RAM/ ROM/ PC. 
}





void verify_state() {
    // Check each CPU state, Registers, Flags.

    // If read from RAM (Set RAM state before read)
    // If write to RAM (Need to READ RAM AFTER. To verify was set correctly)
}

bool reg_compare(CPU *working, CPU *expected) {
    return \
    working->reg.AF == expected->reg.AF &&
    working->reg.BC == expected->reg.BC &&
    working->reg.DE == expected->reg.DE &&
    working->reg.HL == expected->reg.HL &&
    working->reg.PC == expected->reg.PC &&
    working->reg.SP == expected->reg.SP;
}


void unt_test_case() {
    
    instruction_T local_instrc;
    local_instrc.opcode = 0x5C;               // Pass instrc direction into cpu_instructions.c
    local_instrc.operand1 = 0x00;
    local_instrc.operand2 = 0x00;

    test_case_t test;
    test.name = "LD E, H";
    test.opcode = 0x5C;
    test.initial_cpu = cpu_reg_simple_tstate;

    test.expected_cpu = test.initial_cpu;
    test.expected_cpu.reg.E = 0x04;
    test.expected_cpu.reg.PC = 0x078A;


    CPU working_cpu = test.initial_cpu;

    if (execute_test(&working_cpu, local_instrc) != 0) { printf("ERROR [cpu_test], instruction failed to execute"); }
    else { printf("Unit test Finished. Exiting...\n"); }

    if (reg_compare(&working_cpu, &test.expected_cpu)) {
        printf("[PASS] Name:[%s] OPCODE: [0x%02X]\n", test.name, test.opcode);
    }
    else {
        printf("[FAILED] Test failed. Register did not match expected values.\n");
        // Don't like this... This is a HARD CODED Test. I need to make this modular.
        assert(test.expected_cpu.reg.E == working_cpu.reg.E);    // This throws a hard Fail, and immediately ends execution.
    }

    
}

void unit_test_instruction() {
    /// ENTRY: This is the entry point for now....


    // Step 1: Create instruction to execute.
    // Step 2: Define Initial / Expected CPU state and Registers.
    // Step 3: Execute instruction (Directly calling cpu_instructions.c)
    // Step 4: Compare. Throw [Pass]/[Fail]

    // Do next instruction.


    // Directly call instructions in cpu_instructions.c
    // By passes decode, extra states, verification, writes to console, etc

    // Do this first. Because all I need to know is:
    // "Does my instruction even work?"

    CPU cpu = cpu_reg_simple_tstate;    // Pass this directly into cpu_instructions.c

    instruction_T local_instrc;
    
    local_instrc.opcode = 0x5C;               // Pass instrc direction into cpu_instructions.c
    local_instrc.operand1 = 0x00;
    local_instrc.operand2 = 0x00;

    // This is in cpu_instruction.c
    if (execute_test(&cpu, local_instrc) != 0) { printf("ERROR [cpu_test], instruction failed to execute"); }
    else { printf("Unit test Finished. Exiting...\n"); }
    
    


}

void integration_test_instruction() {
    // Run integration test through cpu.c
    // Test full cycle of read, decode, execute of OPCODE
    // Test the connection, reading, execution and return of cpu_instruction.c
    
    
    // Another words.... I need to set the PC to have the OPCODE and operands. 
    // So it can decode them. THEN Execute them.


    // IE: Do this a little bit later.
    // Right now, I need to figure out if my instructions work as intended.
    
}

void instruction_test_advanced(){
    printf("[CPU_Test] Hook into CPU.c ---> Start CPU Test!\n");


    // TEST Section:
    run_cpu_test(0x5C); // cpu_test -> cpu.c
}



void instruction_test(){
    printf("[CPU_Test] Hook into CPU.c ---> Start CPU Test!\n");
    


    // TEST Section:
    run_cpu_test(0x5C); // cpu_test -> cpu.c



}



void run_multiple_cpu_test() {
    int total_tests = 40;

    for(int i = 0; i < total_tests; i++){

        /// EXECUTE: CPU Instruction Test.

    }
    
}
