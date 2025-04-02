#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..

#include "e_core.h"
#include "e_ctrl.h"



void begin_emulation() {

}

void stop_emulation() {
    
}



int main() {

    printf(":E_CORE: Starting up Emulator\n");
    if (startup_sequence() !=0)         // e_ctrl -> Configs, Setup MBC Config, Load ROM, Setup RAM, Initialize CPU,
    {
        fprintf(stderr, "Error during startup Sequence. ABORT:\n");
        //return -2;

    }
    //startup_sequence();


    // Actually begin "emulation" (Emulation in quotes cause still testing).
    // Start Main CPU Loop.


    

    




    return 0;
}