//#include <stdio.h>  // for print / debugging stuff

#include "mbc.h"
#include "cart_types.h"
#include "../gb.h"


#include "../../debug/logger.h"

static void mbc_none_init_state(Cartridge *cart) {
    // nothing to do..
}

static int mbc_none_decode(Cartridge *cart, uint8_t type_code) {
    // Default MBC1 - turn off all features.
    cart->config.mbc_type = MBC_NONE;

    return 0;
}

uint8_t mbc_none_read(Cartridge *cart, uint16_t addr) {
    if (addr >= 0x0000 && addr <= 0x7FFF) {
        return cart->storage.rom_data[addr];
    }
    return 0xFF;
}

void mbc_none_write(Cartridge *cart, uint16_t addr, uint8_t write_val) {
    // No write intercept. Maybe possible RAM, but no switching or anything else.
    (void)addr;
    (void)write_val;
    return;
}

void mbc_none_write_ext(Cartridge *cart, uint16_t addr, uint8_t write_val) {
    /*
        Optionally up to 8 KiB of RAM could be connected at $A000-BFFF.
        Using a discrete logic decoder in place of a full MBC chip.
    */
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        if (cart->state.mbc_none.ram_enabled) {
            cart->storage.ram_data[addr] = write_val;
            logging_cart_mbc_log("[MBC-NONE] R EXT-R |A=%04X V=%02X|\n", addr, write_val);
            return;
        }
    }
    return;
}

uint8_t mbc_none_read_ext(Cartridge *cart, uint16_t addr){
    /*
        Optionally up to 8 KiB of RAM could be connected at $A000-BFFF.
        Using a discrete logic decoder in place of a full MBC chip.
    */

    // RAM bank size: 0x2000 // IF any.
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        if (cart->state.mbc_none.ram_enabled) {
            logging_cart_mbc_log("[MBC-NONE] R EXT-R |A=%04X|\n", addr);
            return cart->storage.ram_data[addr];
        }
    }
    return 0xFF;
}

// Bind Read/Write functions to MBC-None operations
const Operations mbc_none_ops = {
    .write = mbc_none_write,
    .read = mbc_none_read,
    .write_ext = mbc_none_write_ext,
    .read_ext = mbc_none_read_ext,
};

int mbc_none_init(GB *gb, Cartridge *cart, uint8_t type_code) {
    mbc_none_init_state(cart);
    // Bind mbc_none config:
    cart->ops = mbc_none_ops;

    printf(":MBC: [MBC-NONE Init]. Done.\n");
    return 0;
}



/*
*** MBC 1 *** - Implement Higher Priority.

MBC1 by Default supports:
    up to: 512 KiB ROM with..
    up to: 32 KiB of banked RAM

However, for 2MB Roms support:
    MBC1 will wire the RAM as extra ROM space to support 2MB
    But at the cost of ONLY 1 fixed 8 KB of cartridge RAM
Notice: Any Cartridge with 1MB+ of 'ROM" data, means it will have only 1 fixed bank of 8KB cart RAM.


Do note: there may be multi-rom carts. (Which I don't care about)

Also Note, the address range:
0000–7FFF is used both for reading from ROM and writing to the MBCs Control Registers

0000–3FFF — ROM Bank 00     | ReadOnly | First 16KiB of ROM (bank 00)
4000–7FFF — ROM Bank 01-7F  | ReadOnly |



*/
static void mbc1_init_state(Cartridge *cart) {
    uint8_t ram_enabled;
    uint8_t rom_bank_low5;
    uint8_t rom_bank_high2;
    uint8_t banking_mode;
    // uint16_t current_rom_bank;   // don't think I need?
    // uint8_t current_ram_bank;
    cart->state.mbc1.ram_enabled = 0;
    cart->state.mbc1.bank_low5 = 1; // Bank 1 is normally Default.
    cart->state.mbc1.bank_high2 = 0;
    cart->state.mbc1.banking_mode = 0;
}

static int mbc1_decode(Cartridge *cart, uint8_t type_code) {
    // Default MBC1 - turn off all features.
    cart->config.has_ram = 0;
    cart->config.has_battery = 0;
    cart->config.mbc_type = MBC1;

    // Note: $01 => is just base MBC, do not enable any features.
    switch (type_code) {
        case 0x01:
            // Set nothing, but do not return -1
            break;
        case 0x02:
            cart->config.has_ram = 1;
            break;
        case 0x03:
            cart->config.has_ram = 1;
            cart->config.has_battery = 1;
            break;
        default:
            return -1;
    }
    return 0;
}

void mbc1_write(Cartridge *cart, uint16_t addr, uint8_t write_val) {
    if (addr < 0x2000) {    // 0000-1FFF:: RAM enable (0x0A enables)
        cart->state.mbc1.ram_enabled = ((write_val & 0x0F) == 0x0A) ? 1 : 0;
        // if ((write_val & 0x0F) == 0x0A ) {
        //     logging_cart_mbc_log("[MBC1] W-INCPT :: Addr= %04X Write=%02X RAM Enabled.\n", addr, write_val);
        // }
        // if ((write_val & 0x0F) == 0x0F ) {
        //     logging_cart_mbc_log("[MBC1] W-INCPT :: Addr= %04X Write=%02X RAM DISABLED.\n", addr, write_val);
        // }
        return;
    }

    if (addr < 0x4000) {    // 2000-3FFF:: lower 5 bits of ROM bank number
        uint8_t low5 = write_val & 0x1F;
        if (low5 == 0) low5 = 1;           // bank 0 forbidden for switch area
        cart->state.mbc1.bank_low5 = low5;
        //logging_cart_mbc_log("[MBC1] W-INCPT :: Addr= %04X Write=%02X BANK-Low5=%02X\n", addr, write_val, cart->state.mbc1.bank_low5);
        return;
    }
    if (addr < 0x6000) {    // 4000-5FFF:: upper ROM bank bits (mode 0) OR RAM bank (mode 1)
        cart->state.mbc1.bank_high2 = write_val & 0x03;
        //logging_cart_mbc_log("[MBC1] W-INCPT :: Addr= %04X Write=%02X BANK-High2=%02X\n", addr, write_val, cart->state.mbc1.bank_high2);
        return;
    }
    if (addr < 0x8000) {    // 6000-7FFF:: banking mode select
        cart->state.mbc1.banking_mode = write_val & 0x01;
        logging_cart_mbc_log("[MBC1] W-INCPT :: Addr= %04X Write=%02X BankingMode Switch=%02X\n", addr, write_val, cart->state.mbc1.banking_mode);
        return;
    }
    // Didn't land anywhere correct. Return.
    return;
}

static inline uint32_t mbc1_rom_bank_0(const Cartridge *cart) {
    if (cart->state.mbc1.banking_mode == 0) {
        return 0;
    }
    uint32_t num_banks = (uint32_t)(cart->config.rom_size / 0x4000);
    if (num_banks == 0) return 0;

    uint32_t bank = ((uint32_t)(cart->state.mbc1.bank_high2 & 0x03) << 5);
    bank %= num_banks;
    return bank;
}

static inline uint32_t mbc1_rom_bank_x(const Cartridge *cart) {
    uint32_t bank = ((uint32_t)(cart->state.mbc1.bank_high2 & 0x03) << 5) |
                    (uint32_t)(cart->state.mbc1.bank_low5 & 0x1F);

    // "forbidden" bank numbers where lower 5 bits are 0 -> bump by 1
    if ((bank & 0x1F) == 0) {
        bank += 1;
    }

    // clamp by ROM size (number of 16KB banks)
    uint32_t num_banks = (uint32_t)(cart->config.rom_size / 0x4000);
    if (num_banks == 0) return 0;

    bank %= num_banks;
    return bank;
}

uint8_t mbc1_read(Cartridge *cart, uint16_t addr) {
    if (addr < 0x4000) {
        uint32_t bank = mbc1_rom_bank_0(cart);
        uint32_t offset = bank * 0x4000u + (uint32_t)addr;
        if (offset < cart->config.rom_size) return cart->storage.rom_data[offset];
        return 0xFF;
    }

    if (addr < 0x8000) {
        uint32_t bank = mbc1_rom_bank_x(cart);
        uint32_t offset = bank * 0x4000u + (uint32_t)(addr - 0x4000);
        if (offset < cart->config.rom_size) return cart->storage.rom_data[offset];
        return 0xFF;
    }
    return 0xFF;
}

void mbc1_write_ext(Cartridge *cart, uint16_t addr, uint8_t write_val) {
    // A000-BFFF: external RAM write (if present + enabled)
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        if (!cart->state.mbc1.ram_enabled) return;
        if (cart->storage.ram_data == NULL || cart->state.mbc1.calc_ram_size == 0) return;

        uint32_t ram_bank = 0;
        if (cart->state.mbc1.banking_mode == 1) {
            ram_bank = (uint32_t)(cart->state.mbc1.bank_high2 & 0x03);
            logging_cart_mbc_log("[MBC1] Addr= %04X Write=%02X RAM_BANK_MODE=%02X\n", addr, write_val, cart->state.mbc1.banking_mode);
        }

        uint32_t offset = ram_bank * 0x2000u + (uint32_t)(addr - 0xA000);
        if (offset < cart->state.mbc1.calc_ram_size) {
            cart->storage.ram_data[offset] = write_val;
            trace_general_write((0x00), addr, write_val, 4);
        }
        return;
    }
}

uint8_t mbc1_read_ext(Cartridge *cart, uint16_t addr){
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        if (!cart->state.mbc1.ram_enabled) {
            return 0xFF;
        }
        if (cart->storage.ram_data == NULL || cart->state.mbc1.calc_ram_size == 0) {
            return 0xFF;
        }

        uint32_t ram_bank = 0;
        if (cart->state.mbc1.banking_mode == 1) {
            ram_bank = (uint32_t)(cart->state.mbc1.bank_high2 & 0x03);
        }

        uint32_t offset = ram_bank * 0x2000u + (uint32_t)(addr - 0xA000);
        if (offset < cart->state.mbc1.calc_ram_size) {
            return cart->storage.ram_data[offset];
        }
        printf(":MBC: MBC1 Read-RAM WARNING. Addr Out of RANGE, Returning 0xFF. Details: RamBank=%u Ram_Offset=%u. CalcRamSize=%zu\n", ram_bank, offset, cart->state.mbc1.calc_ram_size);
        trace_general_read((0x00), addr, (0x00), 4);
        return 0xFF;
    }
    return 0xFF;
}

// Bind Read/Write functions to MBC1 operations
const Operations mbc1_ops = {
    .write = mbc1_write,
    .read = mbc1_read,
    .write_ext = mbc1_write_ext,
    .read_ext = mbc1_read_ext,
};

// Setup the ram data
void mbc1_init_ram(Cartridge *cart) {
    // If MBC1 ROM is 512KB. Then RAM will be => 32kb total. (8kb * 4 banks);
    if (cart->config.rom_size == (512 * 1024)) {
        cart->config.ram_bank_count = 4;
        cart->state.mbc1.calc_ram_size = (cart->config.ram_size * 4);
        printf("Notice, setting RAM size (8KB) * 4. As ROM is only 512KB\n");
    }

    size_t ram_size = cart->state.mbc1.calc_ram_size;   // Use the calculated RAM size.

    // Initialize Cartridge RAM data, setting all values to 0.
    uint8_t *ram_data = calloc(ram_size, sizeof(uint8_t));
    cart->storage.ram_data = ram_data;
}

int mbc1_init(GB *gb, Cartridge *cart, uint8_t type_code) {
    if (mbc1_decode(cart, type_code) != 0) {
        fprintf(stderr, ":MBC: [MBC1_SETUP] Error Decoding MBC\n");
        return -1;
    }
    // Note. Test rom I'm using is:
    // Cart_type: 0x03 => MBC1 with RAM and battery
    // ROM Size: 0x04  => 512 KB ROM
    // RAM Size: 0x02  => 8 KB RAM

    mbc1_init_state(cart);

    // Ram init:
    mbc1_init_ram(cart);

    // Bind mbc1 config:
    cart->ops = mbc1_ops;

    printf(":MBC: [MBC1 Init]. Done.\n");

    return 0;
}


/*
=== MBC2 ===    -- Unknown, likely lower priority.

0000–3FFF — ROM Bank 0      | read-only |       first 16kb of rom
4000–7FFF — ROM Bank $01-0F | read-only |       as MBC1, but only 16 ROM Banks.
A000–A1FF — Built-in RAM    | ?? |              built in ram
"technically" no ram. => max 256 KiB ROM and 512×4 bits RAM

*/

void mbc2 () {  // Unknown priority. Likely lower.

    /*
    MBC 2 Header codes:

    $05	MBC2
    $06	MBC2+BATTERY

    */
}
static void mbc2_init_state(Cartridge *cart) {
    cart->state.mbc2.ram_enabled = 0;
    cart->state.mbc2.current_rom_bank = 1;
    cart->state.mbc2.current_ram_bank = 0;
}

static int mbc2_decode(Cartridge *cart, uint8_t type_code) {
    // Default MBC2 - turn off all features.
    cart->config.has_battery = 0;
    cart->config.mbc_type = MBC2;

    // Note: $05 => is just base MBC-2, do not enable any features.
    switch (type_code) {
        case 0x05:
            // Set nothing, but do not return -1
            break;
        case 0x06:
            cart->config.has_battery = 1;
            break;
        default:
            return -1;
    }
    return 0;
}

void mbc2_write(Cartridge *cart, uint16_t addr, uint8_t val) {

}

uint8_t mbc2_read(Cartridge *cart, uint16_t addr) {
    if (addr <= 0x3FFF) {   // Fixed-Bank
        // printf(":MBC1: Read Matches ROM Bank 00 -> Fixed Bank Val: 0x%02X\n", cart->cartstorage.rom_data[addr]);
        return cart->storage.rom_data[addr];
    }
    if (addr <= 0x7FFF) {   // Switch-Bank
        uint8_t bank = cart->state.mbc2.current_rom_bank;
        uint32_t rom_offset =
        (bank * 0x4000) + (addr - 0x4000);
        // printf(":MBC3: Read Matches ROM Bank: 0x%02X -> Switch-Bank Val: 0x%02X\n", bank, cart->cartstorage.rom_data[rom_offset]);

        return cart->storage.rom_data[rom_offset];
    }
    return 0xFF;
}

void mbc2_write_ext(Cartridge *cart, uint16_t addr, uint8_t val) {

}

uint8_t mbc2_read_ext(Cartridge *cart, uint16_t addr){
    return 0xFF;
}

// Bind Read/Write functions to MBC2 operations
const Operations mbc2_ops = {
    .write = mbc2_write,
    .read = mbc2_read,
    .write_ext = mbc2_write_ext,
    .read_ext = mbc2_read_ext,
};

int mbc2_init(GB *gb, Cartridge *cart, uint8_t type_code) {
    if (mbc2_decode(cart, type_code) != 0) {
        fprintf(stderr, "MBC_SETUP: [MBC2_DECODE] Error Decoding MBC\n");
        return -1;
    }

    mbc2_init_state(cart);
    // Bind mbc2 config:
    cart->ops = mbc2_ops;

    printf(":MBC: [MBC2 Init]. Done.\n");

    return 0;
}





/*
*** MBC 3 *** - Implement Higher Priority.

Beside for the ability to access up to:
2MB ROM (128 banks), and 32KB RAM (4 banks).
The MBC3 also includes a built-in Real Time Clock (RTC).

Notice, Edge case: MBC3 with 64 KiB of SRAM refers to MBC30, used only in [ 1 ] Japanese title.

Address space:
--------------------------------------------------
0000-3FFF - ROM Bank 00             | Read Only  |       Contains the first 16 KiB of the ROM.
4000-7FFF - ROM Bank 01-7F          | Read Only  |       Same as for MBC1, except that accessing banks $20, $40, and $60 is supported now.
--------------------------------------------------
A000-BFFF - REGISTERS (BELOW)       | Read/Write |       Ram Bank, plus the RTC Register (real time clock)


Registers: (Write intercepts probably)
A000-BFFF - RTC / RAM Register      | Read/Write |      Ram Bank, plus the RTC Register (real time clock)
--------------------------------------------------
0000-1FFF - RAM and Timer Enable    | Write Only |      Value of $0A will enable both | value of $00 will disable both
2000-3FFF - ROM Bank Number         | Write Only |
4000-5FFF - RAM BANK / RTC Select   | Write Only |      Selects either: $00-$07 Ram-Bank | $08-$0C RTC Register
6000-7FFF - Latch Clock Data        | Write Only |      Writing $00, then $01 to register current time will latch to RTC

--------------------------------------------------
--------------------------------------------------
MBC3 Header codes:

$0F	MBC3+TIMER+BATTERY
$10	MBC3+TIMER+RAM+BATTERY
$11	MBC3
$12	MBC3+RAM
$13	MBC3+RAM+BATTERY
*/

/*
The simplest rule set for MBC3

Here is the stripped-down version you should keep in your head:

Writes
0000-1FFF: enable RAM/RTC if low nibble is 0xA
2000-3FFF: select ROM bank, 0 becomes 1
4000-5FFF: select RAM bank 0-3 or RTC register 0x08-0x0C
6000-7FFF: RTC latch

Reads
0000-3FFF: fixed ROM bank 0
4000-7FFF: switchable ROM bank
A000-BFFF: RAM bank or RTC register, depending on current selection

That is MBC3 in its practical form.
*/

static void mbc3_init_state(Cartridge *cart) {
    cart->state.mbc3.ram_rtc_enabled = 0;
    cart->state.mbc3.current_rom_bank = 1;
    cart->state.mbc3.current_ram_bank = 0;
    cart->state.mbc3.rtc_reg_select = 0;
    cart->state.mbc3.ram_bank_mode = 0;
    cart->state.mbc3.rtc_latch_armed = 0;
}

static int mbc3_decode(Cartridge *cart, uint8_t type_code) {
    // Default MBC3 - turn off all features.
    cart->config.has_ram = 0;
    cart->config.has_battery = 0;
    cart->config.has_rtc = 0;
    cart->config.mbc_type = MBC3;

    // Note: $11 => is just base MBC, do not enable any features.
    switch (type_code) {
        case 0x0F:
            cart->config.has_rtc = 1;
            cart->config.has_battery = 1;
            break;
        case 0x10:
            cart->config.has_rtc = 1;
            cart->config.has_ram = 1;
            cart->config.has_battery = 1;
            break;
        case 0x11:
            // Set nothing, but do not return -1
            break;
        case 0x12:
            cart->config.has_ram = 1;
            break;
        case 0x13:
            cart->config.has_ram = 1;
            cart->config.has_battery = 1;
            break;
        default:
            return -1;
    }
    return 0;
}

void mbc3_write(Cartridge *cart, uint16_t addr, uint8_t val) {
    if (addr <= 0x1FFF) {
        if (cart->config.has_ram || cart->config.has_rtc) {
            /* RAM / RTC enable/disable */
            cart->state.mbc3.ram_rtc_enabled = ((val & 0x0F) == 0x0A) ? 1 : 0;
            if ((val & 0x0F) == 0x0A) {
                logging_cart_mbc_log("[MBC3] Addr= %04X Write=%02X RTC/RAM Enabled.\n", addr, val);
            }
            if ((val & 0x0F) == 0x00) {
                logging_cart_mbc_log("[MBC3] Addr= %04X Write=%02X RTC/RAM Disabled.\n", addr, val);
            }
            return;
        }
        return;
    }
    if (addr <= 0x3FFF) {
        /* ROM bank select */
        uint8_t bank = val & 0x7F;
        if (bank >= cart->config.rom_bank_count) {
            bank %= cart->config.rom_bank_count;
            if (bank == 0) { bank = 1; }
        }

        if (cart->config.rom_bank_count == 0) {
            printf("cart: ERROR rom_bank_count == 0\n");
            return;
        }

        // Clamp the bank to the total rom bank count.
        if (cart->state.mbc3.current_rom_bank != bank) {
            logging_cart_mbc_log("[MBC3] Addr= %04X Write=%02X RomBank_From=%02X RomBank_To=%02X\n", addr, val, cart->state.mbc3.current_rom_bank, bank);
            // Add the bank switch to the CPU trace log. So I know when it happens
            logging_cpu_trace("[MBC3] Addr= %04X Write=%02X RomBank_From=%02X RomBank_To=%02X\n", addr, val, cart->state.mbc3.current_rom_bank, bank);
        }

        cart->state.mbc3.current_rom_bank = bank;
        //printf("cart: [MBC3-WriteIntercept] ROM Bank Switch: %02X\n", bank);
        return;
    }

    if (addr <= 0x5FFF) {
        /* RAM bank select or RTC register select */
        if (val >= 0x00 && val <= 0x03) {  // Despite this space allowing 0x00 to 0x07. MBC3 only supports 4 banks (0x03)
            cart->state.mbc3.current_ram_bank = val;
            cart->state.mbc3.ram_bank_mode = 0;
            logging_cart_mbc_log("[MBC3] Addr= %04X Write=%02X RAM_BANK_MODE=%02X\n", addr, val, cart->state.mbc3.ram_bank_mode);

        } else if (val >= 0x08 && val <= 0x0C) {
            cart->state.mbc3.rtc_reg_select = val;
            cart->state.mbc3.ram_bank_mode = 1; // mode 1 = RTC read mode.
            logging_cart_mbc_log("[MBC3] Addr= %04X Write=%02X RAM_BANK_MODE=%02X\n", addr, val, cart->state.mbc3.ram_bank_mode);
        }
        return;
    }

    if (addr <= 0x7FFF) {
        /* RTC latch */
        if (cart->state.mbc3.rtc_latch_armed == 0 && val == 0x00) {
            cart->state.mbc3.rtc_latch_armed = 1;
        } else if (cart->state.mbc3.rtc_latch_armed == 1 && val == 0x01) {
            /* latch rtc snapshot */
            cart->state.mbc3.rtc_latch_armed = 0;
        } else {
            cart->state.mbc3.rtc_latch_armed = 0;
        }

        return;
    }
}

uint8_t mbc3_read(Cartridge *cart, uint16_t addr) {
    if (addr <= 0x3FFF) {   // Fixed-Bank
        return cart->storage.rom_data[addr];
    }
    if (addr <= 0x7FFF) {   // Switch-Bank
        uint8_t bank = cart->state.mbc3.current_rom_bank;
        uint32_t rom_offset =
        (bank * 0x4000) + (addr - 0x4000);

        return cart->storage.rom_data[rom_offset];
    }
    return 0xFF;
}

void mbc3_write_ext(Cartridge *cart, uint16_t addr, uint8_t write_val) {
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        if (cart->state.mbc3.ram_rtc_enabled) {
            if (cart->state.mbc3.ram_bank_mode == 0) {  // Write RAM
                uint8_t bank = cart->state.mbc3.current_ram_bank;
                size_t b_offset = (bank * 0x2000)+(addr - 0x2000);

                uint8_t old_val = cart->storage.ram_data[b_offset];

                cart->storage.ram_data[b_offset] = write_val;
                logging_cart_mbc_log("[MBC3] R EXT-R |A=%04X O_V=%02X W_V=%02X| B=%02X\n", addr, old_val, write_val, bank);

                return;
            }
            if (cart->state.mbc3.ram_bank_mode == 1) {  // Write RTC Registers

                return;
            }
        }
    }
    return;
}

uint8_t mbc3_read_ext(Cartridge *cart, uint16_t addr){
    // RAM bank size: 0x2000
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        if (cart->state.mbc3.ram_rtc_enabled) {
            if (cart->state.mbc3.ram_bank_mode == 0) {  // Read RAM
                uint8_t bank = cart->state.mbc3.current_ram_bank;
                size_t b_offset = (bank * 0x2000)+(addr - 0x2000);

                uint8_t val = cart->storage.ram_data[b_offset];

                logging_cart_mbc_log("[MBC3] R EXT-R |A=%04X V=%02X| B=%02X\n", addr, val, bank);
                return cart->storage.ram_data[b_offset];
            }
            if (cart->state.mbc3.ram_bank_mode == 1) {  // Read RTC Registers
                /*
                    $08	RTC S	Seconds	0-59 ($00-$3B)
                    $09	RTC M	Minutes	0-59 ($00-$3B)
                    $0A	RTC H	Hours	0-23 ($00-$17)
                    $0B	RTC DL	Lower 8 bits of Day Counter	($00-$FF)
                    $0C	RTC DH	Upper 1 bit of Day Counter, Carry Bit, Halt Flag.
                    Bit 0: Most significant bit (Bit 8) of Day Counter
                    Bit 6: Halt (0=Active, 1=Stop Timer)
                    Bit 7: Day Counter Carry Bit (1=Counter Overflow)
                */
                //rtc_reg_select
                return 0xFF;
            }
        }
    }
    return 0xFF;
}

// Bind Read/Write functions to MBC3 operations
const Operations mbc3_ops = {
    .write = mbc3_write,
    .read = mbc3_read,
    .write_ext = mbc3_write_ext,
    .read_ext = mbc3_read_ext,
};

// Setup the ram data
void mbc3_init_ram(Cartridge *cart) {
    size_t ram_size = cart->config.ram_size;    // MBC3 RAM size should be: 32kb total. (8kb * 4 banks);

    // Initialize Cartridge RAM data, setting all values to 0.
    uint8_t *ram_data = calloc(ram_size, sizeof(uint8_t));
    cart->storage.ram_data = ram_data;
}

int mbc3_init(GB *gb, Cartridge *cart, uint8_t type_code) {
    if (mbc3_decode(cart, type_code) != 0) {
        fprintf(stderr, "MBC_SETUP: [MBC3_DECODE] Error Decoding MBC\n");
        return -1;
    }

    mbc3_init_state(cart);
    // MBC3 RAM:
    mbc3_init_ram(cart);
    // Bind mbc3 config:
    cart->ops = mbc3_ops;

    printf(":MBC: [MBC3 Init]. Done.\n");

    return 0;
}

/// MBC5:

/*

*** MBC 5 *** - Implement Higher Priority.

*/

void mbc5(Cartridge *cart) {
    /*
    MBC5 Header codes:
    $19	MBC5
    $1A	MBC5+RAM
    $1B	MBC5+RAM+BATTERY
    $1C	MBC5+RUMBLE
    $1D	MBC5+RUMBLE+RAM
    $1E	MBC5+RUMBLE+RAM+BATTERY
    */

}

static void mbc5_init_state(Cartridge *cart) {
    cart->state.mbc5.ram_enabled = 0;
    cart->state.mbc5.rumble_enable = 0;
    cart->state.mbc5.current_rom_bank = 1;
    cart->state.mbc5.current_ram_bank = 0;
}

static int mbc5_decode(Cartridge *cart, uint8_t type_code) {
    // Default MBC5 - turn off all features.
    cart->config.has_ram = 0;
    cart->config.has_rumble = 0;
    cart->config.has_battery = 0;
    cart->config.mbc_type = MBC5;

    // Note: $19 => is just base MBC, do not enable any features.
    switch (type_code) {
        case 0x19:
            // Set nothing, but do not return -1
            break;
        case 0x1A:
            cart->config.has_ram = 1;
            break;
        case 0x1B:
            cart->config.has_ram = 1;
            cart->config.has_battery = 1;
            break;
        case 0x1C:
            cart->config.has_rumble = 1;
            break;
        case 0x1D:
            cart->config.has_rumble = 1;
            cart->config.has_ram = 1;
            break;
        case 0x1E:
            cart->config.has_rumble = 1;
            cart->config.has_ram = 1;
            cart->config.has_battery = 1;
            break;
        default:
            return -1;
    }
    return 0;
}

void mbc5_write(Cartridge *cart, uint16_t addr, uint8_t val) {

}

uint8_t mbc5_read(Cartridge *cart, uint16_t addr) {
    return 0xFF;
}

void mbc5_write_ext(Cartridge *cart, uint16_t addr, uint8_t val) {

}

uint8_t mbc5_read_ext(Cartridge *cart, uint16_t addr){
    return 0xFF;
}

// Bind Read/Write functions to MBC5 operations
const Operations mbc5_ops = {
    .write = mbc5_write,
    .read = mbc5_read,
    .write_ext = mbc5_write_ext,
    .read_ext = mbc5_read_ext,
};

int mbc5_init(GB *gb, Cartridge *cart, uint8_t type_code) {
    if (mbc5_decode(cart, type_code) != 0) {
        fprintf(stderr, ":MBC: [MBC5 Init] Error Decoding MBC\n");
        return -1;
    }

    mbc5_init_state(cart);
    // Bind mbc5 config:
    cart->ops = mbc5_ops;

    printf(":MBC: [MBC5 Init]. Done.\n");

    return 0;
}




// Both mbc6 and mbc7 I think are rare. Lower Priority.
void mbc6(Cartridge *cart) {
    // MBC6 Header codes:
    // $20	MBC6
}

void mbc7(Cartridge * cart) {
    // MBC6 Header codes:
    // $22	MBC7+SENSOR+RUMBLE+RAM+BATTERY
}


void mbc_other(Cartridge *cart) {
    // Donno, maybe a default value.

}




int mbc_init(GB *gb, Cartridge *cart, uint8_t type_code) {
    switch (type_code) {
        case 0x00:
            cart->config.mbc_type = MBC_NONE;
            return mbc_none_init(gb, cart, type_code);
        case 0x01 ... 0x03:
            cart->config.mbc_type = MBC1;
            return mbc1_init(gb, cart, type_code);
        case 0x5 ... 0x6:
            cart->config.mbc_type = MBC2;
            return mbc2_init(gb, cart, type_code);
        case 0x0F ... 0x15:
            cart->config.mbc_type = MBC3;
            return mbc3_init(gb, cart, type_code);
        case 0x19 ... 0x1E:
            cart->config.mbc_type = MBC5;
            return mbc5_init(gb, cart, type_code);
        default:
            return -1;
    }
    return 0;
}