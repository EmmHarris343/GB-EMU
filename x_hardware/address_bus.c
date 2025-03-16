#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Using 1, 16-bit Address space. It can point to 65536 memory locations
0000	3FFF	16 KiB ROM bank 00	From cartridge, usually a fixed bank
4000	7FFF	16 KiB ROM Bank 01–NN	From cartridge, switchable bank via mapper (if any)
8000	9FFF	8 KiB Video RAM (VRAM)	
A000	BFFF	8 KiB External RAM	From cartridge, switchable bank if any
C000	CFFF	4 KiB Work RAM (WRAM)	
D000	DFFF	4 KiB Work RAM (WRAM)	(DMG only 1 additional bank)
E000	FDFF	Echo RAM DNU  - Prohibited - (mirror of C000–DDFF)
FE00	FE9F	Object attribute memory (OAM)	
FEA0	FEFF	Not Usable DNU - Prohibited -
FF00	FF7F	I/O Registers	
FF80	FFFE	High RAM (HRAM)	
FFFF	FFFF	Interrupt Enable register (IE)
*/



// 2000 - 3FFF - Ram Bank Number (Write only)..

// Ok........
// So, to detect if something is requeseting a different Rom BANK. 
// I need to listen for "A Write" to anywhere in between 2000 - 3FFF.
// Record what it is.. and what it's pointing to, to decode what rom bank it wants to be changed to
// BUT, not to actually edit the data in the memory.. 
// Because where it writes to is actually ROM IE: Read only. So do NOT overwrite the rom data like that



void bus_entry(uint16_t address) {
    printf("Yo, here is something");

    switch (address){
        case 0x0000 ... 0x3FFF:
            printf("ROM Bank 00 - Fixed Bank");
        case 0x4000 ... 0x7FFF:
            printf("ROM Bank 01-NN - Switchable Bank");
        case 0x8000 ... 0x9FFF:
            printf("8 KiB VRAM");
        case 0xA000 ... 0xBFFF:
            printf("8 KiB E-RAM (External)");
        case 0xC000 ... 0xCFFF:
            printf("4 KiB WRAM (Work Ram)");
        case 0xD000 ... 0xDFFF:
            printf("4 KiB WRAM - DMG Extra Bank");
        case 0xE000 ... 0xFDFF:
            printf("ECHO RAM - PROHIBITED");
        case 0xFE00 ... 0xFE9F:
            printf("OAM (Object attribute memory)");
        case 0xFEA0 ... 0xFEFF:
            print("NOT USABLE - PROHIBITED");
        case 0xFF00 ... 0xFF7F:
            print("I/O Registers");             // Display, sound and such?
        case 0xFF80 ... 0xFFFE:
            printf("HRAM - High Ram");          // I assume it's like a cpu cache
        case 0xFFFF:
            print("Interupt Space");
        default: 
            printf("Out of range... Abort?");
    }
}



int main() {

    return 0;
}