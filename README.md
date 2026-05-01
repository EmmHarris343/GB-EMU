## Side project for an Original GB Emulator (1989)

### About Project:
1. So far this has been a very fun project, but it's also meant to learn, and get a sense of nostalgia working with old hardware emulation.
3. Everything so far has been written in C when possible.
4. This is using C11 / GCC Compiler on Linux, nothing built was intended to run on Windows.

### Please note:
1. There is absolute zero guarantee there will won't be breaking changes.
2. There is a LOT left to do. This is very far from a completed project.
3. There is no timeline for completion, this is for fun.

## 🎮 PROGRESS Highlghts: Some keys recent changes or milestones:
    ~100% of CPU instructions added.
    ~99% of CPU instructions tested and passed. All Blargg's CPU test roms pass, except for IE interrupt.
    Added basic inputs controls.
    More advanced PPU / LCD rendering, background, window, scrolling, sprites now working for some games.
    Some games are playable!

#### Implemented:
 - [X] All CPU instructions and CB prefixed instructions added.
 - [X] MBC1, MBC2, MBC3. Bank Changing, RAM Read/Writes.
 - [X] Added basic CPU t-cycles for all instructions.
 - [X] Basic Timer. Tick, frames, interrupts.
 - [X] LCD / PPU display.
 - [X] MMU Memory address routing.
 - [X] WRAM, HRAM, VRAM, functionality.
 - [X] Some major bug fixes, corruption, ram issues, no APU crashing issue.
 - [X] Improved render time, and CPU usage.
 - [X] Joypad input and controls.
 - [X] Background scrolling.
 - [X] Window layer functioning.
 - [X] Added palette changing.
 - [X] Renders sprites, and handles OAM logic.
 - [X] MBC3 Basic Ram (Battery-Save) .sav file based on ROM name.

#### Current goals:
 - [ ] Continue to improve timing, response time, rendering, fps, latency.
 - [ ] Add for all MBCs Cartridge battery-memory save/ .sav files - DID FOR MBC3
 - [ ] Add MBC3 Cartridge RTC (Real-Time-Clock)
 - [ ] Improve timing accuracy for CPU t-cycles, DIV, Frames
 - [ ] Improve debuggging, traces, snapshots, crash handling
 - [ ] Basic audio

## Longterm Goals:
 - Full PPU emulation. Screen Scrolling, sprites, backgounds, OAM objects, scrolling
 - Full MBC Emulation. (All Versions) + MBC Ram Saves, RTC, Timers, Rumble Support
 - APC Audio implemented (Heard is very hard)
 - Efficent, smart way of coding emulation
 - Cycle Accuracy, Timings, Speed, Framerate
 - Handle Known GB Bugs
 - Handling of multiple colour palette
#### "Would be nice" but so far away, closer to a dream.
 - GB Color mode and colour palletes. Support some GBC games.
 - Support for Link-Cable
 - Visual debugger, inspector
 - Instruction viewer, UI based instruction monitor, save and record,
 - Headless mode. Pre-set instructions for game.
 - Hyper speed, be able to easily scale speed, for testing, or simulations
 - Able to run in parallel
 - Bot controlled, or Machine learning/ simulation mode



NOTICE:
This project is a personal educational effort to understand how the original Game Boy works. It does not contain or distribute any copyrighted ROMs or BIOS data from Nintendo or others.