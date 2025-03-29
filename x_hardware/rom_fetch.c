#include "rom_fetch.h"


uint8_t *rom_data = NULL;
size_t rom_size = 0;


void chop_rom(uint8_t rom_banks) {
    // Split up the ROM file into it's individual BANKs (If needed)



    if (rom_banks > 0) {
        // Assign 

    }
    // Splitting up the ROM, Needs to know how many ROM banks there are.


}



void load_entire_rom(const char *filename, size_t exp_rom) {

    printf(":ROM_FETCH: Loading entire ROM into Memory\n");
    // Load the entire ROM file
    FILE *rom_file = fopen(filename, "rb");             // rb = Read bytes of the file.
    if (!rom_file) {
        perror("Error reading ROM file\n");
        return;
    }

    // Check ROM Size first..
    fseek(rom_file, 0, SEEK_END);
    long rom_file_len = ftell(rom_file);

    // 
    if (exp_rom == 0 || rom_file_len < exp_rom) {
        fprintf(stderr, "ERROR -> ROM Size does not match. Expected Size: %zu", exp_rom);
        fclose(rom_file);
    }

    // Allocate Memory for the ROM file
    rom_data = malloc(exp_rom);
    if (!rom_data) {
        perror("ERROR -> Failed to allocate Memory for entire ROM file");
        fclose(rom_file);
    }

    // Go back to beginning of ROM File, and Load entire file into RAM.
    rewind(rom_file);
    fread(rom_data, 1, exp_rom, rom_file);
    rom_size = 0;
}




void rom_unpack_init() {
    printf("Initialize ROM Loading..\n");

}