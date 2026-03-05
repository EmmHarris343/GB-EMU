#include "cart.h"

//#include <stdio.h>  // for print / debugging stuff






// Make this file almost entirely MBC configuration. Specific to each individual MBC

int init_mbc(Cartridge *cart) {

    return 0;
}

void load_MBC_Type(uint8_t mbc_code, Cartridge *cart) {
    // Figure out, based on the mbc code. What the MBC type is.



}

void mbc_none(Cartridge *cart) {

}

void mbc1(Cartridge *cart) {   // Implement Medium priority (Lots of older titles)
    // MBC1 Header codes:
    // $01	MBC1
    // $02	MBC1+RAM
    // $03	MBC1+RAM+BATTERY

    int mbc1Type = 0;

    // 0x01 ... 0x03 => MBC 1

    cart->config.mbc_type = 1;
    if (cart->headers.cart_type_code == 0x02) { mbc1Type = 2; }
    if (cart->headers.cart_type_code == 0x03) { mbc1Type = 3; }
    if (mbc1Type == 2 || mbc1Type == 3) {
        cart->config.has_rom_banking = 1;
        cart->config.has_ram = 1;
        cart->config.has_ram_banking = 1;
    }
    if (mbc1Type == 3) {
        cart->config.has_battery = 1;
    }
}

void mbc2 () {  // Unknown priority. Likely lower.
    /*
    MBC 2 Header codes:

    $05	MBC2
    $06	MBC2+BATTERY

    */
}

/*
*** MBC 3 *** - Implement Higher Priority.

Beside for the ability to access up to:
2MB ROM (128 banks), and 32KB RAM (4 banks).
The MBC3 also includes a built-in Real Time Clock (RTC).


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


*/
void mbc3(Cartridge *cart) {
    /*
    MBC3 Header codes:

    $0F	MBC3+TIMER+BATTERY
    $10	MBC3+TIMER+RAM+BATTERY
    $11	MBC3
    $12	MBC3+RAM
    $13	MBC3+RAM+BATTERY

    Notice: MBC3 with 64 KiB of SRAM refers to MBC30, used only in Pocket Monsters: Crystal Version (the Japanese version of Pokémon Crystal Version).
    */

    // MBC3 + RAM + Battery
    cart->config.mbc_type = 3;
    cart->config.has_ram = 1;
    cart->config.has_battery = 1;
    cart->config.has_rtc = 1;
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