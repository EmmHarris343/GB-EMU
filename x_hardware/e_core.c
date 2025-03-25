#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..

#include <e_core.h>




// Global Memory:
uint8_t WRAM[WRAM_size];
uint8_t HRAM[HRAM_size];
uint8_t VRAM[VRAM_size];

uint8_t EXT_RAM;                    // Dynamic Memory
uint8_t ROM_header_raw[HEADER_SIZE];    // Storage of ROM Header

uint8_t ROM;


uint8_t memory_map[M_MAP_size];

// Some CPU Registers:
uint16_t PC = 0x100;                // Maybe delete this...
uint16_t SP = 0xFFFE;               // Maybe.. delete this..



// Read rom, AFTER header.
void load_rom(filename) {
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
    *value = combined_hdr;      // Likely will throw an error cause I suck at pointers...
    
}

void get_RomHeaders(filename) {
    FILE *file = fopen(filename, "rb");             // rb = Read bytes of the file.
    if (!file) {
        perror("Error reading ROM file\n");
        return;
    }

    fseek(file, HEADER_OFFSET, SEEK_SET);           // The offset is 0x0100 to 0x0150
    fread(ROM_header_raw, 1, HEADER_SIZE, file);
    fclose(file);

    // Using static values, cause I am... lame.

    Header.Hd_cart_type = ROM_header_raw[0x0147];
    Header.Hd_rom_size = ROM_header_raw[0x0148];    // Rom
    Header.Hd_ram_size = ROM_header_raw[0x0149];    // Ram  (Memory size)
    Header.Hd_chksm = ROM_header_raw[0x014D];

    //printf(" - Cartridge Type: 0x%02X\n", cartridge_type);
    //printf(" - ROM Size Code: 0x%02X\n", rom_size);
    //printf(" - RAM Size Code: 0x%02X\n", ram_size);
    // Verify Chksum
    //u_int8_t clc_hdr_chksm = calculate_header_checksum(ROM_header);


    #define ENTRY_POINT_START   0x0100
    #define ENTRY_POINT_END     0x0103
    
    #define TITLE_START         0x0134
    #define TITLE_END           0x0143
    #define TITLE_LENGTH        (TITLE_END - TITLE_START +1)
    
    #define LIC_NEW_CODE_START  0x0144
    #define LIC_NEW_CODE_END    0x0145
    #define LIC_NEW_LENG        (LIC_NEW_CODE_END - LIC_NEW_CODE_START +1)
    #define LIC_OLD             0x014B
    
    #define CART_TYPE_ADDR      0x0147
    #define ROM_SIZE_ADDR       0x0148
    #define RAM_SIZE_ADDR       0x0149
    
    #define CHKSM_START         0x0134
    #define CHKSM_END           0x014C
    #define CHKSM_BYTE          0x014D


}

void get_RomEnPnt() {

}


int main() {
    const char *rom_file = "rom/pkmn_red.gb";
    printf("NOTE: Using rom file: %s\n\n", rom_file);


    // Need to get th ROMs header first. To know how big the entire Rom file is.


    // Load the entire Rom into memory. (Deal with banks after, if any)



    return 0;
}