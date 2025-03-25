#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..

#include "e_core.h"
#include "cart.h"


extern Cartridge cartridge;             // Global instance of this, so it can be passed around (Declared only once)



// Global Memory:
uint8_t WRAM[WRAM_size];
uint8_t HRAM[HRAM_size];
uint8_t VRAM[VRAM_size];

uint8_t EXT_RAM;                        // Dynamic Memory
uint8_t ROM_header_raw[HEADER_SIZE];    // Storage of ROM Header

uint8_t ROM;


uint8_t memory_map[M_MAP_size];

// Some CPU Registers:
uint16_t PC = 0x100;                    // Maybe delete this...
uint16_t SP = 0xFFFE;                   // Maybe.. delete this..



// Read rom, AFTER header.
void load_rom(const char *filename) {
    FILE *file = fopen(filename, "rb");     // rb = Read bytes of the file.
    if (!file) {
        perror("Error reading ROM file\n");
        return;
    }

    //printf("Entry Points: (0)%02X (1)%02X (2)%02X\n", entry_point[0], entry_point[1], entry_point[2]);
    // BE, 00. Little Endian means. Least significant Byte first.

    uint16_t jump_address;

    // Entry point value said: 0xC3 (JUMP) to BE OO (Reversed cause little endian)
    jump_address = 0x00BE;

    uint8_t next_op[OP_Extra_size] = {0};


    fseek(file, jump_address, SEEK_SET);
    fread(next_op, 1, 10, file);            // Read up to 10 values, and store into next_op[x]
    fclose(file);
}

// maybe delete...
void get_header_multi_bit_val(uint8_t *value, int is_string, uint8_t range_start, uint8_t range_end) {
    
    uint8_t length;
    if (is_string == 1) {
        length = (range_end - range_start) + 1;
    }
    else {
        length = (range_end - range_start);
    }

    uint8_t combined_hdr[length];

    for (int i = range_start; i <= length; i++)
        {
            combined_hdr[i] = ROM_header_raw[i];        // I really don't think that is going to work :/
            printf("%02X ", combined_hdr[i]);
        }
    value = combined_hdr;      // Likely will throw an error cause I suck at pointers...
    
}



void parse_cart_header(const char *filename, Cartridge *cart) {
    FILE *file = fopen(filename, "rb");             // rb = Read bytes of the file.
    if (!file) {
        perror("Error reading ROM file\n");
        return;
    }

    fseek(file, HEADER_OFFSET, SEEK_SET);           // The offset is 0x0100 to 0x0150
    fread(ROM_header_raw, 1, HEADER_SIZE, file);
    fclose(file);

    
    
    // NOTICE: Codes are usually like "0x0134", how I read the header file, means all those codes have 0x100 removed already.
    cart->header_config.cart_type_code = ROM_header_raw[0x47];
    cart->header_config.rom_size_code = ROM_header_raw[0x48];    // Rom
    cart->header_config.ram_size_code = ROM_header_raw[0x49];    // Ram  (Memory size)
    cart->header_config.chksm = ROM_header_raw[0x4D];

    for (int i = 0; i <= 3; i++)
    {
        cart->header_config.entry_point[i] = ROM_header_raw[i];        // I really don't think that is going to work :/
        printf("%02X ", cart->header_config.entry_point[i]);
    }

}



int main() {
    const char *rom_file = "../rom/pkmn_red.gb";    
    printf("NOTE: Using rom file: %s\n\n", rom_file);

    // Need to get th ROMs header first. To know how big the entire Rom file is.
    //get_RomHeader(rom_file);
    parse_cart_header(rom_file, &cartridge);     // Loads the header, reads the data, parses each setting, sets easy to use flags for each header.

    set_cart_features(&cartridge);
    
    // Check Rom Data
    // Raw Codes:
    printf("::ROM_RAW:: Cart_type: 0x%02X ROM Size: 0x%02X RAM Size: 0x%02X\n", cartridge.header_config.cart_type_code, cartridge.header_config.rom_size_code, cartridge.header_config.ram_size_code);

    // Cart settings.
    printf("::ROM:: \n| MBC_type 0x%02X | ROM Size: 0x%02X | RAM Size: 0x%02X | ROM BANKS: %d\n", cartridge.config.mbc_type, cartridge.config.rom_size, cartridge.config.ram_size, cartridge.config.rom_banks);

    // Load the entire Rom into memory. (Deal with banks after, if any)



    return 0;
}