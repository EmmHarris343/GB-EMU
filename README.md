## Side project for an Original GB Emulator (1989)

### About Project:
1. So far this has been a very fun project, but it's also meant to learn, and get a sense of nostalgia working with old hardware emulation.
3. Everything so far has been written in C when possible.
4. This is using C11 / GCC Compiler on Linux, nothing built was intended to run on Windows.

### Please note:
1. There is absolute zero guarantee there will won't be breaking changes.
2. There is a LOT left to do. This is very far from a completed project.
3. There is no timeline for completion, this is for fun.

## 🎮 PROGRESS: I do want to note some progress so far:
    ~100% of CPU instructions added.
    ~99% of CPU instructions tested and passed. All Blargg's CPU test roms pass, except for IE interrupt.
    Basic Emulation now works!
    Basic LCD / PPU rendering now works. Can display video.

#### Implemented:
 - [X] All CPU instructions and CB prefixed instructions added.
 - [X] MBC1, MBC2, MBC3. Bank Changing, RAM Read/Writes.
 - [X] Added basic CPU t-cycles for all instructions.
 - [X] Basic Timer. Tick, frames, interrupts.
 - [X] LCD / PPU display, currently renders backgrounds.
 - [X] MMU Memory address routing.
 - [X] WRAM, HRAM, VRAM, functionality.

#### Current goals:
 - [ ] Add Joypad inputs
 - [ ] Add Cartridge battery-memory save/ .sav files
 - [ ] Add MBC3 Cartridge RTC (Real-Time-Clock)
 - [ ] Render sprites, add OAM support
 - [ ] Handle Background scrolling
 - [ ] Improve timing accuracy for CPU t-cycles, DIV, Frames
 - [ ] Improve debuggging, traces, snapshots, crash handling

## Longterm Goals:
 - Full PPU emulation. Screen Scrolling, sprites, backgounds, OAM objects, scrolling
 - Full MBC Emulation. (All Versions) + MBC Ram Saves, RTC, Timers, Rumble Support
 - APC Audio implemented (Heard is very hard)
 - Efficent, smart way of coding emulation
 - Cycle Accuracy, Timings, Speed, Framerate
 - Handle Known GB Bugs
 - Handling of multiple colour palette
#### "Would be nice" but so far away, closer to a dream.
 - Support for Link-Cable
 - Visual debugger, inspector
 - Instruction viewer, UI based instruction monitor, save and record,
 - Headless mode. Pre-set instructions for game.
 - Hyper speed, be able to easily scale speed, for testing, or simulations
 - Able to run in parallel
 - Bot controlled, or Machine learning/ simulation mode



NOTICE:
This project is a personal educational effort to understand how the original Game Boy works. It does not contain or distribute any copyrighted ROMs or BIOS data from Nintendo or others.