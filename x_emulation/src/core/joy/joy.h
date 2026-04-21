#ifndef JOY_H
#define JOY_H

#include <stdint.h>

typedef struct gb_s GB;

typedef struct joy_s {
    uint8_t select;
    uint8_t start;
    uint8_t b;
    uint8_t a;
    uint8_t d_up;
    uint8_t d_down;
    uint8_t d_left;
    uint8_t d_right;

    uint8_t mode;
} JOY;


uint8_t joy_io_read(GB *gb, uint16_t addr);
void joy_io_write(GB *gb, uint16_t addr, uint8_t write_val);

#endif