#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..

#include "e_core.h"
#include "e_ctrl.h"



void begin_emulation() {

}

void stop_emulation() {
    
}



int main() {

    printf(":E_CORE: Starting up Emulator\n");
    startup_sequence();     // e_ctrl -> Configs, Setup MBC Config, Load ROM, Setup RAM, Initialize CPU,


    // Actually begin "emulation" (Emulation in quotes cause still testing).
    // Start Main CPU Loop.

    
    

    




    return 0;
}