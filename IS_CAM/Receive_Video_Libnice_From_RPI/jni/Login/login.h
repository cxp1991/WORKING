/*
 * login.h
 *
 *  Created on: Apr 4, 2014
 *      Author: cxphong
 */

#ifndef LOGIN_H_
#define LOGIN_H_

#include <jni.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <android/log.h>
#include "../utils/base64.h"

#define SERVER "112.213.86.238"
#define SERVER_PORT 5000

int global_socket;

int connect_with_timeout(char *host, int port, int timeout_sec,
							int timeout_usec, char *account);

int login_to_server (JNIEnv *env, jobject thiz,
						jstring _username, jstring _password);

jstring list_online_client(JNIEnv *env, jobject thiz,
							jstring _username);

#endif /* LOGIN_H_ */
