

#include "apu.h"
#include "gb.h"


int apu_init(GB *gb) {
    return 0;
}

uint8_t apu_read(GB *gb, uint16_t addr) {
    return 0xFF;

}
void apu_write(GB *gb, uint16_t addr, uint8_t val)
{
    (void)addr;
    (void)val;
}
