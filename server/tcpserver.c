/* Meteo-zone 2 project
 *
 * Copyright (C) 2016 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 */

#include "tcpserver.h"
#include <stdlib.h>


struct sock_data {
    struct tcp_client *client;
    struct tcp_server *server;
};


static void *new_client(void *data)
{
    struct sock_data *sdata = (struct sock_data *)data;

    sdata->server->new_session(sdata->client, sdata->server->data);

    tcp_client_close(sdata->client);
    free(sdata->client);
    free(sdata);
    return NULL;
}


void tcp_server_init(struct tcp_server *restrict sock)
{
    sock->accept_error = NULL;
    sock->new_session = NULL;
}

void tcp_server_set_accepterr_cb(struct tcp_server *restrict sock, void (*accept_error)(void*), void *data)
{
    sock->accept_error = accept_error;
    sock->data = data;
}

void tcp_server_set_newsession_cb(struct tcp_server *restrict sock, void (*new_session)(struct tcp_client*, void*), void *data)
{
    sock->new_session = new_session;
    sock->data = data;
}

bool tcp_server_bind(struct tcp_server *sock, const unsigned short port, const unsigned max_clients)
{
    int ret_val;
    SOCKET s_client;
    struct sockaddr_in sock_addr;

    sock->s = socket(AF_INET, SOCK_STREAM, 0);
    if (sock->s == INVALID_SOCKET)
        return false;

    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port = htons(port);

    ret_val = bind(sock->s, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
    if (ret_val == SOCKET_ERROR)
        return false;

    ret_val = listen(sock->s, max_clients);
    if (ret_val == SOCKET_ERROR)
        return false;

    for (;;) {
        s_client = accept(sock->s, NULL, NULL);
        if (s_client == SOCKET_ERROR) {
            if (sock->accept_error != NULL)
                sock->accept_error(sock->data);
            continue;
        }

        if (sock->new_session == NULL)
            continue;

        pthread_t cl_th;
        struct tcp_client *client = (struct tcp_client *)malloc(sizeof(client));
        struct sock_data *sdata = (struct sock_data *)malloc(sizeof(sdata));
        sdata->client = client;
        sdata->server = sock;

        client->s = s_client;
        pthread_create(&cl_th, NULL, &new_client, (void *)sdata);
        pthread_detach(cl_th);        
    }
    return true;
}