#include <stdint.h>

typedef void (*mbc_write_func)(uint16_t addr, uint8_t val);
typedef void (*mbc_read_func)(uint16_t addr);



typedef struct {
    uint8_t mbc_type;           // 0 = none, 1 = MBC1, 2 = MBC2, etc.
    uint8_t has_battery;        // 1 if battery-backed RAM
    uint8_t has_ram;            // 1 if external RAM exists
    uint8_t has_rtc;            // For MBC3 Real-Time Clock
    uint32_t rom_size;          // in bytes
    uint32_t ram_size;          // in bytes
    uint32_t rom_banks;         // Total of rom banks
} Cart_Config;


typedef struct {
    uint8_t entry_point[3];
    uint8_t logo[47];
    uint8_t title[16];       // +1 cause string.. I guess. --- Depending on rom not used at all, and non populated.
    uint8_t cart_type_code;
    uint8_t rom_size_code;
    uint8_t ram_size_code;
    uint8_t chksm;
    uint8_t gbl_chksm[2];    // Technically 16 bit....
    uint8_t cgb_f;
    uint8_t sgb_f;
    uint8_t new_lic[2];      // Only used if old licence = $33
    uint8_t old_lic;
} Header;



typedef struct {
    Cart_Config config;
    Header header_config;
    mbc_write_func mbc_write;
    mbc_read_func mbc_read;
} Cartridge;




// Functions inside cart.c (Make it accessable elsewhere)

void parse_cart_header(const char *filename, Cartridge *cart);
void set_cart_features(Cartridge *cart);







// Cartridge *cart 
// cart->config.mbc_type;


