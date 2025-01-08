# Makefile

# Compiler and flags
CC = avr-gcc
CFLAGS = -Wall -g -Os -mmcu=atmega328p

# Object and output files
OBJ = build/main.o
HEX = build/main.hex

# Default target
all: $(HEX)

# Build target
$(OBJ): main.c uart.c
	mkdir -p build
	$(CC) $(CFLAGS) -o $(OBJ) main.c uart.c

# Link target
$(HEX): $(OBJ)
	avr-objcopy -j .text -j .data -O ihex $(OBJ) $(HEX)

# Clean target
clean:
	rm -rf build

.PHONY: all clean
