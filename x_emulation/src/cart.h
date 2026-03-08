#ifndef CART_H
#define CART_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cart_types.h"

typedef struct gb_s GB;

// Rom Header:
#define HEADER_OFFSET       0x0100          // Same as Start, just easier to understand
#define HEADER_SIZE         0x50            // Header END point is: 0x014f (50 Bytes Total - 80 Decimal)
#define BANK_SIZE           0x4000

// Functions inside cart.c (Make it accessable elsewhere)
//int initialize_cartridge(const char *filename);
int cartridge_init(GB *gb, const char *filename);

int initialize_cartridge_simple();
int init_cart_test_mode();

// ENTRY POINTS (e_ctrl/ MMU):
uint8_t cart_rom_read(GB *gb, uint16_t addr);
void cart_rom_write(GB *gb, uint16_t addr, uint8_t val);
uint8_t cart_ram_read(GB *gb, uint16_t addr);
void cart_ram_write(GB *gb, uint16_t addr, uint8_t val);

#endif