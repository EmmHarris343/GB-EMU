#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/*
Timers..
DIV at FF04
TIMA at FF05
TMA at FF06
TAC at FF07

Names:
DIV = divider register
TIMA = timer counter
TMA = timer modulo / reload value
TAC = timer control register
*/

typedef struct gb_s GB;

struct timer_s {
    uint8_t div;    // On DMG it increments at 16384 Hz, which corresponds to every 256 M-cycles. Writing to DIV resets it to 00.
    uint8_t tima;   // Iccrements only when enabled by TAC, using one of four clock rates selected by the low 2 bits of
    uint8_t tma;
    uint8_t tac;

    uint16_t div_counter;
    uint16_t tima_counter;
};


uint8_t timer_div_read(GB *gb, uint16_t addr);
uint8_t timer_tima_read(GB *gb, uint16_t addr);
uint8_t timer_tma_read(GB *gb, uint16_t addr);
uint8_t timer_tac_read(GB *gb, uint16_t addr);

void timer_div_write(GB *gb, uint16_t addr, uint8_t val);
void timer_tima_write(GB *gb, uint16_t addr, uint8_t val);
void timer_tma_write(GB *gb, uint16_t addr, uint8_t val);
void timer_tac_write(GB *gb, uint16_t addr, uint8_t val);


#endif