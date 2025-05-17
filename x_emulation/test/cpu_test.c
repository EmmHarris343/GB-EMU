#ifdef ENABLE_TESTS
// This is for The build flag.

#include <stdio.h>
#include "../src/cpu.h"

#endif


#include <stdio.h>
#include "../src/cpu.h"





//static opcode_t *opcodes[256] = {
static uint32_t *opcodes[256] = {       // This is likely a bad idea. or you know not right at all...
    // Each "test" to do here?

};

void set_register_state() {

}




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
