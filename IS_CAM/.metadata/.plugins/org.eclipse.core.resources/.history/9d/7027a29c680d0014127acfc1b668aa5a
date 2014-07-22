#include "connect_to_server.h"

int connect_with_timeout(char *host, int port, int timeout_sec,
		int timeout_usec, char *account)
{
	  int res, valopt;
	  struct sockaddr_in addr;
	  long arg;
	  fd_set myset;
	  struct timeval tv;
	  socklen_t lon;
	  char buffer[181] = {0};

	  /* Base64 */
	  char sender[181] = {0};
	  char receiver[181] = {0};
	  int rc;

	  // Create socket
	  int soc = socket(AF_INET, SOCK_STREAM, 0);

	  // Set non-blocking
	  arg = fcntl(soc, F_GETFL, NULL);
	  arg |= O_NONBLOCK;
	  fcntl(soc, F_SETFL, arg);

	  // Trying to connect with timeout
	  addr.sin_family = AF_INET;
	  addr.sin_port = htons(port);
	  addr.sin_addr.s_addr = inet_addr(host);
	  res = connect(soc, (struct sockaddr *)&addr, sizeof(addr));

	  if (res < 0) {
	     if (errno == EINPROGRESS) {
	        tv.tv_sec = timeout_sec;
	        tv.tv_usec = timeout_usec;
	        FD_ZERO(&myset);
	        FD_SET(soc, &myset);
	        if (select(soc+1, NULL, &myset, NULL, &tv) > 0) {
	           lon = sizeof(int);
	           getsockopt(soc, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
	           if (valopt) {
	              return -2;
	           }
	        }
	        else {
	           return -3;
	        }
	     }
	     else {
	        return -4;
	     }
	  }

	  // Set to blocking mode again...
	  arg = fcntl(soc, F_GETFL, NULL);
	  arg &= (~O_NONBLOCK);
	  fcntl(soc, F_SETFL, arg);

	  rc = Base64Encode(account, sender, BUFFFERLEN);
	  send(soc,sender,181,NULL);
	  recv(soc, buffer, 181, NULL);
	  rc = Base64Decode(buffer, receiver, BUFFFERLEN);

	  printf ("Connect to server: account= %s, buffer = %s\n", account, buffer);
	  if (strcmp(buffer,"Failed"))
	  {
		  return soc;
	  }
	  else
	  {
		shutdown(soc,2);
		return -5;
	  }
}

int connect_to_server()
{
	/*
	 * 1. Connect to server, timeout is 5s connect
	 *		If server down -> reconnect after 5s
	 * 2. Succeess, wait until receive request from Android client
	 */

	int ret;

	// Connect to server

	printf ("==========	CONNECT TO SERVER	========== \n");
	while(1)
	{
		ret = connect_with_timeout (SERVER, SERVER_PORT, 5, 0, "rpi001$6789");

		if (ret == -2 || ret == -3 || ret == -4)//timeout
		{
			printf("Connect failed - Cause by server's network state!, ret =%d\n", ret);
			sleep(5);
		}
		else if (ret == -5)
		{
			printf("Connect failed - Cause by wrong username or password!, ret =%d\n", ret);
			sleep(5);
		}
		else //sucess
		{
			global_socket = ret;
			printf ("Connect to server: ret = %d\n", global_socket);
			printf("Connect successful!\n");
			printf ("\n\n");
			break;
		}
	}
}

int wait_android_request()
{
	char *header, *origin, *dest;
	char buffer[181] = {0};
	char temp[181]={0};
	char temp1[181]={0};
	char receiver[181] = {0};
	int rc;

	memset(originBuf,'\0', sizeof(originBuf));
	memset(destBuf,'\0', sizeof(destBuf));

	printf ("==========	WAITTNG ANDROID REQUEST	========== \n");
	printf ("Waiting android request ...\n");

	/*
	 *  wait until receive android's request
	 * */
	if(recv(global_socket, buffer, 181, NULL))
	{
		printf("buffer  = %s\n",buffer);
		rc = Base64Decode(buffer, receiver, BUFFFERLEN);
		printf("receiver = %s\n",receiver);
		header = strtok (receiver,"$");
		origin = strtok (NULL,"$");
		dest = strtok (NULL,"$");
		strncpy(originBuf, origin, strlen(origin));
		strncpy(destBuf, dest, strlen(dest));

		printf ("\n\n");

		if (!strcmp(header,"001"))
		{
			return 1;
		}
		else
			return 0;
	}
	/*
	 * Server down
	 */
	else
	{
		printf ("Server Down!");
		return 2;
	}


}
