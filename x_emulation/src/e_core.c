#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..

#include "e_core.h"
#include "e_ctrl.h"

int emulation_mode = 2; // 0 = normal ; 3 = test.


void begin_emulation() {

}

void stop_emulation() {
    
}



int main() {

    // Normal CPU Run.
    if (emulation_mode == 0) {
        printf(":E_CORE: Starting up Emulator\n");
        if (startup_sequence() !=0)         // e_ctrl -> Configs, Setup MBC Config, Load ROM, Setup RAM, Initialize CPU,
        {
            fprintf(stderr, "Error during startup Sequence. ABORT:\n");
            //return -2;
    
        }
    }
    // Normal CPU Run, BY TIME!
    if (emulation_mode == 2) {
        printf(":E_CORE: Starting up Emulator\n");
        if (startup_seq_bytime() !=0)         // e_ctrl -> Configs, Setup MBC Config, Load ROM, Setup RAM, Initialize CPU,
        {
            fprintf(stderr, "Error during startup Sequence. ABORT:\n");
            //return -2;
    
        }
    }    


    // -- TEST MODE -- !
    if (emulation_mode == 3) {
        printf(":E_CORE: -- TEST MODE -- Startup Test Mode...\n");
        if (test_sequence() !=0)         // e_ctrl -> Configs, Setup MBC Config, Load ROM, Setup RAM, Initialize CPU,
        {
            fprintf(stderr, "Error during startup Sequence. ABORT:\n");
            //return -2;
    
        }
    }

    //startup_sequence();


    // Actually begin "emulation" (Emulation in quotes cause still testing).
    // Start Main CPU Loop.


 
    return 0;
}