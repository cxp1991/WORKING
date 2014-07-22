#include "command.h"
#include "uart.h"

int control_servo (int fd, int servo_id, int angle)
{
	char command[6] = {0};
	int len = sizeof(command);
	
	/* Header */	
	command[0] = 0xA5;
	command[1] = 0x5A;

	/* Command ID */		
	command[2] = 0x3;

	/* servo ID */
	command[3] = servo_id; // 1, 2

	/* angle*/
	if (angle < 0)
		command[4] = '-';
	else 
		command[4] = '+';

	if (angle < 0 )
		angle *= (-1);
	command[5] = angle;

	int ret = tx_uart(fd, command, len);
	return ret;
}

int get_temperature (int fd)
{
	char command[6];
	int len = sizeof(command);
	
	/* Header */	
	command[0] = 0xA5;
	command[1] = 0x5A;

	/* Command ID */		
	command[2] = 0x5;

	command[3] = 0xFF;
	command[4] = 0xFF;
	command[5] = 0xFF;

	int ret = tx_uart(fd, command, len);
	return ret;
}

int control_piezosiren (int fd, int state)
{
	char command[6];
	int len = sizeof(command);

	/* Header */
	command[0] = 0xA5;
	command[1] = 0x5A;

	/* Command ID */
	command[2] = 0x1;

	command[3] = state;
	command[4] = 0xFF;
	command[5] = 0xFF;

	int ret = tx_uart(fd, command, len);
	return ret;
}

int control_pump (int fd, int state)
{
	char command[6];
	int len = sizeof(command);

	/* Header */
	command[0] = 0xA5;
	command[1] = 0x5A;

	/* Command ID */
	command[2] = 0x2;

	command[3] = state;
	command[4] = 0xFF;
	command[5] = 0xFF;

	int ret = tx_uart(fd, command, len);
	return ret;
}

