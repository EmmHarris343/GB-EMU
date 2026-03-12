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


/*
The Tac frequency table:
BITS    | Timer Frequency   | Increment every
00      |    4096 HZ        |   1024 cycles
01      |    262,144 HZ     |   16 cycles
10      |    65,536 HZ      |   64 cycles
11      |    16,384 HZ      |   256 cycles

*/
static uint32_t tac_frequency(uint8_t tac) {
    switch (tac & 0x03) {
        // case 0x00: return 256;
        // case 0x01: return 4;
        // case 0x02: return 16;
        // case 0x03: return 64;
        case 0x00: return 1024;
        case 0x01: return 16;
        case 0x02: return 64;
        case 0x03: return 256;
    }
    return 1024;
}

void timer_tick(GB *gb, Timer *timer, uint32_t cycles) {
    printf("Executing Timer Tick. Cycles: %u\n", cycles);
    // Div:
    timer->div_cycles += cycles;

    while (timer->div_cycles>= 256) {
        timer->div_cycles -= 256;
        timer->div ++;
    }

    if ((timer->tac & 0x04) == 0) { // Verify if timer is enabled.
        return;
    }

    // Tima:
    timer->tima_cycles += cycles;

    uint32_t period = tac_frequency(timer->tac);

    while (timer->tima_cycles >= period) {
        timer->tima_cycles -= period;

        // When tima overflows 0xFF -> 0x00, reload tima from tma, then request interupt.
        if (timer->tima == 0xFF){
            timer->tima = timer->tma;
            gb_request_interrupt(gb, 2);    // Timer interrupt bit.
        } else {
            timer->tima ++;
        }
    }
}