#include <stdio.h>
#include <unistd.h>			
#include <fcntl.h>			
#include <termios.h>		
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
 #include <pthread.h>
#include "../Rpi_Setup/Check_Hardware.h"
#include "../P2P_Stream/stream.h"

pthread_t rx_thread;
int StopUartReceive;

int open_uart (const char *device, const int baud);
int tx_uart (int fd, const char *tx_buffer, int len);
char* rx_uart (int fd, char *rx_buffer, int num_byte_receive_expected);
int startUartReceiveThread();
