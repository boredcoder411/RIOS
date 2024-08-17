void uart_init();

int uart_read(char *buf, int len);
int uart_write(char *buf, int len);
bool uart_available();
char uart_getchar();