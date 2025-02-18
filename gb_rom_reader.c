#define _GNU_SOURCE     // This is needed to get the functions in the libraries to work :/ stupid I know..
#include <stdio.h>
#include <stdlib.h>


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

#define CARTRIDGE_TYPE_ADDR   0x0147
#define ROM_SIZE_ADDR           0x0148


#define QA_ASCII_START 32
#define QA_ASCII_END 126


#define CHKSM_START         0x0134
#define CHKSM_END           0x014C
#define CHKSM_BYTE          0x014D


// Checksum.
unsigned char calculate_header_checksum(unsigned char *rom_header) {
    u_int8_t chksum = 0;        // This is required to force this into 8 bit (1 byte). Unsigned.int
    printf("... What's the initial Value. %02x\n", chksum);
    for (int address = CHKSM_START; address <= CHKSM_END; address++) {
        printf("Address line: 0x%02x :: ", address);
        printf(":: HRD_DATA %02x :: ", rom_header[address - HEADER_OFFSET]);
        chksum = chksum - rom_header[address - HEADER_OFFSET] - 1;
        printf("%d :: ", chksum);
        printf("chk %02x \n :: ", chksum);
    }
    printf("..\n");
    printf("Final Chksum, before return. Dec: %d | Byte: %02x\n", chksum, chksum);
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


    // Get Entry point:
    printf("Entry point:\n");
    for (int i = ENTRY_POINT_START - HEADER_START; 
        i <= ENTRY_POINT_START - HEADER_START; 
        i++){
        printf("%02X ", header[i]);
    }
    printf("\n");

    // Get Title:

    char title[TITLE_LENGTH + 1]; // +1 for null terminator
    for (int i = 0; i < TITLE_LENGTH; i++) {
        title[i] = (
            header[TITLE_START - HEADER_START + i] >= QA_ASCII_START &&
            header[TITLE_START - HEADER_START + i] <= QA_ASCII_END
            )
            ? header[TITLE_START - HEADER_START + i]
            : '.';  // Add the little Dots, between the Character.
    }
    title[TITLE_LENGTH] = '\0';     // Set the Null Terminator (For the string stuff);
    printf("Rom Title: %s\n", title);

    // Get the Code:

    
    unsigned char LIC[LIC_LENG +1];
    unsigned char LIC_2;
    LIC_2 = (header[LIC_CODE_START - HEADER_START]);
    printf("LIC CODE (2): %02X | (DEC): %d\n", LIC_2, LIC_2);


    unsigned char cartridge_type = header[CARTRIDGE_TYPE_ADDR- HEADER_START];
    printf("Cartridge Type: 0x%02X\n", cartridge_type);


    unsigned char rom_size = header[ROM_SIZE_ADDR - HEADER_START];
    printf("ROM Size Code: 0x%02X\n", rom_size);


    calculate_header_checksum(header);

    unsigned char chk_byte_val = header[CHKSM_BYTE - HEADER_OFFSET];
    printf("And what's in this byte.. (Without the 0x) %02X\n", chk_byte_val);

    // for (int i = 0; i < LIC_LENG; i++ ){
    //     LIC[i] = (header[LIC_CODE_START - HEADER_START + i]);
    //     printf("VAL: %02X \n", LIC[i]);
    // }
    
    // printf("LIC Code? %s\n", LIC);

}






int main() {
    //char *rom_file = "zelda_lnk_a(fr).gbc";
    char *rom_file = "pkmn_red.gb";
    printf("Using rom file: %s\n", rom_file);

    // NOTE, when accessing ROM Data from the header. It is FINALLYYYY something that is:
    // header[OFFSET - HEADER_START]        A fucking offset.... 


    read_rom_info(rom_file);
    return 0;
}