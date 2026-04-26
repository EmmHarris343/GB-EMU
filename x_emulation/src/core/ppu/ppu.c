#include "ppu.h"
#include "../gb.h"
#include <string.h>


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


//// NOTE: PPU should own: LCD, LCD Registers, VRAM, OAM, framebuffer





/*

The LCDC bit values:
LCDC bits are:
bit 7 = LCD enabl/disable
bit 6 = window tile map
bit 5 = window enable/disable
bit 4 = BG/window tile data area
bit 3 = BG tile map
bit 2 = sprite size
bit 1 = sprites enable/disable
bit 0 = BG enable/disable

*/




// Set/ Reset the VRAM back to all 0s:
void init_vram(GB *gb) {
    memset(gb->ppu.vram, 0, sizeof(gb->ppu.vram));
    return;
}

void init_oam_ram(GB *gb) {
    memset(gb->ppu.oam, 0, sizeof(gb->ppu.oam));
    return;
}

const PPU ppu_post_startup = {
    .lcdc = 0x91,   // FF40
    .stat = 0x85,   // FF41
    .scy = 0,       // FF42
    .scx = 0,       // FF43
    .ly = 0,        // FF44
    .lyc = 0,       // FF45

    .bgp = 0xFC,    // FF47
    .obp0 = 0xFF,   // FF48
    .obp1 = 0xFF,   // FF49
    .wy = 0,        // FF4A
    .wx = 0,        // FF4B
    .y_condition = 0
};


/// TODO: Do I want a post-boot automatically boot mode.
void ppu_init_timer(PPU *ppu) {
    ppu->lcdc = 0x91;   // FF40
    ppu->stat = 0x85;   // FF41
    ppu->scy = 0;       // FF42
    ppu->scx = 0;       // FF43
    ppu->ly = 0;        // FF44
    ppu->lyc = 0;       // FF45

    ppu->bgp = 0xFC;    // FF47
    ppu->obp0 = 0xFF;   // FF48
    ppu->obp1 = 0xFF;   // FF49

    ppu->wy = 0;        // FF4A
    ppu->wx = 0;        // FF4B

    ppu->y_condition = 0;

    // ppu->oam[0xA0] = 0;


    ppu->line_cycles = 0;
    ppu->mode = 0;
}

void ppu_init_reg_map(PPU *ppu) {
    for (size_t i = 0; i < 0x100; i++) {    // Not super efficient. But this is only called during init!
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
    printf(":PPU: Initializing PPU; Registers and VRAM.\n");

    // VRAM:
    init_vram(gb);
    // OAM:
    init_oam_ram(gb);   // Not really ram. but ehhh don't care.

    // Timer, set all values to 0.
    ppu_init_timer(&gb->ppu);

    // Initialize the register map.
    ppu_init_reg_map(&gb->ppu);

    printf("Done.\n");

    return 0;
}


/*

---- OAM OBJECTS ----

*/

uint8_t oam_read(GB *gb, uint16_t addr) {
    if (addr >= 0xFE00 && addr <= 0xFE9F) { // Allowed range:
        uint16_t offset = addr - 0xFE00;
        return gb->ppu.oam[offset];
    }
    printf("oam: OAM invalid Read! -> Addr:0x%04X. Returning 0xFF.\n", addr);
    return 0xFF;
}
void oam_write(GB *gb, uint16_t addr, uint8_t write_val) {
    if (addr >= 0xFE00 && addr <= 0xFE9F) { // Allowed range:
        uint16_t offset = addr - 0xFE00;
        gb->ppu.oam[offset] = write_val;
        return;
    }
    printf("oam: OAM Write invalid Write! -> Addr: 0x%04X. WriteVal: 0x%02X\n", addr, write_val);
    return;
}

void oam_dma_transfer(GB *gb, uint8_t write_val) {
    uint16_t source_addr = (uint16_t)write_val << 8;

    for (uint16_t i = 0; i < 160; i++) {
        gb->ppu.oam[i] = mmu_read(gb, source_addr + i);
    }
}



/*

---- REST OF PPU CODE ----

*/



static void ppu_update_lyc_flag(GB *gb, PPU *ppu) {
    uint8_t old_match = (ppu->stat >> 2) & 0x01;
    uint8_t new_match = (ppu->ly == ppu->lyc) ? 1u : 0u;

    if (new_match) {
        ppu->stat |= (1u << 2);
    } else {
        ppu->stat &= ~(1u << 2);
    }

    if (!old_match && new_match) {
        // printf("[PPU] LYC match rising edge: LY=%02X LYC=%02X STAT=%02X\n",
        //     ppu->ly, ppu->lyc, ppu->stat);
        if (ppu->stat & (1u << 6)) {
            // printf("[PPU] Request LCD STAT from LYC match\n");
            gb_request_interrupt(gb, GB_INTERRUPT_LCD_STAT);
        }
    }
}

// PPU Set mode, sets the STAT mode for HBLANK, VBLANK, OAM, TRANSFER.
static void ppu_set_mode(GB *gb, PPU *ppu, int mode) {

    uint8_t previous_mode = ppu->stat & 0x03;

    if (previous_mode == mode) {
        //printf("PPU: set mode same, return.\n");
        return;
    }

    // & 0xFC -> Removes first two bits 0000 00xx
    // & 0x03 -> Filters only the first 2 bits.
    ppu->stat = (ppu->stat & 0xFC) | (mode & 0x03);

    /* STAT interrupt enable:
        bit 3 = HBLANK
        bit 4 = VBLANK
        bit 5 = OAM
    */

    switch (mode) {
        case PPU_MODE_HBLANK:   // Mode 0; int select
            if (ppu->stat & (1 << 3)) {
                gb_request_interrupt(gb, GB_INTERRUPT_LCD_STAT);
            }
            break;
        case PPU_MODE_VBLANK:   // Mode 1; int select
            gb_request_interrupt(gb, GB_INTERRUPT_VBLANK);   // always request the interrupt
            if (ppu->stat & (1 << 4)) {
                gb_request_interrupt(gb, GB_INTERRUPT_LCD_STAT);
            }
            break;
        case PPU_MODE_OAM:      // Mode 2; int select
            if (ppu->stat & (1 << 5)) {
                gb_request_interrupt(gb, GB_INTERRUPT_LCD_STAT);
            }
            break;
        case PPU_MODE_TRANSFER:
            break;
    }
}

// PPU VRAM functions:
uint8_t ppu_vram_read(GB *gb, uint16_t addr){
    if (addr >= 0x8000 && addr <= 0x9FFF) {
        return gb->ppu.vram[addr - 0x8000];
    }
    printf("PPU: VRAM invalid Read! -> Addr:0x%04X. Returning 0xFF.\n", addr);
    return 0xFF;
}
void ppu_vram_write(GB *gb, uint16_t addr, uint8_t write_val){
    if (addr >= 0x8000 && addr <= 0x9FFF) {
        gb->ppu.vram[addr - 0x8000] = write_val;
        return;
    }
    printf("PPU: VRAM invalid Write! -> Addr: 0x%04X. WriteVal: 0x%02X\n", addr, write_val);
    return;
}


// IO section:

uint8_t ppu_stat_read(GB *gb) {
    return gb->ppu.stat | 0x80;
}

void ppu_lcdc_write(GB *gb, uint8_t write_val) {
    PPU *ppu = &gb->ppu;
    uint8_t old_value = ppu->lcdc;

    gb->ppu.lcdc = write_val;

    // if ((old_value ^ write_val) & 0x80) {
    //     printf("[PPU] LCDC changed %02X -> %02X | LCD %s\n",
    //            old_value, write_val, (write_val & 0x80) ? "ON" : "OFF");
    // }
}
void ppu_stat_write(GB *gb, uint8_t write_val) {
    // printf("PPU: ppu.stat write hit! WriteVal: 0x%02X\n", write_val);
    uint8_t stat = gb->ppu.stat;

    /* keep bits 0-2 from the current STAT */
    stat = (stat & 0x07) | (write_val & 0x78);

    gb->ppu.stat = stat;
}
void ppu_ly_write(GB *gb, uint8_t write_val) {
    // LY technically shouldn't be directly written to. As it's a "calculated" value.
    printf("PPU: ppu.ly write hit! .. THIS NEVER SHOULD HAPPEN. (I THINK) WriteVal: 0x%02X\n", write_val);
    (void)write_val;
    gb->ppu.ly = 0;
    ppu_update_lyc_flag(gb, &gb->ppu);
}
void ppu_lyc_write(GB *gb, uint8_t write_val){
    gb->ppu.lyc = write_val;
    ppu_update_lyc_flag(gb, &gb->ppu);
}


uint8_t ppu_io_read(GB *gb, uint16_t addr) {
    switch (addr) {
        case 0xFF41:
            return ppu_stat_read(gb);
        default: {
            uint8_t index = (uint8_t)(addr & 0x00FF);
            uint8_t *reg_ptr = ppu_regs[index];

            if (reg_ptr != NULL) {
                //printf("PPU: Default. IO_READ HIT! Addr=0x%04X, RegPtr Val= 0x%02X\n", addr, *reg_ptr);
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
        case 0xFF45:
            ppu_lyc_write(gb, write_val);
            return;
        case 0xFF46:
            oam_dma_transfer(gb, write_val);   // DMA
            return;
        default: {
            uint8_t index = (uint8_t)(addr & 0x00FF);
            uint8_t *reg_ptr = ppu_regs[index];

            if (reg_ptr != NULL) {
                //printf("PPU: Default. IO_WRITE HIT! Addr=0x%04X, RegPtr Val= 0x%02X, WriteVal=0x%02X\n", addr, *reg_ptr, write_val);
                *reg_ptr = write_val;
            }
            return;
        }
    }
    return;
}
/*

    PPU timer/ tick
    Handles HBLANK, VBLANK, etc..

*/

/*

456 cycles matters because:
Scanline is made of phases called PPU modes:
Mode 0 = HBlank
Mode 1 = VBlank
Mode 2 = OAM scan
Mode 3 = pixel transfer


Basic timings (not supper accurate):
Mode 2: ~80 Cycles
Mode 3: ~172 Cycles
Mode 0: ~204 Cycles

80 + 172 + 204 = 456 cycles.

So line_cycles is the per-scanline timing accumulator.

*/


/// BIG: TODO: --- I completely forgot to setup the DMA IO code/ PPU code (technilly it might not be PPU).


// PPU Tick, advances through scanliens and modes based on elasped cycles.
// NOTE: This also controls WHEN the pixels are drawn inside display.c!!
void ppu_tick(GB *gb, PPU *ppu, uint32_t cycles) {
    uint8_t old_mode = ppu->stat & 0x03;
    uint8_t new_mode;
    if ((ppu->lcdc & 0x80) == 0) {
        ppu->line_cycles = 0;
        ppu->ly = 0;
        ppu_set_mode(gb, ppu, PPU_MODE_HBLANK); // 87=204 dots (Mode 0)
        //new_mode = PPU_MODE_HBLANK;
        ppu_update_lyc_flag(gb, ppu);
        return;
    }

    ppu->line_cycles += cycles;
    while (ppu->line_cycles >= 456) {   // Each scanline takes 456 Cycles. Or 456 Dots.
        ppu->line_cycles -= 456;
        ppu->ly++;

        ppu_update_lyc_flag(gb, ppu);

        // Visable Scanlines y = (0 - 143)
        // VBlank  Scanlines y = (144 - 153)

        if (ppu->ly == ppu->wy) {
            ppu->y_condition = 1;
        }

        if (ppu->ly == 144) {
            // When y reaches 144, Switches to Mode 1 VBlank and request interrupt.
            gb->db_stats.vblank_entries += 1;

            ppu->y_condition = 0;   // Y condition (for window), resets during vblank.
            ppu_set_mode(gb, ppu, PPU_MODE_VBLANK);
            //new_mode = PPU_MODE_VBLANK;
        } else if (ppu->ly > 153) {
            // When y reaches 153+ reset ly, switch to Mode 2 OAM for 80 dots.
            ppu->ly = 0;
            gb->db_stats.ly_wraps +=1;

            ppu_set_mode(gb, ppu, PPU_MODE_OAM);
            //new_mode = PPU_MODE_OAM;
            ppu_update_lyc_flag(gb, ppu);
        }
    }
    if (ppu->ly < 144) {
        if (ppu->line_cycles < 80) {
            //ppu_set_mode(gb, ppu, PPU_MODE_OAM);
            new_mode = PPU_MODE_OAM;
        } else if (ppu->line_cycles < (80 + 172)) {
            //ppu_set_mode(gb, ppu, PPU_MODE_TRANSFER);
            new_mode = PPU_MODE_TRANSFER;
        } else {
            //ppu_set_mode(gb, ppu,PPU_MODE_HBLANK);
            new_mode = PPU_MODE_HBLANK;
        }

        if (old_mode != PPU_MODE_HBLANK && new_mode == PPU_MODE_HBLANK) {
            gen_pixel_line(&gb->ppu, gb->ppu.vram);
        }

        ppu_set_mode(gb, ppu, new_mode);
    }

    ppu_update_lyc_flag(gb, ppu);
}










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
