#ifndef OAM_H
#define OAM_H

#include <stdint.h>

typedef struct gb_s GB;




// Prototypes:

uint8_t oam_read(GB *gb, uint16_t addr);
void oam_write(GB *gb, uint16_t addr, uint8_t val);


#endif