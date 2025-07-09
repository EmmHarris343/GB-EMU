# CMD
# make -f Makefile gb_em
cc = gcc


CFLAGS = -g -std=c11 -Wall 
# -Wextra

# Object files (Not all of these exist)
OBJ = e_core.o e_ctrl.c cart.c cpu.c cpu_instructions.c loc_ram.c ppu.c oam.c apu.c mmu.c logger.c ../test_cpu/cpu_test.c ../test_cpu/common/utile.c ../test_cpu/0_alu/alu_test.c

# Single Executable:
gb_em: $(OBJ)
	$(CC) $(CFLAGS) -o gb_em $(OBJ)

# Compile .c into .o
# %.o %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Clean the obj files
clean:
	rm -f *.o gameboy