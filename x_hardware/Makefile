cc = gcc


CFLAGS = -Wall -Wextra -std=c11

# Object files (Not all of these exist)
OBJ = e_core.o memory.o cpu.o ppu.o main.o

# Single Executable:
g_emu: $(OBJ)
	$(CC) $(CFLAGS) -o g_emu $(OBJ)

# Compile .c into .o
%.o %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Clean the obj files
clean:
	rm -f *.o gameboy