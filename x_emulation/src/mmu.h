#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <stdio.h>
#include "trace_ids.h"


typedef struct gb_s GB;

// Map sizes:
#define M_MAP_size          0x10000         // Total memory Map Size (full range CPU as access to, THS IS NOT ROM SIZE)

#define OAM_size            0xA0            // (160)
#define io_rgstr_size       0x80            // (128)     // maybe don't use, as it's not normally an array


typedef uint8_t     (*mmu_read_func) (GB *gb, uint16_t addr);
typedef void        (*mmu_write_func)(GB *gb, uint16_t addr, uint8_t val);

typedef struct {
    uint16_t start;
    uint16_t end;
    mmu_read_func read;
    mmu_write_func write;
    bus_tag_t tag;
} MMU_MapRoute;

typedef struct {
    GB *gb;
    MMU_MapRoute map;
} MMU;


void mmu_init();
uint8_t mmu_read(GB *gb, uint16_t addr);
void mmu_write(GB *gb, uint16_t addr, uint8_t val);
void mmu_debugger(uint16_t addr);


#endif