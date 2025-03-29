#ifndef APU_H
#define APU_H

#include <stdint.h>

uint8_t apu_read(uint16_t addr);
void apu_write(uint16_t addr, uint8_t val);

#endif