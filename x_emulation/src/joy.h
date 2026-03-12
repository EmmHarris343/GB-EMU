#ifndef JOY_H
#define JOY_H

#include <stdint.h>

typedef struct gb_s GB;



uint8_t joy_io_read(GB *gb, uint16_t addr);
void joy_io_write(GB *gb, uint16_t addr, uint8_t write_val);

#endif