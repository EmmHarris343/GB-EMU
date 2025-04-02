#ifndef CART_H
#define CART_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


// Rom Header:
#define HEADER_OFFSET       0x0100          // Same as Start, just easier to understand
#define HEADER_SIZE         0x50            // Header END point is: 0x014f (50 Bytes Total - 80 Decimal)
#define BANK_SIZE           0x4000


typedef void (*mbc_write_func)(uint16_t addr, uint8_t val);
typedef void (*mbc_read_func)(uint16_t addr);



typedef struct {    
    uint8_t entry_point[3];
    uint8_t logo[47];
    uint8_t title[16];       // +1 cause string.. I guess. --- Depending on rom not used at all, and non populated.
    uint8_t cart_type_code;
    uint8_t rom_size_code;
    uint8_t ram_size_code;
    uint8_t chksm;
    uint8_t gbl_chksm[2];    // Technically 16 bit....
    uint8_t cgb_f;
    uint8_t sgb_f;
    uint8_t new_lic[2];      // Only used if old licence = $33
    uint8_t old_lic;
} Headers;


typedef struct {
    uint8_t mbc_type;           // 0 = none, 1 = MBC1, 2 = MBC2, etc.
    uint8_t has_battery;        // 1 if battery-backed RAM
    uint8_t has_ram;            // 1 if external RAM exists
    uint8_t has_rtc;            // For MBC3 Real-Time Clock
    uint8_t has_rom_banking;    // Some MBC has weird ROM Banking, deciding it's worth while to know.
    uint8_t has_ram_banking;
    size_t rom_size;           // Size of Entire ROM
    size_t ram_size;
    size_t rom_bank_count;     // Total of rom banks
    size_t ram_bank_count;     // RAM (External Memory Banks)
} Config;

typedef struct {
    uint8_t *rom_data;              // Actual Full Rom data
    uint8_t *ram_data;
    uint8_t ram_toggle;             // I might not use these addr values...
    uint8_t fixed_b_addr;           // 0000 - 4000
    uint8_t calcd_switch_addr;      // 4000	7FFF            n * 4000 = switch bank addr (where n = rom_bank)
    uint8_t current_rom_bank;
    uint8_t current_ram_bank;
} Resources;




typedef struct {
    Headers headers;
    Config config;    
    Resources resrce;
    mbc_write_func mbc_write;
    mbc_read_func mbc_read;
} Cartridge;




// Functions inside cart.c (Make it accessable elsewhere)
int load_headers(const char *filename);
int decode_cart_features();
int load_cartridge(const char *filename);
int initialize_cartridge();

// Test
void write_intercept(uint16_t address, uint8_t data);




// ENTRY POINTS (e_ctrl/ MMU):

uint8_t cart_read(uint16_t addr);
void cart_write(uint16_t addr, uint8_t val);
uint8_t cart_ram_read(uint16_t addr);
void cart_ram_write(uint16_t addr, uint8_t val);

#endif