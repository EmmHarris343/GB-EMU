#include "rom_fetch.h"
#include "mmu.h"        // Do I really need mmu? (mmu WILL need rom_fetch.h)

#include "cart.h"       // This I need, because I need to access headers to know ROM sizes, Total Banks, Individual Bank Size. etc


extern Cartridge cartridge; 

void chop_rom() {
    // Split up the ROM file into it's individual BANKs (If needed)

    uint8_t rom_banks = cartridge.config.rom_bank_count;

    if (rom_banks > 0) {
        // Assign 

    }
    // Splitting up the ROM, Needs to know how many ROM banks there are.


}



void load_entire_rom(const char *filename) {
    // Load the entire ROM file
    FILE *rom_file = fopen(filename, "rb");             // rb = Read bytes of the file.
    if (!rom_file) {
        perror("Error reading ROM file\n");
        return;
    }

    // Check ROM Size first..
    fseek(rom_file, 0, SEEK_END);
    long rom_file_len = ftell(rom_file);

    size_t expected_rom_size = cartridge.config.rom_size;
    if (expected_rom_size == 0 || rom_file_len < expected_rom_size) {
        fprintf(stderr, "ERROR -> ROM Size does not match. Raw: %02x Calc Size: %02x Expected Size: %zu", cartridge.header_config.rom_size_code, cartridge.config.rom_size, expected_rom_size);
        fclose(rom_file);
    }

    // Allocate Memory for the ROM file
    rom_data = malloc(expected_rom_size);
    if (!rom_data) {
        perror("ERROR -> Failed to allocate Memory for entire ROM file");
        fclose(rom_file);        
    }

    // Go back to beginning of ROM File, and Load entire file into RAM.
    rewind(rom_file);
    fread(rom_data, 1, expected_rom_size, rom_file);
    rom_size = 0;
}




void rom_unpack_init() {
    printf("Initialize ROM Loading..\n");

}