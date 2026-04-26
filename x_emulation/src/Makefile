# CMD
# make -f Makefile gb_em
cc = gcc


CFLAGS = -g -std=c11 -Wall
# -Wextra

# Object files (Not all of these exist)
OBJ = e_core.o e_ctrl.c core/gb.c core/cpu/cpu.c core/cpu/cpu_instructions.c\
	  core/io.c core/mmu.c core/loc_ram.c\
	  core/cart/cart.c core/cart/mbc.c\
	  core/ppu/ppu.c\
	  core/joy/joy.c core/timer/timer.c core/apu/apu.c debug/logger.c\
	  core/video/display.c\
	  core/video/adapter.c\
	  host/basic_viewer.c

# removed build files:  ../test_cpu/cpu_test.c ../test_cpu/common/utile.c ../test_cpu/0_alu/alu_test.c


# Single Executable:
gb_em: $(OBJ)
	$(CC) $(CFLAGS) -o gb_em $(OBJ) $(shell sdl2-config --cflags --libs)

# Compile .c into .o
# %.o %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Clean the obj files
clean:
	rm -f *.o gameboy