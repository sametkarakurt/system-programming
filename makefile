# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Iinclude

# Source files
SRCS = src/main.c src/tarsau.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = tarsau

# Default target
all: $(EXEC)

# Compile C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create the executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

# Clean up generated files
clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean
