#include "Rpi_Setup/Check_Hardware.h"
#include "Connect_Server/connect_to_server.h"
#include "P2P_Stream/stream.h"
#include "stdio.h"

int main()
{
	Rpi_setup();
	connect_to_server();
	int ret = 0;

	while (1)
	{
		/* Wait android request */
		do{
			ret = wait_android_request();

			/* Server down -> connect again */
			if (ret == 2)
			{
				shutdown(global_socket, 2);
				connect_to_server();
			}

		}while(ret != 1);

		printf("Accept android's request!\n");
		printf("Start streaming!\n");

		stream();
	}

}
