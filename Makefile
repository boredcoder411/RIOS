# Makefile

# Compiler and flags
CC = avr-gcc
CFLAGS = -Wall -g -Os -mmcu=atmega328p

# Object and output files
OBJ = build/main.o
HEX = build/main.hex

# Port to use for upload
PORT ?= 

# Ensure the PORT variable is set
ifeq ($(PORT),)
$(error Usage: make upload PORT=<your_serial_port>)
endif

# Default target
all: $(HEX)

# Build target
$(OBJ): main.c uart.c
	mkdir -p build
	$(CC) $(CFLAGS) -o $(OBJ) main.c uart.c

# Link target
$(HEX): $(OBJ)
	avr-objcopy -j .text -j .data -O ihex $(OBJ) $(HEX)

# Upload target
upload: $(HEX)
	sudo avrdude -c arduino -p atmega328p -P $(PORT) -b 115200 -U flash:w:$(HEX)

# Clean target
clean:
	rm -rf build

.PHONY: all upload clean
