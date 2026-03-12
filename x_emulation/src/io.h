#ifndef IO_H
#define IO_H

#include <stdint.h>

typedef struct gb_s GB;

typedef uint8_t (*io_map_read) (GB *gb, uint16_t addr);
typedef void    (*io_map_write)(GB *gb, uint16_t addr, uint8_t val);

// Name the IO_BUS routes.
typedef enum {
    IO_INPUT,
    IO_SERIAL,
    IO_TIMER,
    IO_INTERUPTS,
    IO_PPU,
    IO_DMA,
    IO_MAX
} io_subsystem_tag;    // BUS tag names.

typedef struct {
    uint16_t start;
    uint16_t end;
    io_map_read read;
    io_map_write write;
    io_subsystem_tag tag;
} IO_RegMap;

typedef struct {
    GB *gb;
    IO_RegMap map;
} IO;

uint8_t io_read(GB *gb, uint16_t addr);
void io_write(GB *gb, uint16_t addr, uint8_t val);

#endif