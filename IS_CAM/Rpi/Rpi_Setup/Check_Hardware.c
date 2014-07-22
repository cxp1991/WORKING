#include "Check_Hardware.h"
#include "utils.h"

/*
 *	Check Ethernet
 *	Ethernet doesn't work in 2 cases:
 *		1. Doesn't have an IP address
 *		2. Internet down
 */

int check_ethernet()
{
	int ret;
	int count = 0;
	int i;

	printf ("Checking Ethernet ...\n");

	/* Request DHCP again */
	//system("sudo dhclient eth0");

	/* Does Internet work? */
	for (i = 0; i < 4; i++)
	{
		ret = system("ping -I eth0 8.8.8.8 -c 1 1>/dev/null");

		if (!ret)
			count++;
	}

	if (count > 0)
	{
		printf("Ethernet is avalable!\n");
		return STATE_GOOD;
	}

	printf("Ethernet is not avalable!\n");
	return STATE_BROKEN;
}

int check_wifi_dongle ()
{
		int ret;
		int count = 0;
		int i;

		printf ("Checking Wifi ...\n");

		/* Request DHCP again */
		//system("sudo dhclient wlan0");

		/* Does Internet work? */
		for (i = 0; i < 4; i++)
		{
			ret = system("ping -I wlan0 8.8.8.8 -c 1 1>/dev/null");

			if (!ret)
				count++;
		}

		if (count > 0)
		{
			printf("Wifi is avalable!\n");
			return STATE_GOOD;
		}

		printf("Wifi is not avalable!\n");
		return STATE_BROKEN;
}

int check_3G_dongle ()
{
	int ret;
	int count = 0;
	int i;

	printf ("Setup 3G ...\n");
	WEXITSTATUS(system("/home/pi/Rpi_System/Scripts/Enable_3G.sh 1>/dev/null"));
	printf ("Checking 3G ...\n");

	/* Request DHCP again */
	//system("sudo dhclient wlan0");

	/* Does Internet work? */
	for (i = 0; i < 4; i++)
	{
		ret = system("ping -I ppp0 8.8.8.8 -c 1 1>/dev/null");

		if (!ret)
			count++;
	}

	if (count > 0)
	{
		printf("3G is avalable!\n");
		return STATE_GOOD;
	}

	printf("3G is not avalable!\n");
	return STATE_BROKEN;
}

/*
 * Check if Ethernet/Wifi/3G can work.
 */
void check_network_connection()
{
	if ((rpi_hardware.network.component[0].state = check_ethernet()) == STATE_GOOD)
		return;

	if ((rpi_hardware.network.component[1].state = check_wifi_dongle()) == STATE_GOOD)
		return;

	if ((rpi_hardware.network.component[2].state = check_3G_dongle()) == STATE_BROKEN)
		reboot();
}

int setup_network()
{
	rpi_hardware.network.component[0].mode_id = _ETHERNET;
	rpi_hardware.network.component[1].mode_id = _WIFI;
	rpi_hardware.network.component[2].mode_id = _3G;

	check_network_connection();

//	printf("%d\n,%d\n%d\n", rpi_hardware.network.component[0].state,
//		rpi_hardware.network.component[1].state,
//		rpi_hardware.network.component[2].state);

	/* All 3 interfaces downed, we should reboot the board */
//	if (rpi_hardware.network.component[0].state == STATE_BROKEN &&
//				rpi_hardware.network.component[1].state == STATE_BROKEN &&
//				rpi_hardware.network.component[2].state == STATE_BROKEN)
//		reboot();

	// set working mode
	// Route network again
	// Stop the others

}

/*
 * We check camera by using raspistill util(using to capture a jpeg photo).
 *
 * Return:
 * 		0: Success
 * 		Different: Failed
 *
 * */

int check_camera ()
{
	int ret;

	/* Capture photo */
	ret = system("sudo raspistill -t 10 -o test.jpg");

	/* Remove photo */
	system("rm -rf test.jpg");

	if (!ret)
	{
		printf("Camera is avalable!\n");
		return STATE_GOOD;
	}
	else
	{
		printf("Camera is not available\n");
		return STATE_BROKEN;
	}
}

/*
 * 	If camera is not available, reboot Rpi makes no sense, problem almost causes by
 * 	camera's connector was not correct or camera was broken.
 *
 *	Solve:
 * 		We send camera's info to server, so client can check their
 * 		board by hand.
 *
 * */

void setup_camera ()
{
	rpi_hardware.camera.state = check_camera ();
	rpi_hardware.camera.width = 800;
	rpi_hardware.camera.height = 600;
	rpi_hardware.camera.bitrate = 500000;
	rpi_hardware.camera.framerate = 25;

}

/* Check if first sound card is USB adapter */
int check_audio ()
{
	int ret;

	ret = WEXITSTATUS(system("/home/pi/Rpi_System/Scripts/Test_Usb_Audio.sh 1>/dev/null"));

	if (ret == 10)
	{
		printf("Audio adapter is available!\n");
		return STATE_GOOD;
	}
	else
	{
		printf("Audio adapter is not available!\n");
		return STATE_BROKEN;
	}

}

/*
 * 	Check audio adapter fails in 2 cases:
 * 		1. Card was not plug or even it was broken
 * 		2. Card wasn't set as default instead of alsa was set as defalt
 *
 * 	Solve:
 * 		1. Run script setup sound card again. This script contains
 * 		"lsusb" command.
 * 		2. Check sound card again
 *
 * 		If after implementing 2 above steps but fail ->
 * 		send this info to server to notify to user then they can check
 * 		sound card again.
 *
 * */

void setup_audio ()
{
	rpi_hardware.audio_adapter.state = check_audio();
	rpi_hardware.audio_adapter.mic_sample_rate = 44100;
	rpi_hardware.audio_adapter.speaker_sample_rate = 16000;
}

void uart_init ()
{
	int uart0_filestream = -1;

	uart0_filestream = open_uart ("/dev/ttyAMA0", 9600);

	if (uart0_filestream == -1)
	{
		printf("Error - Unable to open UART\n");
	}

	rpi_hardware.uart.uart_fd = uart0_filestream;
	startUartReceiveThread();
	printf("UART is available!\n");
}

void Rpi_setup ()
{
	printf ("==========	CHECKING HARDWARE	========== \n");
	/* Network */
	setup_network();
	setup_camera();
	setup_audio();
	uart_init();

	printf ("\n\n");
}

