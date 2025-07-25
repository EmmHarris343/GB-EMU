#include <stdio.h>
#include "cart.h"


Cartridge cart; 

// Rom Header:
#define HEADER_OFFSET       0x0100          // Same as Start, just easier to understand
#define HEADER_SIZE         0x50            // Header END point is: 0x014f (50 Bytes Total - 80 Decimal)

/*

    cart.c:
        Handles MBC configuration based on ROM (Headers)
        Emulations All Cartridge MBC Functions. 
        ROM, RAM Switching, Enable/ Disable RAM, RTC.. etc etc

        Will also store RAM, RTC, & ROM Loading


*/


Cartridge cartridge;        // This must be defined once. HERE makes the most sense (If not, the linker will throw an error and won't compile)

uint8_t ROM_header_raw[HEADER_SIZE];        // Storage of ROM Header

// Untested, but might work. (More of a placeholder, to test the Function pointers actually work)
void mbc1_write(uint16_t addr, uint8_t val) {
    if (addr >= 0x2000 && addr <= 0x3FFF) {
        
        // Change the ROM bank based on the value
        uint8_t current_rom_bank = val & 0x1F; // Only 5 bits used
        if (current_rom_bank == 0) current_rom_bank = 1; // Bank 0 is forbidden
    }
}


void mbc3_write(uint16_t addr, uint8_t val) {
    // MBC3-specific behavior
}








// Next: Configure Cart.c settings based on the decoded Cart features.




// ------------------------------- Actual Functionality:
/*

Write Table: (Intercepting)

    0000 - 3FFF => FIXED BANK
    0000 - 3FFF (FIXED BANK)    => READ         ACTION: Returns Data in the FIXED BANK ROM.
    0000 - 1FFF (Fixed bank)    => Write        ACTION: Enable/ Disable RAM
    2000 - 3FFF (Fixed Bank)    => Write        ACTION: ROM Bank Switch

    4000 - 7FFF => SWITCH BANK
    4000 - 7FFF (FIXED BANK)    => READ         ACTION: Returns Data in the previously Selected, Switchable ROM BANK
    4000 - 5FFF (Switch BANK)   => Write        ACTION: RAM Bank Switch
    6000 - 7FFF (SWITCH BANK)   => Write        ACTIOH: ROM/RAM Mode 0/1 SWITCH

    A000 - BFFF => EXTERNAL RAM
    A000 - BFFF (EXT RAM)       => READ/ WRITE  ACTION: Reads from External RAM / Writes to External RAM
*/






void read_ROM(uint8_t address) {       
    // 1. Read ROM in FIXED BANK
    // 2. Read ROM in Switch BANK       (What ever was previously Selected)

    // Area:
    // 0000 - 3FFF (FIXED BANK)
    // 4000 - 7FFF => SWITCH BANK


}

void enable_dis_RAM(uint8_t address){   // Enable / Disable RAM usage
    
    /*
        NOTICE: ONLY is for MBC2
        If the value 0A (0000 1010 bits) is written. This will disable RAM.
        If the 
        
    */
   
   if (cartridge.config.mbc_type == 2) {
        // IF Bit 8 (LSB of upper Address Byte) is 0. 
        // The value written in lower address of byte. (first 4 bits). 
        // Determins the Enable/ Disable.

        if ((address & 0x0F) == 0x0A) {     // Select the last 4 bits, Check if they match 0x0A --- (Note, 0x0A & 0xA = 0x0A (Decimal 10) and they are the same thing)
            
        }
   }
}

void enable_dis_RTC(uint8_t address) {  // Enable / Disable RTC (Real Time Clock) MBC 3 ONLY

}

void latch_RTC(uint8_t address) {

}

void set_ROM_Bank(uint16_t address, uint8_t data) {
    // MBC 1, and other MBCs follow. IF: 
    printf("Set ROM bank Function %2x\n", data);


    if (cartridge.config.mbc_type == 1) { 
        // ROM Bank range is: 01-7F

        // Has weird logic, if bit 5 is 1. (I think)
        // Shift to bank +1, so 20,40,60 = 21,41,61.
        // Plus some other logic for Multi Game carts
        // NOTE: Some of these roms are 1Mib+

        // Instead of: Default of: MBC1 upto 512 KiB ROM & upto KiB of banked RAM.
        // 2Mbit ROMS sacrifice ROM space, for more RAM space
    }
    
    if (cartridge.config.mbc_type == 2) { 
        // Max Rom Bank is $01-$0F | MAX of 16 ROM Banks supported
    }

    if (cartridge.config.mbc_type == 3) { 
        // Supports 2MB ROMs (128 Banks) & 32KB RAM (4 banks)
        // Bank switching is the same as MBC1. BUT, has no limitation for 20,40,60 bla bla

        // This is likely easist Logic. (Excluding the RTC Clock).
        // Writing value of $00 will select Bank 1, All other Values are correct for selecting the ROM banks
        // Value of: $01 - $7F (125 ROM Banks)

        if (data == 0x00){
            
        }
        if (data <= cartridge.config.rom_bank_count) {      // Quick note, MBC3 supports up to 2MB / 128 banked ROM (But not all use that, some only use 1MB 64 Bank)
           
        }


    }
}

void set_RAM_Bank(uint8_t address) {

}

void rom_ram_mode(uint8_t address) { // ROM/RAM Mode 0 / 1 

}



void mbc2_RAM(uint16_t address, uint8_t data) {
    // Check if RAM was, and still is enabled
    // Check if address has BIT 8, set as 1



    // This is mostly MOCK Code. (Another words.. likely won't work. )
    uint8_t technically_bit8_flag = (address >> 7) & 0x1;  // Shift over by 7, then select first 1 bit (Rightmost bit)?
    uint8_t bit8_flag = 1;

    if ((bit8_flag != 1) && ((data & 0xF) == 0x0A)) {
        // Enable RAM.
        cartridge.resrce.ram_toggle = 1;
    }

    if (bit8_flag == 1) {   // I suppose technically, it could be 0x0A, but if the memory is in the Address space for Bank Switching....Well Switch Bank..

        printf("Ignoring Bank Change. ROM Not enabled.");
    }
    if ((bit8_flag != 1) && ((data & 0xF) != 0x0A)) {
        printf("8 Bit FLAG not set. Data isn't 0x0A. DISABLE ROM!");
    }
}


// ADDRESS MUST BE 16bit. Because 8bit is too short for the address space 0x0000 - 0xFFFF (1 "digit" is 4 bits. 2*4 = 8bit, 4*4 = 16bit.)
void write_intercept(uint16_t address, uint8_t data) {    
    /*  (Quick recap)
        CPU Will try to write to ROM space. (Which is im possible)
        If you intercept where this ADDRESS is trying to write to, and the DATA trying to be writen. 

        You can interpret the ROMs original instructions to the MBC.
        IE: Enable/ Disable RAM, Switch ROM Bank, Switch RAM Bank etc..
    
    */


    switch (address){
    case 0x0000 ... 0x1FFF: // NOTICE, DOC SAYS SUGGESTED area is: 0000-00FF
        // Enable/ Disable  => RAM      (IF MBC 3, Enable RTC Registers -- Real Time Clock)
        enable_dis_RAM(data);
        if (cartridge.config.mbc_type == 3) {
            enable_dis_RTC(data);
        }
        break;
    case 0x2000 ... 0x3FFF:
        // Change the selected ROM Bank (Read only Memory)
        set_ROM_Bank(address, data);
        break;
    case 0x4000 ... 0x5FFF:
        // RAM Switch Bank (Random access memory)
        set_RAM_Bank(data);
        if (cartridge.config.mbc_type == 5) {
            // Note technically this is RAM Switch / Rumble Enable Space. (But only for MBC 5)
        }
    case 0x6000 ... 0x7FFF:
        if (cartridge.config.mbc_type == 2) {
            // ROM/RAM MODE 0/1 SET
            rom_ram_mode(data);
        }
        if (cartridge.config.mbc_type == 3) {
            latch_RTC(data);
        }
        break;
    default:
        break;
    }

}



// Next: RAM Write to "ROM" space
void write_RAM(uint8_t address, uint8_t data) {
    // Switchable External Ram Space: A000h-BFFFh      (If any available)

}

















// This is misnamed.. This is just a test "main" so to speak
void configure_mbc(Cartridge *cart) {

    if (cart->config.mbc_type == 3) {           // MBC 3: Max 2MByte ROM and/or 32KByte RAM and Timer
        printf("\nDEBUG: Rom Matches MBC Cart Type 3 -- Upto 2MB ROM, and/or 32KB RAM and Timer\n\n");
        cart->mbc_write = mbc1_write;
    }

    printf("ROM Banking? => %d\n", cart->config.rom_bank_count);
    
}




