//#include <stdio.h>  // for print / debugging stuff

#include "mbc.h"
#include "cart_types.h"
#include "gb.h"


#include "logger.h"

int mbc_none_setup(GB *gb, Cartridge *cart) {
    return 0;
}

void mbc_none(Cartridge *cart) {

}


/*
*** MBC 1 *** - Implement Higher Priority.

----> NOTE: This one is a little complicated. I think I'm holding off on it for now.


MBC1 supports up to 512 KiB ROM with up to 32 KiB of banked RAM
To support up to 2 MiB ROM, the MBC1 RAM is wired to support 2MiB rom
At the cost of only supporting a fixed 8 KiB of cartridge RAM

Do note: There is multi-game cartriges. ----> This is not something I personally care about at this time.

Also Note, the address range:
0000–7FFF is used both for reading from ROM and writing to the MBCs Control Registers

0000–3FFF — ROM Bank 00     | ReadOnly | First 16KiB of ROM (bank 00)
4000–7FFF — ROM Bank 01-7F  | ReadOnly |

Note for bank reading: 01-7F
If the main 5-bit ROM banking register is 0, it reads the bank as if it was set to 1.
For 1 MiB+ ROM

*/
static void mbc1_state_init(Cartridge *cart) {
    cart->state.mbc1.ram_enabled = 0;
    cart->state.mbc1.current_rom_bank = 1;
    cart->state.mbc1.current_ram_bank = 0;
    cart->state.mbc1.mode = 0;
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

void mbc1_write(Cartridge *cart, uint16_t addr, uint8_t val) {

}

uint8_t mbc1_read(Cartridge *cart, uint16_t addr) {
    return 0xFF;
}

void mbc1_write_ext(Cartridge *cart, uint16_t addr, uint8_t val) {

}

uint8_t mbc1_read_ext(Cartridge *cart, uint16_t addr){
    return 0xFF;
}

const Operations mbc1_ops = {
    .write = mbc1_write,
    .read = mbc1_read,
    .write_ext = mbc1_write_ext,
    .read_ext = mbc1_read_ext,
};

int mbc1_setup(GB *gb, Cartridge *cart, uint8_t type_code) {
    if (mbc1_decode(cart, type_code) != 0) {
        fprintf(stderr, "MBC_SETUP: [MBC1_DECODE] Error Decoding MBC\n");
        return -1;
    }

    mbc1_state_init(cart);
    // Bind mbc1 config:
    cart->ops = mbc1_ops;

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
static void mbc2_state_init(Cartridge *cart) {
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
    return 0xFF;
}

void mbc2_write_ext(Cartridge *cart, uint16_t addr, uint8_t val) {

}

uint8_t mbc2_read_ext(Cartridge *cart, uint16_t addr){
    return 0xFF;
}

const Operations mbc2_ops = {
    .write = mbc2_write,
    .read = mbc2_read,
    .write_ext = mbc2_write_ext,
    .read_ext = mbc2_read_ext,
};

int mbc2_setup(GB *gb, Cartridge *cart, uint8_t type_code) {
    if (mbc2_decode(cart, type_code) != 0) {
        fprintf(stderr, "MBC_SETUP: [MBC2_DECODE] Error Decoding MBC\n");
        return -1;
    }

    mbc2_state_init(cart);
    // Bind mbc2 config:
    cart->ops = mbc2_ops;

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

static void mbc3_state_init(Cartridge *cart) {
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
        //printf("cart: [MBC3-WriteIntercept] RAM Bank / RTC Register Select: %02X\n", val);
        return;
    }

    if (addr <= 0x7FFF) {
        /* RTC latch */
        if (cart->state.mbc3.rtc_latch_armed == 0 && val == 0x00) {
            cart->state.mbc3.rtc_latch_armed = 1;
            // printf("cart: [MBC3-WriteIntercept] Set RTC - Latch armed=1\n");
            //logging_cart_mbc_log("[MBC3] Addr= %02X Write=%02X RTC_LATCHED=%02X\n", addr, val, cart->state.mbc3.rtc_latch_armed);
        } else if (cart->state.mbc3.rtc_latch_armed == 1 && val == 0x01) {
            /* latch rtc snapshot */
            // printf("cart: [MBC3-WriteIntercept] Set RTC snapshot - Latch armed=0\n");
            //logging_cart_mbc_log("[MBC3] Addr= %02X Write=%02X RTC UnLATCHED=%02X\n", addr, val, cart->state.mbc3.rtc_latch_armed);
            cart->state.mbc3.rtc_latch_armed = 0;
        } else {
            // printf("cart: [MBC3-WriteIntercept] Reset RTC - Latch armed=0\n");
            //logging_cart_mbc_log("[MBC3] Addr= %02X Write=%02X RTC UnLATCHED=%02X\n", addr, val, cart->state.mbc3.rtc_latch_armed);
            cart->state.mbc3.rtc_latch_armed = 0;
        }

        return;
    }}

uint8_t mbc3_read(Cartridge *cart, uint16_t addr) {
    if (addr <= 0x3FFF) {   // Fixed-Bank
        // printf(":MBC3: Read Matches ROM Bank 00 -> Fixed Bank Val: 0x%02X\n", cart->cartstorage.rom_data[addr]);
        return cart->storage.rom_data[addr];
    }
    if (addr <= 0x7FFF) {   // Switch-Bank
        uint8_t bank = cart->state.mbc3.current_rom_bank;
        uint32_t rom_offset =
        (bank * 0x4000) + (addr - 0x4000);
        // printf(":MBC3: Read Matches ROM Bank: 0x%02X -> Switch-Bank Val: 0x%02X\n", bank, cart->cartstorage.rom_data[rom_offset]);

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

// The operation function routing for the MBC3:
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

int mbc3_setup(GB *gb, Cartridge *cart, uint8_t type_code) {
    if (mbc3_decode(cart, type_code) != 0) {
        fprintf(stderr, "MBC_SETUP: [MBC3_DECODE] Error Decoding MBC\n");
        return -1;
    }

    mbc3_state_init(cart);
    mbc3_init_ram(cart);
    // Bind mbc3 config:
    cart->ops = mbc3_ops;

    return 0;
}


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

static void mbc5_state_init(Cartridge *cart) {
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

const Operations mbc5_ops = {
    .write = mbc5_write,
    .read = mbc5_read,
    .write_ext = mbc5_write_ext,
    .read_ext = mbc5_read_ext,
};

int mbc5_setup(GB *gb, Cartridge *cart, uint8_t type_code) {
    if (mbc5_decode(cart, type_code) != 0) {
        fprintf(stderr, "MBC_SETUP: [MBC5_DECODE] Error Decoding MBC\n");
        return -1;
    }

    mbc5_state_init(cart);
    // Bind mbc5 config:
    cart->ops = mbc5_ops;

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




int mbc_setup(GB *gb, Cartridge *cart, uint8_t type_code) {
    switch (type_code) {
        case 0x00:
            cart->config.mbc_type = MBC_NONE;
            return mbc_none_setup(gb, cart);
        case 0x01 ... 0x03:
            cart->config.mbc_type = MBC1;
            return mbc1_setup(gb, cart, type_code);
        case 0x5 ... 0x6:
            cart->config.mbc_type = MBC2;
            return mbc2_setup(gb, cart, type_code);
        case 0x0F ... 0x15:
            cart->config.mbc_type = MBC3;
            return mbc3_setup(gb, cart, type_code);
        case 0x19 ... 0x1E:
            cart->config.mbc_type = MBC5;
            return mbc5_setup(gb, cart, type_code);
        default:
            return -1;
    }
    return 0;
}