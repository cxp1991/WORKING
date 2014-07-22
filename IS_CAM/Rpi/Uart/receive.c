#include <stdio.h>
#include <unistd.h>			
#include <fcntl.h>			
#include <termios.h>		
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
 #include <pthread.h>

int open_uart (const char *device, const int baud)
{
  struct termios options ;
  speed_t myBaud ;
  int     status, fd ;

  switch (baud)
  {
    case     50:	myBaud =     B50 ; break ;
    case     75:	myBaud =     B75 ; break ;
    case    110:	myBaud =    B110 ; break ;
    case    134:	myBaud =    B134 ; break ;
    case    150:	myBaud =    B150 ; break ;
    case    200:	myBaud =    B200 ; break ;
    case    300:	myBaud =    B300 ; break ;
    case    600:	myBaud =    B600 ; break ;
    case   1200:	myBaud =   B1200 ; break ;
    case   1800:	myBaud =   B1800 ; break ;
    case   2400:	myBaud =   B2400 ; break ;
    case   4800:	myBaud =   B4800 ; break ;
    case   9600:	myBaud =   B9600 ; break ;
    case  19200:	myBaud =  B19200 ; break ;
    case  38400:	myBaud =  B38400 ; break ;
    case  57600:	myBaud =  B57600 ; break ;
    case 115200:	myBaud = B115200 ; break ;
    case 230400:	myBaud = B230400 ; break ;

    default:
      return -2 ;
  }

  if ((fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1)
    return -1 ;

  fcntl (fd, F_SETFL, O_RDWR) ;

// Get and modify current options:

  tcgetattr (fd, &options) ;

    cfmakeraw   (&options) ;
    cfsetispeed (&options, myBaud) ;
    cfsetospeed (&options, myBaud) ;

    options.c_cflag |= (CLOCAL | CREAD) ;
    options.c_cflag &= ~PARENB ;
    options.c_cflag &= ~CSTOPB ;
    options.c_cflag &= ~CSIZE ;
    options.c_cflag |= CS8 ;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    options.c_oflag &= ~OPOST ;

    options.c_cc [VMIN]  =   0 ;
    options.c_cc [VTIME] = 100 ;	// Ten seconds (100 deciseconds)

  tcsetattr (fd, TCSANOW | TCSAFLUSH, &options) ;

  ioctl (fd, TIOCMGET, &status);

  status |= TIOCM_DTR ;
  status |= TIOCM_RTS ;

  ioctl (fd, TIOCMSET, &status);

  usleep (10000) ;	// 10mS

  return fd ;
}

int tx_uart (int fd, const char *tx_buffer)
{
	int tx_bytes;
	tx_bytes = write(fd, (void*) tx_buffer, strlen(tx_buffer));

	if ( tx_bytes < 0)
	{
		printf("UART TX error\n");
	}

	printf ("Sended: %s, count = %d\n", tx_buffer,  tx_bytes);

	return 	tx_bytes;
}

int rx_uart (int fd, char *rx_buffer, int num_byte_receive_expected)
{
	int offset = 0;

	do {
		int rx_length = read(fd, (void*)rx_buffer, sizeof(rx_buffer));
		offset += rx_length;
		if (rx_length < 0)
		{
			printf ("Receive Error,  error = %s\n", strerror(errno));
		
		}
		else if (rx_length == 0)
		{
			printf ("No data\n");
		}
		else
		{
			rx_buffer[rx_length] = '\0';
			//printf("%i bytes read : %s\n", rx_length, rx_buffer);
			int i;
			for (i = 0; i < rx_length; i++)
			{
				printf ("%d ", rx_buffer[i]);
			}
		}
	}while (offset < num_byte_receive_expected);
	
	printf ("\n");
	return offset;
}

void receive_uart_thread(int *pfd)
{
	char *rx_buffer;
	rx_buffer = (char*)malloc(sizeof(char)*256);
	
	while (1)
	{
		rx_uart (*pfd, rx_buffer, 6);
		fflush (stdout) ;
	}
}

int main(int argc, char *argv[])
{
	int uart0_filestream = -1;
	pthread_t rx_thread;

	uart0_filestream = open_uart ("/dev/ttyAMA0", 9600);
	
	if (uart0_filestream == -1)
	{
		printf("Error - Unable to open UART\n");
	}
	
	//----- Uart receive -----
	if (uart0_filestream != -1)
	{
		int *pfd;
		pfd = (int*)malloc(sizeof(int));
		*pfd = uart0_filestream;
		pthread_create( &rx_thread, NULL, &receive_uart_thread, pfd);
	}

	//----- Uart Transmit -----
	/*if (uart0_filestream != -1)
	{
		while(1)
		{
			tx_uart (uart0_filestream, argv[1]);
			usleep(1000000);
		}
		//tx_uart (uart0_filestream, argv[1]);
	}*/
	
	while(1);
	close(uart0_filestream);

}
