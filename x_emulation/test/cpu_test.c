#include "cpu_test.h"
//#include <system_error>
#ifdef ENABLE_TESTS
// This is for The build flag.

#include <stdio.h>
#include "../src/cpu.h"

#endif


#include <stdio.h>
#include "../src/cpu.h"
#include "../src/cpu_instructions.h"

#include "../src/logger.h"


extern CPU cpu_reg_simple_tstate;


#define tFLAG_Z 0x80  // 1000 0000
#define tFLAG_N 0x40  // 0100 0000
#define tFLAG_H 0x20  // 0010 0000
#define tFLAG_C 0x10  // 0001 0000


#define FZ 0x80  // 1000 0000
#define FN 0x40  // 0100 0000
#define FH 0x20  // 0010 0000
#define FC 0x10  // 0001 0000


const char* reg_names[8] = { "B", "C", "D", "E", "H", "L", "[HL]", "A" };


// Condensed flag instructions:
void s_f(CPU* c, uint8_t fl) {
    c->reg.F |= fl;
}
void c_f(CPU* c, uint8_t fl) {
    c->reg.F ^= fl;
}


// 
void create_test() {
    


}







void entry_test_case(){
    printf("Start Test Case \n");
    
    //logging_log("[PASS] Name:[%s] OPCODE: [0x%02X]\n", ld_test.name, ld_test.opcode);   // Save to log file

}