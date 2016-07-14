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

#include "cloudserver.h"
#include <pthread.h>
#include "tcpserver.h"
#include "configs.h"
#include "log.h"
#include <stdio.h>
#include <string.h>


static struct {
	pthread_mutex_t mutex;
	struct tcp_server server;
} cloud;


static void new_session(struct tcp_client *s_client, void *data)
{
	struct login_data ldata;

	if (!tcp_client_recv(s_client, (void *)&ldata, sizeof(ldata))) {
		pthread_mutex_lock(&cloud.mutex);
		log_local("Fail receiving login data.", LOG_ERROR);
		pthread_mutex_unlock(&cloud.mutex);
		return;
	}
}

bool cloud_server_start()
{
	struct server_cfg *sc = configs_get_server();

	puts("Starting server...");
	pthread_mutex_init(&cloud.mutex, NULL);

	tcp_server_set_newsession_cb(&cloud.server, new_session, NULL);
	if (!tcp_server_bind(&cloud.server, sc->port, sc->max_users)) {
		log_local("Fail binding tcp server.", LOG_ERROR);
		return false;
	}
	pthread_mutex_destroy(&cloud.mutex);
	return true;
}