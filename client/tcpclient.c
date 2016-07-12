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

#include "tcpclient.h"
#include <stdlib.h>


bool tcp_client_connect(struct tcp_client *restrict sock, const char *ip, const unsigned port)
{
    int ret_val;
    struct sockaddr_in sock_addr;

    memset(&sock_addr, 0x00, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr(ip);
    sock_addr.sin_port = htons(port);

    sock->s = socket(AF_INET, SOCK_STREAM, 0);
    if (sock->s == INVALID_SOCKET)
        return false;

    ret_val = connect(sock->s, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
    if (ret_val == SOCKET_ERROR)
        return false;
    return true;
}

bool tcp_client_send(struct tcp_client *restrict sock, const void *data, const size_t len)
{
    int ret_val = 0;

    for (;;) {
        ret_val = send(sock->s, data, len, MSG_NOSIGNAL);
        if (ret_val == SOCKET_ERROR)
            return false;

        if (ret_val == (int)len)
            break;
    }
    return true;
}

bool tcp_client_recv(struct tcp_client *restrict sock, void *data, size_t len)
{
    size_t bytes;

    while (len) {
        bytes = recv(sock->s, data, len, MSG_NOSIGNAL|MSG_WAITALL);
        if (bytes == len)
            break;

        if ((bytes == 0) || (bytes == SOCKET_ERROR))
            return false;

        data += bytes;
        len -= bytes;
    }
    return true;
}

void tcp_client_close(struct tcp_client *restrict sock)
{
    if (sock->s != INVALID_SOCKET) {
        shutdown(sock->s, 1);
        close(sock->s);
    }
}