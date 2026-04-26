#include "io.h"
#include "gb.h"
#include "ppu/ppu.h"
#include "timer/timer.h"
#include "apu/apu.h"
#include "joy/joy.h"

//#include "logger.h"

uint8_t serial_io_read(GB *gb, uint16_t addr) {
    if (addr == 0xFF01) {   // SB
        return 0xFF;
    }
    if (addr == 0xFF02) {   // SC
        return gb->io.SC;
    }
    return 0xFF;
}
void serial_io_write(GB *gb, uint16_t addr, uint8_t write_val) {
    if (addr == 0xFF01) {   // SB
        gb->io.SB = write_val;
        // printf("[SERIAL] SB=%02X PC=%04X ROM_B=%02X\n",
        //     write_val, gb->cpu.reg.PC, gb->cart.state.mbc3.current_rom_bank);
        return;
    }
    if (addr == 0xFF02) {    // SC
        gb->io.SC = write_val;
        // printf("[SERIAL] SC=%02X PC=%04X ROM_B=%02X\n",
        //         write_val, gb->cpu.reg.PC, gb->cart.state.mbc3.current_rom_bank);
        if (write_val == 0x81) {
            gb->io.SC = 0x01;              // transfer no longer active
            gb->interrupts.IF |= (1u << 3); // serial interrupt
        }
        return;
    }
    return;
}

// Normally the I/O Registers are from/to: FF00-FF7F

// IO Register Routing Map:
static IO_RegMap io_map[] = {
    {0xFF00, 0xFF00, joy_io_read, joy_io_write, IO_INPUT},
    {0xFF01, 0xFF02, serial_io_read, serial_io_write, IO_SERIAL},
    {0xFF04, 0xFF04, timer_div_read, timer_div_write, IO_TIMER},
    {0xFF05, 0xFF05, timer_tima_read, timer_tima_write, IO_TIMER},
    {0xFF06, 0xFF06, timer_tma_read, timer_tma_write, IO_TIMER},
    {0xFF07, 0xFF07, timer_tac_read, timer_tac_write, IO_TIMER},
    {0xFF10, 0xFF26, apu_io_read, apu_io_write, IO_APU},
    {0xFF30, 0xFF3F, apu_wave_read, apu_wave_write, IO_APU_RAM},
    {0xFF40, 0xFF4B, ppu_io_read, ppu_io_write, IO_PPU},
};
const size_t io_map_size = sizeof(io_map) / sizeof(IO_RegMap);

uint8_t io_read(GB *gb, uint16_t addr) {
    uint8_t read_8bit = 0xFF;
    if (io_map_size <= 0) {
        printf("ERROR: io_map is emtpy!\n");
        exit(1);
    }
    for (int i = 0; i < io_map_size; i++) {
        if (addr >= io_map[i].start && addr <= io_map[i].end) {
            read_8bit = io_map[i].read(gb, addr);
            //trace_io_read(addr, read_8bit, i, (uint8_t)io_map[i].tag);
            return read_8bit;
        }
    }
    return read_8bit;
}

void io_write(GB *gb, uint16_t addr, uint8_t write_val)
{
    if (io_map_size <= 0) {
        printf("ERROR: io_map is emtpy!\n");
        exit(1);
    }
    for (int i = 0; i < io_map_size; i++) {
        if (addr >= io_map[i].start && addr <= io_map[i].end) {       // Changed >= is this right?
            io_map[i].write(gb, addr, write_val);
            //trace_io_write(addr, write_val, i, (uint8_t)io_map[i].tag);
        }
    }
}

