#ifndef MBC_H
#define MBC_H

#include <stdint.h>

#include "cart_types.h"
#include "cart.h"

// Expose these function signatures to cart.c, mbc.c, mmu.c, e_ctrl... etc

typedef struct gb_s GB;

typedef struct Cartridge Cartridge;

typedef void (*mbc_write_func)(Cartridge *cart, uint16_t addr, uint8_t val);
typedef uint8_t (*mbc_read_func)(Cartridge *cart, uint16_t addr);

extern const Operations mbc1_ops;
extern const Operations mbc2_ops;
extern const Operations mbc3_ops;
extern const Operations mbc5_ops;

// The main configuration entry point for the MBC
int mbc_setup(GB *gb, Cartridge *cart, uint8_t type_code);


// MBC-1
void mbc1_write(Cartridge *cart, uint16_t addr, uint8_t val);
uint8_t mbc1_read(Cartridge *cart, uint16_t addr);
void mbc1_write_ext(Cartridge *cart, uint16_t addr, uint8_t val);
uint8_t mbc1_read_ext(Cartridge *cart, uint16_t addr);

// MBC-2
void mbc2_write(Cartridge *cart, uint16_t addr, uint8_t val);
uint8_t mbc2_read(Cartridge *cart, uint16_t addr);
void mbc2_write_ext(Cartridge *cart, uint16_t addr, uint8_t val);
uint8_t mbc2_read_ext(Cartridge *cart, uint16_t addr);

// MBC-3
void mbc3_write(Cartridge *cart, uint16_t addr, uint8_t val);
uint8_t mbc3_read(Cartridge *cart, uint16_t addr);
void mbc3_write_ext(Cartridge *cart, uint16_t addr, uint8_t val);
uint8_t mbc3_read_ext(Cartridge *cart, uint16_t addr);





#endif