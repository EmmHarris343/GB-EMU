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

typedef struct timer_s {
    uint8_t div;    // On DMG it increments at 16384 Hz, which corresponds to every 256 clock or cpu cycles (instructions)
    uint8_t tima;   // Counter that increments when the programmable timer fires.
    uint8_t tma;    // Will cause TIMA to get relaoded from TMA and requests timer interrupt (when tima overflows 0xFF -> 0x00)
    uint8_t tac;    // Controls wether the timer runs, and at what frequency. Bit 2= enabled. Bit1-0 = frequency select.

    uint16_t div_cycles;
    uint16_t tima_cycles;
} Timer;


uint8_t timer_div_read(GB *gb, uint16_t addr);
uint8_t timer_tima_read(GB *gb, uint16_t addr);
uint8_t timer_tma_read(GB *gb, uint16_t addr);
uint8_t timer_tac_read(GB *gb, uint16_t addr);

void timer_div_write(GB *gb, uint16_t addr, uint8_t val);
void timer_tima_write(GB *gb, uint16_t addr, uint8_t val);
void timer_tma_write(GB *gb, uint16_t addr, uint8_t val);
void timer_tac_write(GB *gb, uint16_t addr, uint8_t val);


void timer_tick(GB *gb, Timer *timer, uint32_t cycles);


#endif