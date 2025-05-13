#include "cart.h"

Cartridge cart;         // doing *cart means that I need to allocate the memory, maybe I will do that, but for now this works


#include "string.h"


/*
        Steps:
        1. Check ROM File
        2. Read Headers from ROM FIle
        3. Determin ROM Size, MBC, if Bank Switching
        4. Load ROM Fixed Bank into Memory
        5. Load ROM Switchable Bank into Array

*/ 

/// TODO: WRITE INTERCEPTS:
/*

Write Table: (Intercepting)
    0000 - 3FFF => FIXED BANK
    0000 - 3FFF (FIXED BANK)    => READ         ACTION: Returns Data in the FIXED BANK ROM.
    0000 - 1FFF (Fixed bank)    => Write        ACTION: Enable/ Disable RAM
    2000 - 3FFF (Fixed Bank)    => Write        ACTION: ROM Bank Switch

    4000 - 7FFF => SWITCH BANK
    4000 - 7FFF (FIXED BANK)    => READ         ACTION: Returns Data in the currently Selected, Switchable ROM BANK
    4000 - 5FFF (Switch BANK)   => Write        ACTION: RAM Bank Switch
    6000 - 7FFF (SWITCH BANK)   => Write        ACTIOH: ROM/RAM Mode 0/1 SWITCH

    A000 - BFFF => EXTERNAL RAM
    A000 - BFFF (EXT RAM)       => READ/ WRITE  ACTION: Reads from External RAM / Writes to External RAM
*/




// Some re-usable functions:
uint32_t rom_size_by_code(uint8_t rom_size_code) {
    const uint32_t rom_size_lookup[0x55] = {
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
    return rom_size_lookup[rom_size_code];
}

uint16_t rom_bank_by_code(uint8_t rom_bank_code) {
    const uint16_t rom_bank_lookup[0x55] = {
        2, 4, 8, 16, 32, 64, 128, 256, 512,        
        [0x52] = 72, [0x53] = 80, [0x54] = 96 // Special $52 - $54 Rom Size codes:
    };
    return rom_bank_lookup[rom_bank_code];
}

size_t ram_size_by_code(uint8_t ram_code) {    
    size_t ram_size = 0;
    switch (cart.headers.ram_size_code) {
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





int init_cart_test_mode() {
    // Default to DMG 01 | MBC 1
    uint8_t full_header[HEADER_SIZE];
    printf("Starting Mock Rom/ Cartridge Configuration.. \n");


    /// NOTICE: These are hard coded values. Intended strictly for testing

    cart.headers.cart_type_code = 0x03;          // MBC (Zda MBC1 = 0x02 - 0x03)
    cart.headers.rom_size_code = 0x04;           // Rom (Zda uses 0x04)
    cart.headers.ram_size_code = 0x02;           // Ram (Memory size)    

    // ROM Size / Bank count
    cart.config.rom_size = rom_size_by_code(cart.headers.rom_size_code);
    cart.config.rom_bank_count = rom_bank_by_code(cart.headers.rom_size_code);

    // RAM Size:
    cart.config.ram_size = ram_size_by_code(cart.headers.ram_size_code);

    // Some Other Config Settings.
    cart.config.mbc_type = 1;
    cart.config.has_rom_banking = 1;
    cart.config.has_ram = 1;
    cart.config.has_ram_banking = 1;
    cart.config.has_battery = 1;

    // Some default Startup Settings for Rom.
    if (cart.config.has_ram == 1) {
        cart.resrce.ram_toggle = 0;
    }
    if (cart.config.has_ram_banking) {
        cart.resrce.current_ram_bank = 0x00;
    }
    if (cart.config.has_rom_banking == 1) {
        cart.resrce.current_rom_bank = 1;           // Defaulting to bank 1 (WARNING! this behaviour is not consistant in all MBC!)
        cart.resrce.fixed_b_addr = 0x00;
        cart.resrce.calcd_switch_addr = (cart.resrce.current_rom_bank * 0x4000);     // Not confident I want to use this.
    }

    printf("Create, and Malloc. A Mock ROM with test Data.\n");
    // Fills the rom with mock data.
    uint32_t mock_rom_size = cart.config.rom_size;
    uint8_t mock_banks = cart.config.rom_bank_count;

    cart.resrce.rom_data = malloc(mock_rom_size);
    if (!cart.resrce.rom_data) {
        fprintf(stderr, "Mock ROM Malloc failed!\n");
        return -1;
    }

    for (uint32_t bank = 0; bank < mock_banks ; ++bank) {
        uint8_t fill = (uint8_t)(bank & 0xFF);              // Makes each bank have different bytes/ data
        memset(cart.resrce.rom_data + bank * 0x4000, fill, 0x4000);
    }
    printf("Done.\n");
    printf("Finished all Cartridge Configuration & Mock Rom setup.\n");

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

    // NOTICE: Header Addresses are usually like "0x0134", how I read the header file, means all those codes have 0x100 removed already.
    cart.headers.cart_type_code = full_header[0x47];
    cart.headers.rom_size_code = full_header[0x48];           // Rom
    cart.headers.ram_size_code = full_header[0x49];           // Ram  (Memory size)
    cart.headers.chksm = full_header[0x4D];
    for (int i = 0; i <= 3; i++) { cart.headers.entry_point[i] = full_header[i]; }     // Load the 3 Byte Entry point
    printf("Done.\n");

    return 0;
}

int decode_cart_features() {
    printf(":Cart: Decoding Cartridge Features... \n");
    // Set Cartridge type and features.
    switch (cart.headers.cart_type_code) {
        case 0x00:
            cart.config.mbc_type = 0;
            break;
        case 0x01 ... 0x03:
            cart.config.mbc_type = 1;
            cart.config.has_rom_banking = (cart.headers.cart_type_code == 0x02 || cart.headers.cart_type_code == 0x03);
            cart.config.has_ram = (cart.headers.cart_type_code == 0x02 || cart.headers.cart_type_code == 0x03);
            cart.config.has_ram_banking = (cart.headers.cart_type_code == 0x02 || cart.headers.cart_type_code == 0x03);
            cart.config.has_battery = (cart.headers.cart_type_code == 0x03);
            break;
        case 0x13:      // MBC3 + RAM + Battery
            cart.config.mbc_type = 3;
            cart.config.has_ram = 1;
            cart.config.has_battery = 1;
            break;
    }

    //cart.config.rom_size = rom_size_lookup[rom_size_code];
    //cart.config.rom_bank_count = rom_bank_lookup[rom_size_code];

    // ROM Size / Bank count
    cart.config.rom_size = rom_size_by_code(cart.headers.rom_size_code);
    cart.config.rom_bank_count = rom_bank_by_code(cart.headers.rom_size_code);

    // RAM Size:
    cart.config.ram_size = ram_size_by_code(cart.headers.ram_size_code);

    printf("Done.\n");

    return 0;
}




int load_cartridge(const char *filename) {

    size_t expt_rom_size = cart.config.rom_size;

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

    cart.resrce.rom_data = malloc(expt_rom_size);
    if (!cart.resrce.rom_data) {
        perror("ERROR -> Failed to allocate Memory for entire ROM file");
        fclose(rom_file);
        return -2;
    }

    fread(cart.resrce.rom_data, 1, expt_rom_size, rom_file);
    fclose(rom_file);
    printf("Done.\n");
    return 0;
}


int initialize_cartridge() {
    // Load resources and info, set bank values, ram values, etc
    // printf("What does the Has ram look like? %02X, %d\n", cart.config.has_ram, cart.config.has_ram);
    if (cart.config.has_ram == 1) {
        cart.resrce.ram_toggle = 0;
    }
    if (cart.config.has_rom_banking == 1) {
        cart.resrce.current_rom_bank = 1;           // Defaulting to bank 1 (WARNING! this behaviour is not consistant in all MBC!)
        cart.resrce.fixed_b_addr = 0x0;
        cart.resrce.calcd_switch_addr = (cart.resrce.current_rom_bank * 0x4000);     // Not confident I want to use this.
    }
    else {
        // Is a test ROM / Tiny ROM (IE Tetris)
        cart.resrce.current_rom_bank = 1;           // This is a work around. This makes it calculate 0x4000 = extended rom bank.

    }

    return 0;

    //uint8_t* bankN = &cart.resrce.rom_data[cart.resrce.current_rom_bank * 0x4000];
}


void execute_ROM_BSWTCH(uint8_t Bank_num) {
    // Take the provided Bank_num. Use it to change the current ROM Bank.
    if (cart.config.rom_bank_count <= Bank_num) {
        fprintf(stderr, "FATAL ERROR! \n      ----> Invalid Bank switch Location. ROM Bank Count: 0x%04X | Desired ROM Bank: 0x%04X\n", cart.config.rom_bank_count, Bank_num);
        exit(1);  // catch bad rom Bank switching!
    }
    else {
        cart.resrce.current_rom_bank = Bank_num;
        printf("Switched ROM Bank to: 0x%02X, %d\n", Bank_num, Bank_num);
    }


}


uint8_t read_data(uint16_t addr) {
    uint8_t read_rom_data = 0x0;
    switch (addr){        
        case 0x0000 ... 0x3FFF:
            printf(":Cart: Matches ROM Bank 00 -> Fixed Bank\n");           
            read_rom_data = cart.resrce.rom_data[addr];
            //printf(":Cart: FxB\n");
            //printf(":Cart: Data read from ROM (FxB) -> %02X\n", read_data);
            return read_rom_data;
            break;
        case 0x4000 ... 0x7FFF:
            printf(":Cart: Matches ROM Bank 01-NN -> Switchable Bank\n");
            printf(":Cart: Read Address -> %04X\n", addr);
            printf(":Cart: Current ROM bank -> %04X\n", cart.resrce.current_rom_bank);
            printf(":Cart: ROM BANK ADDR + OFFSET. (Actual location) ---> %04X\n,    With addr - 0x4000: %04X\n", (cart.resrce.current_rom_bank * 0x4000), (cart.resrce.current_rom_bank * 0x4000) + (addr - 0x4000));
            read_rom_data = cart.resrce.rom_data[(cart.resrce.current_rom_bank * 0x4000) + (addr - 0x4000)];
            printf(":Cart: SwB Read\n");
            printf(":Cart: Data read from ROM (SwB) -> %02X\n", read_rom_data);
            return read_rom_data;
            break;
        default:
        return 0xFF;    // If somehow values are out of range for Reading rom Data.
    }
        
}

// This is realistically, going to be RAM space. (maybe other I guess)
void write_data(uint16_t addr) {

}


void write_intercept(uint16_t addr, uint8_t write_value) {
    switch(addr) {
        case 0x0000 ... 0x1FFF:
            // Ram Enable
            printf("Enable RAM\n");
            break;
        case 0x2000 ... 0x3FFF:
            // ROM "file" Bank Switch
            execute_ROM_BSWTCH(write_value);
            break;
        case 0x4000 ... 0x5FFF:
            // RAM Bank Switch
            break;
        case 0x6000 ... 0x7FFF:
            // ROM/RAM Mode 0/1 Switch
            break;
    }

}








// Entry point from MMU:

uint8_t cart_read(uint16_t addr) {
    // Memory Locations are 16bit
    // Values stored in those locations are 8bit.

    /// TODO:
    // Read the memory at either, fixed ROM, or Switchable Bank.
    // Return the value read.
    return read_data(addr); // Just pass this back.
    
}
void cart_write(uint16_t addr, uint8_t val) {
    // There is only really 2 fields. 
    // A write intercept Command.
    // Storing data into RAM (maybe something to do with clock too). That's it.

    if (addr >= 0x000 && addr <= 0x7FFF) {
        printf(":cart: Intercepting Write instruction\n");
        write_intercept(addr, val);
    }
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        printf(":cart: Inside External RAM Write Space\n");

    }

}

uint8_t cart_ram_read(uint16_t addr) {
    
    return 0xFF; // Returns dummy response of 0xFF

}
void cart_ram_write(uint16_t addr, uint8_t val) {

}