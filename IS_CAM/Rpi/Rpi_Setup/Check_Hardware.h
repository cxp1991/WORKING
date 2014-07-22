#ifndef _CHECK_HARDWARE_H
#define _CHECK_HARDWARE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Raspberry pi network connection mode */
#define	_ETHERNET 0
#define _WIFI 1
#define _3G 2

/* State of components */
#define STATE_GOOD 3
#define STATE_BROKEN 4
#define SUCCESS 5
#define FAILED 6

/* uart */
typedef struct __rpi_uart
{
	int uart_fd;
}Rpi_Uart;

/* Camera */
typedef struct __rpi_camera_info
{
	int state;
	int width;
	int height;
	int bitrate;
	int framerate;
	//[...]

}Rpi_Camera;

/*Audio */
typedef struct  __rpi_usb_audio_adapter
{
	int state;
	int mic_sample_rate;
	int speaker_sample_rate;
	//[..]

}Rpi_Usb_Audio_Adapter;

/* Network connection */
typedef struct __rpi_network_component
{
	int state;
	int mode_id;
	int upload_speed;
	int download_speed;
	//[...]

}Rpi_Network_Component;

typedef struct __rpi_network_info
{
	Rpi_Network_Component component[3]; // Ethernet/Wifi/3G
	int working_mode;
}Rpi_Network;


/* Rpi Hardware */
typedef struct __rpi_hardware_info 
{
	Rpi_Camera camera;
	Rpi_Usb_Audio_Adapter audio_adapter;
	Rpi_Network network;
	Rpi_Uart uart;
	//[...]

}Rpi_Hardware;

Rpi_Hardware rpi_hardware;

/* Setup function */
int check_camera ();
void setup_camera ();

/* Networking */
int find_network_download_average_speed(int net_mode);
int find_network_upload_average_speed (int net_mode);
int find_network_speed();
int check_ethernet();
int check_wifi_dongle ();
int check_3G_dongle();
void check_network_connection();
int setup_network();

/* Audio */
int check_audio ();
void setup_audio ();

/* Uart */
void uart_init();

//[...]

/* Rpi */
void Rpi_setup ();

#endif
