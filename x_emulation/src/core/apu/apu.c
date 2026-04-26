#include "apu.h"
#include "../gb.h"


int apu_init(GB *gb) {
    return 0;
}

uint8_t apu_io_read(GB *gb, uint16_t addr){
    if (addr >= 0xFF10 && addr <= 0xFF3F) {
        return 0x00;
    }
    return 0xFF;
}
uint8_t apu_wave_read(GB *gb, uint16_t addr) {
    if (addr >= 0xFF10 && addr <= 0xFF3F) {
        return 0x00;
    }
    return 0xFF;
}

void apu_io_write(GB *gb, uint16_t addr, uint8_t write_val){
    (void)addr;
    (void)write_val;
    if (addr >= 0xFF10 && addr <= 0xFF3F) {
        return;
    }
    return;
}
void apu_wave_write(GB *gb, uint16_t addr, uint8_t write_val){
    (void)addr;
    (void)write_val;
    if (addr >= 0xFF10 && addr <= 0xFF3F) {
        return;
    }
    return;
}
