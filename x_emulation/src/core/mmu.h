#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <stdio.h>
#include "../debug/trace_ids.h"


typedef struct gb_s GB;

// Map sizes:
#define M_MAP_size          0x10000         // Total memory Map Size (full range CPU as access to, THS IS NOT ROM SIZE)

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



#define MEM_TRACE_CAPACITY 100

typedef enum {
    MEM_TRACE_READ,
    MEM_TRACE_WRITE
} MemTraceType;

typedef struct {
    uint64_t step;
    uint16_t pc;
    uint16_t address;
    uint8_t value;
    uint8_t type;
    uint8_t bus;
} MemTraceEntry;

typedef struct {
    MemTraceEntry entries[MEM_TRACE_CAPACITY];
    uint32_t head;
    uint32_t count;
} MemTraceBuffer;


void mmu_init();
uint8_t mmu_read(GB *gb, uint16_t addr);
void mmu_write(GB *gb, uint16_t addr, uint8_t val);
void mmu_debugger(uint16_t addr);


#endif