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
#include "filetransfer.h"
#include <sys/stat.h>
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

enum cmds {
	CMD_ADD_FILE,
	CMD_REMOVE_FILE,
	CMD_UPDATE_FILE,
	CMD_SESSION_EXIT
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

struct command {
	uint8_t code;
	char param1[255];
	char param2[255];
};

static struct {
	pthread_mutex_t mutex;
	struct tcp_server server;
} cloud;


static void exit_fail(const char *message, const char *filename)
{
	char msg[512];

	strcpy(msg, message);
	if (filename != NULL)
		strcat(msg, filename);

	pthread_mutex_lock(&cloud.mutex);
	log_local(msg, LOG_ERROR);
	puts("Client disconnected.");
	pthread_mutex_unlock(&cloud.mutex);
}

static void translate_cmd(struct login_data *ldata, struct tcp_client *s_client)
{
	uint8_t ret_val;
	struct command cmd;
	struct file_transfer ftransfer;
	struct storage_cfg *stc = configs_get_storage();

	file_transfer_init(&ftransfer, s_client);
	for (;;) {
		if (!tcp_client_recv(s_client, (void *)&cmd, sizeof(cmd))) {
			exit_fail("Fail receiving command.", NULL);
			return;
		}

		if (cmd.code == CMD_ADD_FILE) {
			char full_path[512];

			strcpy(full_path, stc->path);
			strcat(full_path, ldata->login);
			mkdir("storage", 0777);
			mkdir(full_path, 0777);
			strcat(full_path, "/");
			strcat(full_path, cmd.param1);

			ret_val = file_transfer_recv_file(&ftransfer, full_path);
			if (ret_val != FT_RECV_OK)
				exit_fail("Fail receiving file: ", cmd.param1);
			printf("New file was received: %s\n", cmd.param1);
		}

		if (cmd.code == CMD_UPDATE_FILE) {
			char full_path[512];

			strcpy(full_path, stc->path);
			strcat(full_path, ldata->login);
			strcat(full_path, "/");
			strcat(full_path, cmd.param1);

			ret_val = file_transfer_recv_file(&ftransfer, full_path);
			if (ret_val != FT_RECV_OK)
				exit_fail("Fail updating file: ", cmd.param1);
			printf("File was received: %s\n", cmd.param1);
		}

		if (cmd.code == CMD_REMOVE_FILE) {
			char full_path[512];

			strcpy(full_path, stc->path);
			strcat(full_path, ldata->login);
			strcat(full_path, "/");
			strcat(full_path, cmd.param1);

			remove(full_path);
			printf("File was removed: %s\n", cmd.param1);
		}

		if (cmd.code == CMD_SESSION_EXIT) {
			break;
		}
	}
}

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
	translate_cmd(&ldata, s_client);
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