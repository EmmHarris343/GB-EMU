#ifndef MMU_INTERFACE_H
#define MMU_INTERFACE_H

#include <stdint.h>
#include <stdio.h>

typedef uint8_t     (*mmu_read_func) (uint16_t addr);
typedef void        (*mmu_write_func)(uint16_t addr, uint8_t val);

typedef struct {
    uint16_t start;
    uint16_t end;
    mmu_read_func read;
    mmu_write_func write;
} mmu_map_entry;

void mmu_init(mmu_map_entry *map, int num_entries);
uint8_t mmu_read(uint16_t addr);
void mmu_write(uint16_t addr, uint8_t val);

#endif