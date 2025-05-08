#ifdef ENABLE_DEBUGGER

printf("Debugger enabled");

#endif


#include <stdio.h>
#include "../src/cpu.h"

void launch_debugger(CPU *cpu) {
    printf("Debugger STARTED\n");
    // Do I even what this junk? Donno.

    
    // printf("PC: %04X  A: %02X  F: %02X\n", cpu->pc, cpu->a, cpu->f);
    // printf("B: %02X  C: %02X  D: %02X  E: %02X\n", cpu->b, cpu->c, cpu->d, cpu->e);
    // printf("H: %02X  L: %02X  SP: %04X\n", cpu->h, cpu->l, cpu->sp);    
}


