#include "login.h"
#include <stdio.h>

int connect_with_timeout(char *host,
			int port,
			int timeout_sec,
			int timeout_usec,
			char *account)
{
	  int 		res, valopt;
	  struct 	sockaddr_in addr;
	  long 		arg;
	  fd_set 	myset;
	  struct 	timeval tv;
	  socklen_t lon;
	  char 		buffer[181] = {0};
	  char 		sender[181] = {0};
	  char 		receiver[181] = {0};
	  int 		rc;

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
	  res = connect(  soc,
			  	  	  (struct sockaddr *)&addr,
			  	  	  sizeof(addr));

	  if (res < 0)
	  {
	     if (errno == EINPROGRESS)
	     {
	        tv.tv_sec = timeout_sec;
	        tv.tv_usec = timeout_usec;
	        FD_ZERO(&myset);
	        FD_SET(soc, &myset);

	        if (select(	soc+1,
						NULL, &myset,
						NULL, &tv) > 0)
	        {
	           lon = sizeof(int);
	           getsockopt( soc, SOL_SOCKET,
						   SO_ERROR, (void*)(&valopt),
						   &lon);

	           if (valopt)
	           {
	              return -1;
	           }
	        }
	        else
	        {
	           return -1;
	        }
	     }
	     else
	     {
	        return -1;
	     }
	  }

	  // Set to blocking mode again...
	  arg = fcntl(soc, F_GETFL, NULL);
	  arg &= (~O_NONBLOCK);
	  fcntl(soc, F_SETFL, arg);

	  Base64Encode(account, sender, BUFFFERLEN);
	  send(soc,sender,181, 0);
	  recv(soc, buffer, 181, 0);
	  Base64Decode(buffer, receiver, BUFFFERLEN);

	  if (strcmp(receiver,"Failed"))
	  {
		  return soc;
	  }
	  else
	  {
		  shutdown(soc,2);
		  return -2;
	  }
}


int login (char *username,
	   char *password)
{
	char info[200] = {0};
	int retValue;

	sprintf(info, "%s$%s", username, password);

	retValue = connect_with_timeout(SERVER, SERVER_PORT, 5, 0, info);
	printf("[login.c][login] retValue = %d", retValue);

	switch (retValue)
	{
		case -2:
			return 0; // wrong username/password

		case -1:
			return 1; // server down

		default:
			global_socket = retValue;
			return 2;
	}
}

