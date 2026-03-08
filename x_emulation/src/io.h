#ifndef IO_H
#define IO_H

#include <stdint.h>

typedef struct gb_s GB;

uint8_t io_read(GB *gb, uint16_t addr);
void io_write(GB *gb, uint16_t addr, uint8_t val);

#endif