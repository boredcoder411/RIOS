#define BAUD 9600
#define F_CPU 16000000L
#define UBRR (F_CPU / 16 / BAUD - 1)

#define UartRingBufferSize    255