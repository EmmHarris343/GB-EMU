#ifndef GB_H
#define GB_H

#include <stdint.h>
#include "cpu.h"
#include "mmu.h"
#include "ppu.h"
#include "timer.h"
#include "cart.h"
#include "mbc.h"

typedef struct gb_s {
    CPU cpu;
    struct mmu_s *mmu;
    struct ppu_s *ppu;
    struct timer_s *timer;
    struct cart_s *cart;

    uint64_t total_cycles;
    uint32_t frame_cycles;
} GB;

int gb_init(GB *gb);

int gb_run_steps(GB *gb, int max_steps);
void gb_run_time(GB *gb, uint64_t max_time);

uint32_t gb_step(GB *gb);
void gb_tick(GB *gb, uint32_t cycles);
void gb_reset(GB *gb);


#endif