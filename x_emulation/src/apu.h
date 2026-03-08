#ifndef APU_H
#define APU_H

#include <stdint.h>

typedef struct gb_s GB;

int apu_init(GB *gb);


uint8_t apu_read(GB *gb, uint16_t addr);
void apu_write(GB *gb, uint16_t addr, uint8_t val);

#endif