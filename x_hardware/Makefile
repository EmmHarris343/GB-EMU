cc = gcc


CFLAGS = -std=c11 #-Wall 
# -Wextra

# Object files (Not all of these exist)
OBJ = e_core.o cpu.c cpu_instructions.c cart.c mbc.c

# Single Executable:
gb_em: $(OBJ)
	$(CC) $(CFLAGS) -o gb_em $(OBJ)

# Compile .c into .o
# %.o %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Clean the obj files
clean:
	rm -f *.o gameboy