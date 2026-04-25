#ifndef OAM_H
#define OAM_H

#include <stdint.h>

typedef struct gb_s GB;

typedef struct {
    uint8_t y;
    uint8_t x;
    uint8_t tile_id;
    uint8_t flags;
} OAMSprite;

// 40 Sprites *4 bytes each = 160 Bytes


// Prototypes:

uint8_t oam_read(GB *gb, uint16_t addr);
void oam_write(GB *gb, uint16_t addr, uint8_t val);


#endif