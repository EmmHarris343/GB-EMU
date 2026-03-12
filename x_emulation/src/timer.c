#include "timer.h"
#include "gb.h"


/*
The timer logic..

Cycles should be added to div_counter
everytime div_counter >= 256 subtract -256, and increment div.


If timer enabled, add cycles to tima_counter.
when tima_counter reaches the selected period, increment tima.

On overflow. load tma into tima and request timer interrupt.

*/

// FF04	DIV	    Timer
// FF05	TIMA	Timer
// FF06	TMA	    Timer
// FF07	TAC	    Timer


uint8_t timer_div_read(GB *gb, uint16_t addr) { // 0xFF04
    return 0;
}
uint8_t timer_tima_read(GB *gb, uint16_t addr) { // 0xFF05
    return 0;
}
uint8_t timer_tma_read(GB *gb, uint16_t addr) { // 0xFF06
    return 0;
}
uint8_t timer_tac_read(GB *gb, uint16_t addr) { // 0xFF07
    return 0;
}



void timer_div_write(GB *gb, uint16_t addr, uint8_t val) { // 0xFF04

}
void timer_tima_write(GB *gb, uint16_t addr, uint8_t val) { // 0xFF05

}
void timer_tma_write(GB *gb, uint16_t addr, uint8_t val) { // 0xFF06

}
void timer_tac_write(GB *gb, uint16_t addr, uint8_t val) { // 0xFF07

}