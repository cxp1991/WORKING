/*
 * login.h
 *
 *  Created on: Apr 4, 2014
 *      Author: cxphong
 */

#ifndef LOGIN_H_
#define LOGIN_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include "base64.h"

#define SERVER "66.219.103.59"
#define SERVER_PORT 9997

int global_socket;

int connect_with_timeout(char *host, int port, int timeout_sec,
			int timeout_usec, char *account);

int login (char* _username, char* _password);

#endif /* LOGIN_H_ */
