void uart_init();

int uart_read(char *buf, int len);
int uart_write(char *buf, int len);
bool uart_available();
char uart_getchar();
char* uart_get_buffer();

extern char rx_buffer[64];