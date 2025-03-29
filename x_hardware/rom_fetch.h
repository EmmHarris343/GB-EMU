#ifndef ROM_FETCH_H
#define ROM_FETCH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>





// ROM / Rom Banks
#define FIXED_BANK_SIZE     0x4000          // The fixed bank at 0x0000-0x3FFF
#define ROM_BANK_SIZE       0x4000          // 16 KB banks
#define MAX_ROM_SIZE        0x800000        // 8MB Max Rom Size. (Note 800000 => 8,388,608 bytes. /1024 = 8,192 KB)


// Prototypes: 
void load_entire_rom(const char *filename, size_t exp_rom);


#endif