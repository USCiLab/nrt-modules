/*
 *  SendUDP.h
 *  Acsend
 *
 *  Created by John Boiles on 7/2/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

//Initialize network connection
int SUDP_Init(const char* ipaddress, unsigned int port);

//Send a UDP packet
int SUDP_SendMsg(const char * data, int length);

// Recieve a packet on the sending socket
int SUDP_RecvMsg(const char * data, int length);

//Close the socket
int SUDP_Close();

// Test if the socket is open or not
int SUDP_IsOpen();

