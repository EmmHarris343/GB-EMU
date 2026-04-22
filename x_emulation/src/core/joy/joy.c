#include "joy.h"
#include "../gb.h"

/*
The Joy Register 0xFF00 is shared/ multiplexed.

If the CPU writes to the Joy Register to change mode with values:
    $10 Button Read Mode. $20 D-Pad Read Mode. $30 Neither Mode.

When the CPU reads the Joy Register, 0xFF00.
For Button Read Mode
    -> Bit 5 must be set (1)
    -> Bit 3-0 is the start, sel, b, a, buttons.
For D-Pad Read mode:
    -> Bit 4 Must be set (1)
    -> Bit 3-0 is the down, up, left, right.

While the select mode is bit5/4 = 1. The 3-0 bits for buttons/ d-pad. 0 = Pressed. 1 = Released.

Example:
    0010 0111 => Button Read MOde. Start Pushed.
    0001 1110 => D-Pad Read Mode. Right Pushed.
*/

int joy_io_init(GB *gb){
    gb->joy.d_up    = 0x00;
    gb->joy.d_down  = 0x00;
    gb->joy.d_left  = 0x00;
    gb->joy.d_right = 0x00;

    gb->joy.select  = 0x00;
    gb->joy.start   = 0x00;
    gb->joy.a       = 0x00;
    gb->joy.b       = 0x00;

    gb->joy.mode    = 0x00;    // Write intercepted mode select. "Select Buttons" or "Select D-Pad"

    return 0;
}

uint8_t joy_io_read(GB *gb, uint16_t addr){
    uint8_t calc_reg = 0x0F;
    switch (gb->joy.mode) {
        case 0x10: // Mode = Select Buttons:
            calc_reg |= (1 << 5);
            if (gb->joy.a) calc_reg      &= ~(1 << 0);
            if (gb->joy.b) calc_reg      &= ~(1 << 1);
            if (gb->joy.start) calc_reg  &= ~(1 << 2);
            if (gb->joy.select) calc_reg &= ~(1 << 3); // Note Bit 3-0 pressed = 0, released = 1.
            break;
        case 0x20: // Mode = Select D-Pad:
            calc_reg |= (1 << 4);
            if (gb->joy.d_right) calc_reg &= ~(1 << 0);
            if (gb->joy.d_left) calc_reg  &= ~(1 << 1);
            if (gb->joy.d_up) calc_reg    &= ~(1 << 2);
            if (gb->joy.d_down) calc_reg  &= ~(1 << 3); // Note Bit 3-0 pressed = 0, released = 1.
            break;
        case 0x30: // Mode = Select Neither. Documentation says return 0x0F (All keys released)
            calc_reg = 0x0F;
            break;
    }
    return calc_reg;
}
void joy_io_write(GB *gb, uint16_t addr, uint8_t write_val){
    if (addr == 0xFF00) {
        gb->joy.mode = (write_val & 0x30);    // Limits setting of only bit5/4
    }
}