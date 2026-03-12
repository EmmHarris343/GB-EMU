#include "ppu.h"
#include "gb.h"
#include <string.h>


static uint8_t VRAM[VRAM_SIZE]; // Make only available directly from PPU.
static uint8_t *ppu_regs[0x100];

//PPUIO_RegMap *ppu_regs[0x100];


/*

PPU is:
A time-driven "state-machine".
Which controls line-draws, cycles, when to set

Control for io for LCD, scan lines, draws.

VRAM read/writes.




Vram, read / writes.
IO calls for LCD, change view-point coordinates, y (scan-line draw).


Counts for line draws, cycles, when y-reachs end.
VBank mode, Requests interupts to IF.

Allow OAM (sprits/ tiles) for new frames.

*/






// Set/ Reset the VRAM back to all 0s:
void init_vram(GB *gb) {
    memset(VRAM, 0, VRAM_SIZE);

}

void ppu_init_timer(PPU *ppu) {
    ppu->lcdc = 0;      // FF40
    ppu->stat = 0;      // FF41
    ppu->scy = 0;       // FF42
    ppu->scx = 0;       // FF43
    ppu->ly = 0;        // FF44
    ppu->lyc = 0;       // FF45

    ppu->bgp = 0;       // FF47
    ppu->obp0 = 0;      // FF48
    ppu->obp1 = 0;      // FF49

    ppu->wy = 0;        // FF4A
    ppu->wx = 0;        // FF4B


    ppu->oam[0xA0] = 0;
    ppu->vram[0x2000] = 0;

    ppu->line_cycles = 0;
    ppu->mode = 0;
}

void ppu_init_reg_map(PPU *ppu) {
    for (size_t i = 0; i < 0x100; i++) {
        ppu_regs[i] = NULL;
    }
    // Normally Registers at location: 0xFF40 - 0xFF4B:
    ppu_regs[0x40] = &ppu->lcdc;
    ppu_regs[0x41] = &ppu->stat;
    ppu_regs[0x42] = &ppu->scy;
    ppu_regs[0x43] = &ppu->scx;
    ppu_regs[0x44] = &ppu->ly;
    ppu_regs[0x45] = &ppu->lyc;
    ppu_regs[0x47] = &ppu->bgp;
    ppu_regs[0x48] = &ppu->obp0;
    ppu_regs[0x49] = &ppu->obp1;
    ppu_regs[0x4A] = &ppu->wy;
    ppu_regs[0x4B] = &ppu->wx;
}

int ppu_init(GB *gb) {
    printf("Initializing PPU; Registers and VRAM.\n");
    // VRAM:
    init_vram(gb);

    // Timer, set all values to 0.
    ppu_init_timer(&gb->ppu);

    // Initialize the register map.
    ppu_init_reg_map(&gb->ppu);

    return 0;
}


void ppu_set_mode(PPU *ppu, int val) {

}

uint8_t ppu_stat_read(GB *gb) {
    return 0;
}

void ppu_lcdc_write(GB *gb, uint8_t write_val) {

}
void ppu_stat_write(GB *gb, uint8_t write_val) {

}
void ppu_ly_write(GB *gb, uint8_t write_val) {
    // LY technically should be directly written to. As it's a "calculated" value.

}






uint8_t ppu_io_read(GB *gb, uint16_t addr) {
    switch (addr) {
        case 0xFF41:
            return ppu_stat_read(gb);
        default: {
            uint8_t index = (uint8_t)(addr & 0x00FF);
            uint8_t *reg_ptr = ppu_regs[index];

            if (reg_ptr != NULL) {
                return *reg_ptr;
            }

            return 0xFF;
        }
    }
    return 0xFF;
}
void ppu_io_write(GB *gb, uint16_t addr, uint8_t write_val) {
    switch (addr) {
        case 0xFF40:
            ppu_lcdc_write(gb, write_val);
            return;
        case 0xFF41:
            ppu_stat_write(gb, write_val);
            return;
        case 0xFF44:    // Note, ly is technically read only on GB
            ppu_ly_write(gb, write_val);
            return;
        default: {
            uint8_t index = (uint8_t)(addr & 0x00FF);
            uint8_t *reg_ptr = ppu_regs[index];

            if (reg_ptr != NULL) {
                *reg_ptr = write_val;
            }
            return;
        }
    }
    return;
}

// PPU VRAM functions:
uint8_t ppu_vram_read(GB *gb, uint16_t addr){
    printf(":ppu: Hit VRAM Read!\n");
    if (addr < 0x8000 || addr > 0x9FFF) {
        // Invalid
        return 0xFF;
    }
    return VRAM[addr - 0x8000];
}
void ppu_vram_write(GB *gb, uint16_t addr, uint8_t write_val){
    printf(":ppu: Hit VRAM WRITE!\n");
    if (addr < 0x8000 || addr > 0x9FFF) {
        // Invalid
        return;
    }
    VRAM[addr - 0x8000] = write_val;
}


// PPU Timer/ tick:
void ppu_tick(PPU *ppu, GB *gb, uint32_t cycles) {
    if ((ppu->lcdc & 0x80) == 0) {
        ppu->line_cycles = 0;
        ppu->ly = 0;
        ppu_set_mode(ppu, 0);
        return;
    }
}


// More advanced PPU tick:










/*

The GB has a built-in LCD screen of 160 × 144 pixels. (Not sure how I make it appear larger)



SCY and SCX defines the X- and Y-
coordinate of the background tile view


LY indicates the line, that is about to be
drawn, and setting LYC can enable an in-
terrupt, when LY and LYC are equal. If the
condition is met, a flag in the STAT register
is set.

*/
