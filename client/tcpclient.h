/* File Cloud project
 *
 * Copyright (C) 2016 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence 3
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 */

#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#define SOCKET int


struct tcp_client {
    /* Socket id */
    SOCKET s;
};


/**
 * Connect to other socket
 * @sock: socket struct
 * @ip: ip address of other socket
 * @port: tcp port of other socket
 *
 * Returns true if succeful connection
 * Returns false if fail connection
 */
bool tcp_client_connect(struct tcp_client *restrict sock, const char *ip, const unsigned port);

/**
 * Send data to other socket
 * @socket: socket struct
 * @data: sending data
 * @len: size of sendign data
 *
 * Returns true if succeful sending
 * Returns false if fail sending
 */
bool tcp_client_send(struct tcp_client *restrict sock, const void *data, const size_t len);

/**
 * Receive data from other socket
 * @socket: socket struct
 * @data: sending data
 * @len: size of receiving data
 *
 * Returns true if succeful receiving
 * Returns false if fail receiving
 */
bool tcp_client_recv(struct tcp_client *restrict sock, void *data, size_t len);

/*
 * Close connection
 */
void tcp_client_close(struct tcp_client *restrict sock);


#endif