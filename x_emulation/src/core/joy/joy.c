#include "joy.h"
#include "../gb.h"

/*
Note:

The bit layout is as follows:
bit 5: Select buttons
bit 4: Select d-pad
bit 3: Start/Down
bit 2: Select/UP
bit 1: B/Left
bit 0: A/ Right


But as well, GB is kind of inversed.
1 = not pushed
0 = pushed
*/





/*

Ok......
This requires a few states.
The constant states that are assigned by the SDL2 window / button presses / releases state.
Basically a struct with, d-pad up,down,left,right. button select,start,b,a. Whenever SDL changes. Those change from 1/ 0.


The second is. "Read-Mode". The CPU will poll the register 0xFF00. With 4 values (For DMG 1):
$10 bit5 selected (Button read mode) - Set bit 0-3 with the currently select button pushed.
$20 bit4 selected (d-pad read mode) - Set bit 0-3 with the current d-pad direction pushed.
$30 bit4/5 neither selected. - Documantation says bit 0-3 should show F (all button released).
$00 both bit4/5 selected. - Very rare. I may be able to ignore.

*/

void joy_io_init(GB *gb){
    gb->joy.d_up    = 0x00;
    gb->joy.d_down  = 0x00;
    gb->joy.d_left  = 0x00;
    gb->joy.d_right = 0x00;

    gb->joy.select  = 0x00;
    gb->joy.start   = 0x00;
    gb->joy.a       = 0x00;
    gb->joy.b       = 0x00;

    gb->joy.mode    = 0x00;    // Write intercepted mode select. "Select Buttons" or "Select D-Pad"
}

/*
Important!
When setting the joy register 0xFF00.
    Bit 5 or 4. Need to be 1 (for selected).
    Inversly, bit 3-0. Need to be 0 (for pressed). 1 is "released".
*/

uint8_t joy_io_read(GB *gb, uint16_t addr){
    // Reset "selected" and "pressed" bits. IE: 1100 1111
    uint8_t calc_reg = 0x0F;

    switch (gb->joy.mode) {
        case 0x10: // Mode = Select Buttons:
            calc_reg = (1 << 5);
            if (gb->joy.a) calc_reg      &= ~(1 << 0);
            if (gb->joy.b) calc_reg      &= ~(1 << 1);
            if (gb->joy.start) calc_reg  &= ~(1 << 2);
            if (gb->joy.select) calc_reg &= ~(1 << 3);
            return calc_reg;
        case 0x20: // Mode = Select D-Pad:
            calc_reg = (1 << 4);
            if (gb->joy.d_right) calc_reg &= ~(1 << 0);
            if (gb->joy.d_left) calc_reg  &= ~(1 << 1);
            if (gb->joy.d_up) calc_reg    &= ~(1 << 2);
            if (gb->joy.d_down) calc_reg  &= ~(1 << 3);
            return calc_reg;
        case 0x30: // Mode = Select Neither. Documentation says return 0x0F (All keys released)
            calc_reg= 0x0F;
            return calc_reg;
    }
    return calc_reg;
}
void joy_io_write(GB *gb, uint16_t addr, uint8_t write_val){
    if (addr == 0xFF00) {
        gb->joy.mode = (write_val & 0x30);    // Allows only bit4/5 to be set.
    }
}