#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


// See documentation for these values at: https://gbdev.io/pandocs/The_Cartridge_Header.html

#define HEADER_START        0x0100
#define HEADER_OFFSET       0x0100      // Same as Start, just easier to understand
#define HEADER_SIZE         0x50        // Header END point is: 0x014f (50 Bytes Total)




#define ENTRY_POINT_START   0x0100
#define ENTRY_POINT_END     0x0103

#define TITLE_START         0x0134
#define TITLE_END           0x0143
#define TITLE_LENGTH        (TITLE_END - TITLE_START +1)

#define LIC_CODE_START      0x0144
#define LIC_CODE_END        0x0145
#define LIC_LENG            (LIC_CODE_END - LIC_CODE_START +1)

#define CARTRIDGE_TYPE_ADDR     0x0147
#define ROM_SIZE_ADDR           0x0148
#define RAM_SIZE_ADDR           0x0149


#define QA_ASCII_START 32
#define QA_ASCII_END 126


#define CHKSM_START         0x0134
#define CHKSM_END           0x014C
#define CHKSM_BYTE          0x014D




// Checksum.
unsigned char calculate_header_checksum(unsigned char *rom_header) {
    u_int8_t chksum = 0;        // This is required to force this into 8 bit (1 byte). Unsigned.int
    for (int address = CHKSM_START; address <= CHKSM_END; address++) {
        chksum = chksum - rom_header[address - HEADER_OFFSET] - 1;
    }
    return chksum;
}


void read_rom_info(const char *filename) {
    FILE *file = fopen(filename, "rb");     // rb = Read bytes of the file.

    if (!file) {
        perror("Error reading ROM file\n");
        return;
    }



    // Read entire Header:

    unsigned char header[HEADER_SIZE];
    fseek(file, HEADER_START, SEEK_SET);
    fread(header, 1, HEADER_SIZE, file);
    fclose(file);

    printf("== Header data ; Information as follows.. ==\n");

    // Get Title:

    char title[TITLE_LENGTH + 1]; // +1 for null terminator
    for (int i = 0; i < TITLE_LENGTH; i++) {
        title[i] = (
            header[TITLE_START - HEADER_START + i] >= QA_ASCII_START &&
            header[TITLE_START - HEADER_START + i] <= QA_ASCII_END
            )
            ? header[TITLE_START - HEADER_START + i]
            : '.';  // Add dots for Unused Characters
    }
    title[TITLE_LENGTH] = '\0';     // Set the Null Terminator (For the string stuff);
    printf(" - ROM Title: %s\n", title);


    // Get Entry point:
    printf(" - Entry point: ");
    for (int i = ENTRY_POINT_START - HEADER_START; 
             i <= ENTRY_POINT_START - HEADER_START; 
             i++)
    {
        printf("%02X ", header[i]);
    }
    printf("\n");

    // Get the Code:    
    uint8_t LIC[LIC_LENG +1];
    uint8_t LIC_2;
    LIC_2 = (header[LIC_CODE_START - HEADER_START]);
    printf(" - Licensee CODE: 0x%02X\n", LIC_2);


    uint8_t cartridge_type = header[CARTRIDGE_TYPE_ADDR- HEADER_START];
    printf(" - Cartridge Type: 0x%02X\n", cartridge_type);

    uint8_t rom_size = header[ROM_SIZE_ADDR - HEADER_START];
    printf(" - ROM Size Code: 0x%02X\n", rom_size);

    uint8_t ram_size = header[RAM_SIZE_ADDR - HEADER_START];
    printf(" - RAM Size Code: 0x%02X\n", ram_size);
    // Verify Chksum
    u_int8_t clc_hdr_chksm = calculate_header_checksum(header);
    u_int8_t chk_byte_val = header[CHKSM_BYTE - HEADER_OFFSET];
    // printf("And what's in this byte.. (Without the 0x) %02X\n", chk_byte_val);

    printf("Start Verifying Chksum....\n");

    if (clc_hdr_chksm == chk_byte_val) {
        printf("  - Success : Calculated Chksum Matches 8 Bit: 0x%02X\n", chk_byte_val);
    }
    else {
        printf("  - Failure.. : Calculated Chksum failed doesn't match");
    }


    printf("\n:::LEGEND:::\n");
    printf(" --Cartridge Type $13 = MBC3+RAM+BATTERY\n");
    printf(" --ROM Size $05 = 1MB|64 Rom Banks\n");
    printf(" --RAM Size $03 = 32 KiB|4 banks of 8 KiB each\n");

}






int main() {
    printf("Startup Begun..\n");
    //char *rom_file = "zelda_lnk_a(fr).gbc";
    char *rom_file = "rom/pkmn_red.gb";
    printf("NOTE: Using rom file: %s\n\n", rom_file);

    // NOTE, when accessing ROM Data from the header. It is FINALLYYYY something that is:
    // header[OFFSET - HEADER_START]        A fucking offset.... 


    read_rom_info(rom_file);
    return 0;
}