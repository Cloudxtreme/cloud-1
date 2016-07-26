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
#include <stdint.h>
#include <string.h>

enum {
	FIRST_LOGIN = 1,
	REGULAR_LOGIN = 0
};

enum {
	LOGIN_OK,
	LOGIN_FAIL
};

struct login_data {
	uint8_t first;
	unsigned id;
	char login[100];
	char passwd_hash[129];	
};

struct login_answ {
	unsigned code;
};

static struct {
	pthread_mutex_t mutex;
	struct tcp_server server;
} cloud;


static void new_session(struct tcp_client *s_client, void *data)
{
	struct login_data ldata;
	struct login_answ lansw;
	
	if (!tcp_client_recv(s_client, (void *)&ldata, sizeof(ldata))) {
		pthread_mutex_lock(&cloud.mutex);
		log_local("Bad client connected.", LOG_ERROR);
		puts("Client disconnected.");
		pthread_mutex_unlock(&cloud.mutex);
		return;
	}
	pthread_mutex_lock(&cloud.mutex);
	printf("Client ID:%d connected.\n%s", ldata.id, "Login...");
	pthread_mutex_unlock(&cloud.mutex);
	
	//Checking login and password
	lansw.code = LOGIN_OK;

	if (!tcp_client_send(s_client, (const void *)&lansw, sizeof(lansw))) {
		pthread_mutex_lock(&cloud.mutex);
		printf("%s\n", "FAIL.");
		log_local("Fail sending login answare.", LOG_ERROR);
		puts("Client disconnected.");
		pthread_mutex_unlock(&cloud.mutex);
		return;
	}
	pthread_mutex_lock(&cloud.mutex);
	printf("%s\n", "OK.");
	pthread_mutex_unlock(&cloud.mutex);

	if (ldata.first == FIRST_LOGIN) {
		pthread_mutex_lock(&cloud.mutex);
		puts("Client disconnected.");
		pthread_mutex_unlock(&cloud.mutex);
		return;
	}
	if (ldata.first != REGULAR_LOGIN) {
		pthread_mutex_lock(&cloud.mutex);
		log_local("Bad login data.", LOG_ERROR);
		puts("Client disconnected.");
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
	if (!tcp_server_bind(&cloud.server, sc->port, sc->max_clients)) {
		log_local("Fail binding tcp server.", LOG_ERROR);
		return false;
	}
	pthread_mutex_destroy(&cloud.mutex);
	return true;
}

bool cloud_server_set_log(const char *filename)
{
	return log_set_path(filename);
}

uint8_t cloud_server_load_cfg(const char *filename)
{
	return configs_load(filename);
}