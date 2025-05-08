## Side project for an Original GB Emulator (1989)

So far this has been an extremely fun project, seeing it slowly get pieced together. 

----

This is intended to be written entirely in C when possible. Using the C11/ GCC Compiler on Linux. Nothing being built is intended to run on windows.

##### Completing this project quickly is not my intention. It's to have fun, learn more, and get some sense of nostalgic working with old hardware emulation. 


#### NOTE: This is VERY far from any functionality
    This only has the most basic infastructure put in place.
    Only some CPU OPCodes are functional. Very little are tested.
    This is far from playable, or usable.
    There is absolute zero gaurentee there will won't be breaking changes.
    There is a LOT left to do.
    There is no timeline for completion, this is for fun.

------------
That being said. My goals are not set in stone. But at bare-min basic functionality would be nice.

### 🎮 My priority so far:
 - [ ] Efficent, smart way of coding emulation (Will I ever truely check this?)
 - [ ] MBC Emulation, banking switching, Cartrige RAM
 - [ ] MMU address space handling.
 - [ ] CPU, OP Code / PC syncronization. 
 - [X] CPU OP Code/ Instruction Emulation (Basic at first)
 - [ ] All OPCodes & Prefixed CPU instructions finished
 - [ ] Test & Verify All CPU OPCodes ensuring no Desyncranization occures
 - [ ] Debugger, logger, way to verify each Instruction, RAM etc
 - [ ] VRAM, GPU, LCD Emulation
 - [ ] Rendering of screen, logos, images, sprites.
 - [ ] PPU, APU emulation (Visual, LCD, Audio)
 - [ ] .. more I am sure

#### Hope to reach:
 - [ ] Full MBC Emulation. (All Versions) + MBC Ram Saves, Timers, Rumble Support
 - [ ] Handle Known GB Bugs
 - [ ] Handling of multiple colour palette
 - [ ] Cycle Accuracy, Correct Speed

#### Special "Would be nice", but so far away, closer to a dream.
 - [ ] Support for Link-Cable 
 - [ ] Visual debugger, inspector
 - [ ] Instruction viewer, UI based instruction monitor, save and record,
 - [ ] Headless mode. Pre-set instructions for game.
 - [ ] Hyper speed, be able to easily scale speed, for testing, or simulations
 - [ ] Able to run in parallel 
 - [ ] Bot controlled, or Machine learning/ simulation mode



NOTICE:
This project is a personal educational effort to understand how the original Game Boy works. It does not contain or distribute any copyrighted ROMs or BIOS data from Nintendo or others.