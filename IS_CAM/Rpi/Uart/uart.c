#include "uart.h"


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

  // Get and modify current options
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

int tx_uart (int fd, const char *tx_buffer, int len)
{
	int tx_bytes;
	tx_bytes = write(fd, (void*) tx_buffer, len);

	if ( tx_bytes < 0)
	{
		printf("UART TX error\n");
	}

	//printf ("Sended: %s, count = %d\n", tx_buffer,  tx_bytes);

	int i;
	printf ("Sended: ");
	for (i = 0; i < tx_bytes; i++)
	{
		printf ("%d ", tx_buffer[i]);
	}
	printf ("\n");

	return 	tx_bytes;
}

char* rx_uart (int fd, char *rx_buffer, int num_byte_receive_expected)
{
	int offset = 0;
	char *receiveBuf = (char *) malloc(6);
	memset(receiveBuf,0, 6);
	int mOffset = 0;

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
				//printf ("%d ", rx_buffer[i]);
				receiveBuf[mOffset++] = rx_buffer[i];
			}
			//printf("\n");
		}
	}while (offset < num_byte_receive_expected);

	printf ("\n");
	return receiveBuf;
}

static void receive_uart_thread(int *pfd)
{
	char *rx_buffer;
	rx_buffer = (char*) malloc(sizeof(char)*256);
	int i;
	char *command;
	command = (char*) malloc(sizeof(char)*5);

	while ( !StopUartReceive )
	{
		char *resultBuf = rx_uart (*pfd, rx_buffer, 6);
//		int i = 0;
//		for (;i<6;i++)
//			printf("%d _ ", resultBuf[i]);

		//printf ("reseultBuf[2] = %d\n", resultBuf[2]);
		switch(resultBuf[2])
		{
		case 0x05:
			printf("Temperature = %d\n", resultBuf[3]);
			command[0] = 5;
			command[1] = resultBuf[3];
			command[2] = 0xff;
			command[3] = 0xff;
			command[4] = '\0';
			nice_agent_send ( RpiData_Text->agent, RpiData_Text->streamID, 1, sizeof(command), command);
			break;
		default:
			break;
		}

		fflush (stdout);
	}
}


int startUartReceiveThread()
{
	StopUartReceive = 0;
	int *pfd;
	pfd = (int*)malloc(sizeof(int));
	*pfd = rpi_hardware.uart.uart_fd;
	pthread_create( &rx_thread, NULL, &receive_uart_thread, pfd);
}
