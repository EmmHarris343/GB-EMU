#ifndef CART_TYPES
#define CART_TYPES

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct gb_s GB;


typedef enum {
    MBC_NONE = 0,
    MBC1,
    MBC2,
    MBC3,
    MBC5
} MbcType;


// The MBC States:
// MBC None
typedef struct {
    uint8_t ram_enabled;    // Not really sure if this is needed but... Oh well.
} MBC_NONE_state;
// MBC1 state:
typedef struct {
    uint8_t ram_enabled;
    size_t calc_ram_size;
    uint8_t bank_low5;
    uint8_t bank_high2;
    uint8_t banking_mode;
    uint16_t current_rom_bank;
    uint8_t current_ram_bank;
} MBC1_state;

// MBC2 state:
typedef struct {
    uint8_t ram_enabled;
    uint16_t current_rom_bank;
    uint8_t current_ram_bank;
} MBC2_state;

// MBC3 state:
typedef struct {
    uint8_t ram_rtc_enabled;
    uint8_t current_rom_bank;   // mbc3 will only need 7bits => fits in uint8_t
    uint8_t current_ram_bank;
    uint8_t rtc_reg_select;
    uint8_t ram_bank_mode;      /* 0 = RAM bank selected, 1 = RTC register selected */
    uint8_t rtc_latch_armed;
} MBC3_state;

// MBC5 state:
typedef struct {
    uint8_t ram_enabled;
    uint8_t rumble_enable;
    uint16_t current_rom_bank;  // MBC5 will need 9bits => needs uint16_t
    uint8_t current_ram_bank;
} MBC5_state;


typedef struct {
    MBC_NONE_state mbc_none;
    MBC1_state mbc1;
    MBC2_state mbc2;
    MBC3_state mbc3;
    MBC5_state mbc5;
} MBC_State;


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
} Headers;

typedef struct {
    uint8_t mbc_type;           // 0 = none, 1 = MBC1, 2 = MBC2, etc.
    uint8_t has_battery;
    uint8_t has_ram;
    uint8_t has_rtc;            // For MBC3 Real-Time Clock
    uint8_t has_rumble;         // Only certain MBCs have rumble inside the cartridge.
    uint8_t has_rom_banking;    // Some MBC has weird ROM Banking, deciding it's worth while to know.
    uint8_t has_ram_banking;
    size_t rom_size;            // Size of Entire ROM
    size_t ram_size;
    uint8_t rom_bank_count;     // Total of rom banks
    size_t ram_bank_count;      // RAM (External Memory Banks)
} Config;

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day_low;
    uint8_t day_high;
    uint8_t latched_seconds;
    uint8_t latched_minutes;
    uint8_t latched_hours;
    uint8_t latched_day_low;
    uint8_t latched_day_high;
} RTC;

typedef struct {
    uint8_t *rom_data;              // Actual Full ROM data
    uint8_t *ram_data;

    RTC rtc;
} Storage;

// Declare cartridge so it can be included, before it is fully declared later.
typedef struct Cartridge Cartridge;

typedef void (*mbc_write_func)(struct Cartridge *cart, uint16_t addr, uint8_t val);
typedef uint8_t (*mbc_read_func)(struct Cartridge *cart, uint16_t addr);

typedef struct {
    mbc_write_func write;
    mbc_read_func read;
    mbc_write_func write_ext;
    mbc_read_func read_ext;
} Operations;

// Now declare what is inside the Cartridge typedef/struct ..
struct Cartridge{
    Config config;
    Storage storage;
    MBC_State state;
    Operations ops;
};

#endif