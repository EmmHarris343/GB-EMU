#include "joy.h"
#include "gb.h"



uint8_t joy_io_read(GB *gb, uint16_t addr){
    return 0xFF;
}
void joy_io_write(GB *gb, uint16_t addr, uint8_t write_val){
    (void)addr;
    (void)write_val;
}