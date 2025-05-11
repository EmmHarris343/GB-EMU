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





void verify_state() {
    // Check each CPU state, Registers, Flags.

    // If read from RAM (Set RAM state before read)
    // If write to RAM (Need to READ RAM AFTER. To verify was set correctly)
}


void run_cpu_test(){


}



void run_chaque_cpu_test() {

    int total_tests = 40;

    for(int i = 0; i < total_tests; i++){

        /// EXECUTE: CPU Instruction Test.

    }
    
}






void ld_r8_test() {
    
}