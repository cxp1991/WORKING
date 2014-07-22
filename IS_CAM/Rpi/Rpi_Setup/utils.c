#include "utils.h"

void reboot ()
{
	system ("sudo shutdown -h now\n");
	printf ("System is rebooting!\n");
}

void restart_netwok_manager()
{
	system ("sudo service networking restart");
}
