#include "string.h"

#include "cart.h"
#include "cart_types.h"
#include "mbc.h"
#include "../gb.h"


Headers headers;

/// TODO: WRITE INTERCEPTS:
/*

Write Table: (Intercepting)
    ---------------------------------------------------------------------------------------
    0000 - 3FFF => FIXED BANK SPACE
    ---------------------------------------------------------------------------------------
    0000 - 3FFF (FIXED BANK)    => READ         ACTION: Returns Data in the FIXED BANK ROM.
    0000 - 1FFF (Fixed bank)    => Write        ACTION: Enable/ Disable RAM
    2000 - 3FFF (Fixed Bank)    => Write        ACTION: ROM Bank Switch

    ---------------------------------------------------------------------------------------
    4000 - 7FFF => SWITCH BANK SPACE
    ---------------------------------------------------------------------------------------
    4000 - 7FFF (FIXED BANK)    => READ         ACTION: Returns Data in the currently Selected, Switchable ROM BANK
    4000 - 5FFF (Switch BANK)   => Write        ACTION: RAM Bank Switch
    6000 - 7FFF (SWITCH BANK)   => Write        ACTIOH: ROM/RAM Mode 0/1 SWITCH

    ---------------------------------------------------------------------------------------
    A000 - BFFF => EXTERNAL RAM
    A000 - BFFF (EXT RAM)       => READ/ WRITE  ACTION: Reads from External RAM / Writes to External RAM
*/

// Static constant:
static const uint32_t rom_size_lookup[0x55] = {
    [0x00] = 32 * 1024,
    [0x01] = 64 * 1024,
    [0x02] = 128 * 1024,
    [0x03] = 256 * 1024,
    [0x04] = 512 * 1024,
    [0x05] = 1024 * 1024,
    [0x06] = 2 * (1024 * 1024),
    [0x07] = 4 * (1024 * 1024),
    [0x08] = 8 * (1024 * 1024),
    [0x52] = 72 * 16 * 1024,  // Wonky 72 banks => 1.1 MB
    [0x53] = 80 * 16 * 1024,  // Wonky 80 banks => 1.2 MB
    [0x54] = 96 * 16 * 1024   // Wonky 96 banks => 1.5 MB
};

static const uint16_t rom_bank_lookup[0x55] = {
    2, 4, 8, 16, 32, 64, 128, 256, 512,
    [0x52] = 72, [0x53] = 80, [0x54] = 96 // Special $52 - $54 Rom Size codes:
};

// Some re-usable functions:
static uint32_t rom_size_by_code(uint8_t rom_size_code) {
    return rom_size_lookup[rom_size_code];
}

static uint16_t rom_bank_by_code(uint8_t ram_bank_code) {
    return rom_bank_lookup[ram_bank_code];
}

static size_t ram_size_by_code(uint8_t ram_code) {
    static size_t ram_size = 0;
    switch (headers.ram_size_code) {
        case 0x00: ram_size = 0; break;
        case 0x01: ram_size = 2 * 1024; break;
        case 0x02: ram_size = 8 * 1024; break;
        case 0x03: ram_size = 32 * 1024; break;
        case 0x04: ram_size = 128 * 1024; break;
        case 0x05: ram_size = 64 * 1024; break;
        default: ram_size = 0; break;
    }
    return ram_size;
}


/// NOTICE: This is a FAKE cartridge. For use when ONLY testing!
int init_cart_test_mode(GB *gb) {
    printf("Starting Mock Rom/ Cartridge Configuration.. \n");


    /// NOTICE: These are hard coded values. Intended strictly for testing

    headers.cart_type_code = 0x03;          // MBC (Zda MBC1 = 0x02 - 0x03)
    headers.rom_size_code = 0x04;           // Rom (Zda uses 0x04)
    headers.ram_size_code = 0x02;           // Ram (Memory size)

    // ROM Size / Bank count
    gb->cart.config.rom_size = rom_size_by_code(headers.rom_size_code);
    gb->cart.config.rom_bank_count = rom_bank_by_code(headers.rom_size_code);

    // RAM Size:
    gb->cart.config.ram_size = ram_size_by_code(headers.ram_size_code);

    // Some Other Config Settings.
    gb->cart.config.mbc_type = 1;
    gb->cart.config.has_rom_banking = 1;
    gb->cart.config.has_ram = 1;
    gb->cart.config.has_ram_banking = 1;
    gb->cart.config.has_battery = 1;

    // Some default Startup Settings for Rom.
    if (gb->cart.config.has_ram == 1) {
        gb->cart.state.mbc3.ram_rtc_enabled = 1;
    }
    if (gb->cart.config.has_ram_banking) {
        gb->cart.state.mbc3.current_ram_bank = 0x00;
    }
    if (gb->cart.config.has_rom_banking == 1) {
        gb->cart.state.mbc3.current_rom_bank = 1;           // Defaulting to bank 1 (WARNING! this behaviour is not consistant in all MBC!)
        //gb->cart.state.mbc3.fixed_b_addr = 0x00;
        //gb->cart.state.mbc3.calcd_switch_addr = (cart.state.mbc3.current_rom_bank * 0x4000);     // Not confident I want to use this.
    }

    printf("Create, and Malloc. A Mock ROM with test Data.\n");
    // Fills the rom with mock data.
    uint32_t mock_rom_size = gb->cart.config.rom_size;
    uint8_t mock_banks = gb->cart.config.rom_bank_count;

    gb->cart.storage.rom_data = malloc(mock_rom_size);
    if (!gb->cart.storage.rom_data) {
        fprintf(stderr, "Mock ROM Malloc failed!\n");
        return -1;
    }

    for (uint32_t bank = 0; bank < mock_banks ; ++bank) {
        uint8_t fill = (uint8_t)(bank & 0xFF);              // Makes each bank have different bytes/ data
        memset(gb->cart.storage.rom_data + bank * 0x4000, fill, 0x4000);
    }
    printf("Done.\n");
    printf("Finished all Cartridge Configuration & Mock Rom setup.\n");

    return 0;
}

// DO_NOT_USE Old method, sets static configurations.
int decode_cart_features(GB *gb) {
    printf(":Cart: Decoding Cartridge Features... \n");
    // Set Cartridge type and features.
    switch (headers.cart_type_code) {
        case 0x00:
            gb->cart.config.mbc_type = 0;
            break;
        case 0x01 ... 0x03:
            gb->cart.config.mbc_type = 1;
            gb->cart.config.has_rom_banking = (headers.cart_type_code == 0x02 || headers.cart_type_code == 0x03);
            gb->cart.config.has_ram = (headers.cart_type_code == 0x02 || headers.cart_type_code == 0x03);
            gb->cart.config.has_ram_banking = (headers.cart_type_code == 0x02 || headers.cart_type_code == 0x03);
            gb->cart.config.has_battery = (headers.cart_type_code == 0x03);
            break;
        case 0x13:      // MBC3 + RAM + Battery
            gb->cart.config.mbc_type = 3;
            gb->cart.config.has_ram = 1;
            gb->cart.config.has_battery = 1;
            break;
    }

    // ROM Size / Bank count
    gb->cart.config.rom_size = rom_size_by_code(headers.rom_size_code);
    gb->cart.config.rom_bank_count = rom_bank_by_code(headers.rom_size_code);

    // RAM Size:
    gb->cart.config.ram_size = ram_size_by_code(headers.ram_size_code);

    printf("Done.\n");

    return 0;
}
// DO_NOT_USE Old way to setup cartrige
int configure_cartrige_old(GB *gb) {
    // ROM Size / Bank count
    gb->cart.config.rom_size = rom_size_by_code(headers.rom_size_code);
    gb->cart.config.rom_bank_count = rom_bank_by_code(headers.rom_size_code);

    // RAM Size:
    gb->cart.config.ram_size = ram_size_by_code(headers.ram_size_code);

    return 0;
}


int load_headers(const char *filename) {
    uint8_t full_header[HEADER_SIZE];                   // Not static means, header will be deleted after function finishes.

    printf(":Cart: Load and Decode Headers... \n");
    // Load the entire ROM file
    FILE *rom_file = fopen(filename, "rb");             // rb = Read bytes of the file.
    if (!rom_file) {
        perror("Error reading ROM file\n");
        return -1;
    }

    fseek(rom_file, HEADER_OFFSET, SEEK_SET);           // The offset is 0x0100 to 0x0150
    fread(full_header, 1, HEADER_SIZE, rom_file);
    fclose(rom_file);

    // TO ADD LATER:
    // 0143 - CGB Flag (Tells wether the ROM supports DMB or GBC only.) - Very helpful to know!
    // 80h - Game supports CGB functions, but works on old gameboys also
    // C0h - Game works on CGB onl

    // 0146 - SGB Flag
    // 00h = No SGB functions (Normal Gameboy or CGB only game)
    // 03h = Game supports SGB functions (Super GameBoy)
    // 146B = 014B - Old Licensee Code ... Can be used to see if it will support SGB (Super Gameboy).
    // SGB only supports $33
    printf("Check if game has SGB Flag... ====>%02X\n", full_header[0x46]);


    headers.cart_type_code = full_header[0x47];
    // NOTICE: Header Addresses are usually like "0x0134", how I read the header file, means all those codes have 0x100 removed already.
    headers.cart_type_code = full_header[0x47];
    headers.rom_size_code = full_header[0x48];           // Rom
    headers.ram_size_code = full_header[0x49];           // Ram  (Memory size)
    headers.chksm = full_header[0x4D];

    // Header entry point I haven't really used... maybe delete.
    for (int i = 0; i <= 3; i++) { headers.entry_point[i] = full_header[i]; }     // Load the 3 Byte Entry point
    printf("Done.\n");

    return 0;
}

int load_cartridge(GB *gb, const char *filename) {
    size_t expt_rom_size = rom_size_by_code(headers.rom_size_code);
    //size_t expt_rom_size = cart.config.rom_size;

    printf(":Cart: Loading full ROM into Memory... \n");
    // Load the entire ROM file
    FILE *rom_file = fopen(filename, "rb");             // rb = Read bytes of the file.
    if (!rom_file) {
        perror("Error reading ROM file\n");
        return -1;
    }

    fseek(rom_file, 0, SEEK_END);
    long rom_file_len = ftell(rom_file);
    rewind(rom_file);

    if (expt_rom_size == 0 || rom_file_len < expt_rom_size) {
        fprintf(stderr, "ERROR -> Read ROM Size: %02lX Does NOT match Expected Size: %zu\n", rom_file_len, expt_rom_size);
        fclose(rom_file);
        return -2;
    }

    gb->cart.storage.rom_data = malloc(expt_rom_size);
    if (!gb->cart.storage.rom_data) {
        perror("ERROR -> Failed to allocate Memory for entire ROM file");
        fclose(rom_file);
        return -2;
    }

    fread(gb->cart.storage.rom_data, 1, expt_rom_size, rom_file);
    fclose(rom_file);
    printf("Done.\n");
    return 0;
}

int configure_cartrige(GB *gb) {
    // ROM Size / Bank count
    gb->cart.config.rom_size = rom_size_by_code(headers.rom_size_code);
    gb->cart.config.rom_bank_count = rom_bank_by_code(headers.rom_size_code);

    // RAM Size:
    gb->cart.config.ram_size = ram_size_by_code(headers.ram_size_code);

    return 0;
}


// The main Entry-Point from e_ctrl.c..
int cartridge_init(GB *gb, const char *rom_file) {
    if (load_headers(rom_file) != 0) {
        fprintf(stderr, "Init Cartrige: [LoadHeaders] Error Loading Headers:\n");
        return -1;
    }
    if (load_cartridge(gb, rom_file) != 0) {
        fprintf(stderr, "Init Cartrige: [LoadCartridge] Error loading the cartridge data. (rom file)\n");
        return -1;
    }
    if (mbc_setup(gb, &gb->cart, headers.cart_type_code) != 0) {
        fprintf(stderr, "Init Cartrige: [SetupMBC] Error during mbc setup.\n");
        return -1;
    }
    if (configure_cartrige(gb) != 0) {
        fprintf(stderr, "Init Cartrige: [ConfigCartridge] Error during Config Cartridge.\n");
        return -1;
    }
    return 0;
}


// void execute_rom_bankswitch(uint8_t bank_num) {
//     // Take the provided Bank_num. Use it to change the current ROM Bank.
//     if (cart.config.rom_bank_count <= bank_num) {
//         fprintf(stderr, "FATAL ERROR! \n      ----> Invalid Bank switch Location. ROM Bank Count: 0x%04X | Desired ROM Bank: 0x%04X\n", cart.config.rom_bank_count, bank_num);
//         exit(1);  // catch bad rom Bank switching!
//     }
//     else {
//         cart.cartstorage.current_rom_bank = bank_num;
//         printf("Switched ROM Bank to: 0x%02X, %d\n", bank_num, bank_num);
//     }
// }

// // Reads either fixed bank/ switchable bank.
// uint8_t read_data(uint16_t addr) {
//     uint8_t read_rom_data = 0x0;
//     switch (addr){
//         case 0x0000 ... 0x3FFF:
//             printf(":Cart: Matches ROM Bank 00 -> Fixed Bank\n");
//             read_rom_data = cart.cartstorage.rom_data[addr];
//             //printf(":Cart: FxB\n");
//             //printf(":Cart: Data read from ROM (FxB) -> %02X\n", read_data);
//             return read_rom_data;
//             break;
//         case 0x4000 ... 0x7FFF:
//             printf(":Cart: Matches ROM Bank 01-NN -> Switchable Bank\n");
//             printf(":Cart: Read Address -> %04X\n", addr);
//             printf(":Cart: Current ROM bank -> %04X\n", cart.cartstorage.current_rom_bank);
//             printf(":Cart: ROM BANK ADDR + OFFSET. (Actual location) ---> %04X\n,    With addr - 0x4000: %04X\n", (cart.resource.current_rom_bank * 0x4000), (cart.resource.current_rom_bank * 0x4000) + (addr - 0x4000));
//             read_rom_data = cart.cartstorage.rom_data[(cart.cartstorage.current_rom_bank * 0x4000) + (addr - 0x4000)];
//             printf(":Cart: SwB Read\n");
//             printf(":Cart: Data read from ROM (SwB) -> %02X\n", read_rom_data);
//             return read_rom_data;
//             break;
//         default:
//         return 0xFF;    // If somehow values are out of range for Reading rom Data.
//     }

// }


/*

===-- The Entry point into cart.c from the MMU --===

*/


// cart_rom_read from MMU
uint8_t cart_rom_read(GB *gb, uint16_t addr) {
    return gb->cart.ops.read(&gb->cart, addr);
}

// cart_rom_write from MMU ... => to write intercept
void cart_rom_write(GB *gb, uint16_t addr, uint8_t val) {
    gb->cart.ops.write(&gb->cart, addr, val);
}

// cart_ram_read from MMU
uint8_t cart_ram_read(GB *gb, uint16_t addr) {
    return gb->cart.ops.read_ext(&gb->cart, addr);
}

// cart_ram_write from MMU ... => to write intercept
void cart_ram_write(GB *gb, uint16_t addr, uint8_t val) {
    gb->cart.ops.write_ext(&gb->cart, addr, val);
}
