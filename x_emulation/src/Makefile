cc = gcc


CFLAGS = -std=c11 -Wall 
# -Wextra

# Object files (Not all of these exist)
OBJ = e_core.o e_ctrl.c cart.c cpu.c cpu_instructions.c loc_ram.c ppu.c oam.c apu.c mmu.c logger.c

# Single Executable:
gb_em: $(OBJ)
	$(CC) $(CFLAGS) -o gb_em $(OBJ)

# Compile .c into .o
# %.o %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Clean the obj files
clean:
	rm -f *.o gameboy