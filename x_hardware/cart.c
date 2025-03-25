#include <stdio.h>

#include "cart.h"
#include "mbc.h"


/*
    This is cartridge configuration. 

    This is responsible for:
        Parsing the header file,
        Pointing each write, read function so other functions doesn't have to care.
        Ram Configuration, reserving memory. etc

*/ 

// Rom Header:
#define HEADER_OFFSET       0x0100          // Same as Start, just easier to understand
#define HEADER_SIZE         0x50            // Header END point is: 0x014f (50 Bytes Total - 80 Decimal)

Cartridge cartridge;        // This must be defined once. HERE makes the most sense (If not, the linker will throw an error and won't compile)

uint8_t ROM_header_raw[HEADER_SIZE];    // Storage of ROM Header

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



void set_cart_features(Cartridge *cart) {

    
        // Set Cart type and features.
        switch (cart->header_config.cart_type_code) {
            case 0x00:
                cart->config.mbc_type = 0;
                break;
            case 0x01 ... 0x03:
                cart->config.mbc_type = 1;
                cart->config.has_ram = (cart->header_config.cart_type_code == 0x02 || cart->header_config.cart_type_code == 0x03);
                cart->config.has_battery = (cart->header_config.cart_type_code == 0x03);
                break;
            case 0x13:      // MBC3 + RAM + Battery
                cart->config.mbc_type = 3;
                cart->config.has_ram = 1;
                cart->config.has_battery = 1;
                break;
            default:
                // Other default I need to remember. 
                break;
        }
    
    
        // Set ROM Size:
        cart->config.rom_size = 32 * 1024 << cart->header_config.rom_size_code;        // Each step will double the Ram size by 32
    
        cart->config.rom_banks = 2 << cart->header_config.rom_size_code;               // Each step doubles the rom Banks (Yes, even no MSB, technically is 2 ROM Banks)
    
    
        // Configure RAM Size:
        switch (cart->header_config.ram_size_code) {
            case 0x00: cart->config.ram_size = 0; break;
            case 0x01: cart->config.ram_size = 2 * 1024; break;
            case 0x02: cart->config.ram_size = 8 * 1024; break;
            case 0x03: cart->config.ram_size = 32 * 1024; break;
            case 0x04: cart->config.ram_size = 128 * 1024; break;
            case 0x05: cart->config.ram_size = 64 * 1024; break;
            default: cart->config.ram_size = 0; break;
        }
}


void configure_mbc(Cartridge *cart) {

    if (cart->config.mbc_type == 3) {           // MBC 3: Max 2MByte ROM and/or 32KByte RAM and Timer
        printf("\nDEBUG: Rom Matches MBC Cart Type 3 -- Upto 2MB ROM, and/or 32KB RAM and Timer\n\n");
        cart->mbc_write = mbc1_write;
    }
}