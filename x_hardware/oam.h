#ifndef OAM_H
#define OAM_H

#include <stdint.h>





// Prototypes:

uint8_t oam_read(uint16_t addr);
void oam_write(uint16_t addr, uint8_t val);


#endif