// This feels a little silly this living here. but.... w/e

typedef enum {
    BUS_ROM,    // cart 0000-7FFF
    BUS_ECRAM,   // cart Ram A000-BFFF
    BUS_VRAM,
    BUS_WRAM,
    BUS_HRAM,
    BUS_ECHO,
    BUS_OAM,
    BUS_IO,
    BUS_IE,
    BUS_UNMAPPED,
    BUS_MAX
} bus_tag_t;    // BUS tag names.


// Add a Trace record for what service emitted the Trace? IE: CPU =0, MMU =1, CART =1.. etc