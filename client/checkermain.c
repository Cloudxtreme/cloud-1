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

#include "log.h"
#include "configs.h"
#include "checkermain.h"
#include "tcpclient.h"
#include "sync.h"
#include "flist.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>


enum {
	FIRST_LOGIN = 1,
	REGULAR_LOGIN = 0
};

enum error_codes {
	ERR_CONNECT = 100,
	ERR_SEND_LOGIN = 101,
	ERR_RECV_LOGIN = 102,
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
	bool is_connected;
	pthread_t thread;
	struct user_login ulogin;
	struct tcp_client client;

	void *err_data;
	void (*error)(const char *, uint8_t, void*);
} checker = {
	.is_connected = false,
	.err_data = NULL,
	.error = NULL
};


static void exit_fail(const char *message, uint8_t code, pthread_mutex_t *mutex)
{
	tcp_client_close(&checker.client);
	pthread_mutex_lock(mutex);
	log_local(message, LOG_ERROR);
	if (checker.error != NULL)
		checker.error(message, code, checker.err_data);
	pthread_mutex_unlock(mutex);
}

static void checker_handle(void *data)
{
	struct login_data ldata;
	struct login_answ lansw;
	pthread_mutex_t *mutex = (pthread_mutex_t *)data;
	struct user_cfg *uc = (struct user_cfg *)configs_get_user();
	struct server_cfg *sc = (struct server_cfg *)configs_get_server();


	strncpy(ldata.login, checker.ulogin.username, 99);
	strncpy(ldata.passwd_hash, checker.ulogin.passwd_hash, 128);
	ldata.first = REGULAR_LOGIN;
	ldata.id = uc->id;

	/*
	 * Sending login data to server
	 */
	if (!tcp_client_connect(&checker.client, sc->ip, sc->port)) {
		pthread_mutex_lock(mutex);
		log_local("Fail connecting to login server!", LOG_ERROR);
		if (checker.error != NULL)
			checker.error("Fail connecting to login server!", ERR_CONNECT, checker.err_data);
		pthread_mutex_unlock(mutex);
		checker.is_connected = false;
		return;
	}
	if (!checker.is_connected) {
		checker.is_connected = true;
		puts("Connection successful!");
	}
	if (!tcp_client_send(&checker.client, (const void *)&ldata, sizeof(ldata))) {
		exit_fail("Fail sending login data.", ERR_SEND_LOGIN, mutex);
		return;
	}
	if (!tcp_client_recv(&checker.client, (void *)&lansw, sizeof(lansw))) {
		exit_fail("Fail receiving login answare.", ERR_RECV_LOGIN, mutex);
		return;
	}

	struct flist *files = sync_get_file_list("/home/serg/");

	for (struct flist *fs = files; fs != NULL; fs = flist_next(fs)) {
		struct file *cur_file = flist_get_file(fs);
		puts(cur_file->name);
	}
	flist_free_all(files);

	tcp_client_close(&checker.client);
}

static void *checker_thread(void *data)
{
	struct main_checker_cfg *mcc = configs_get_main_checker();

	for (;;) {
		struct timeval tv = {mcc->interval, 0};
		if (select(0, NULL, NULL, NULL, &tv) == -1)
			if (EINTR == errno)
				continue;
		checker_handle(data);
	}
	return NULL;
}


void checker_main_start(pthread_mutex_t *mutex)
{
	pthread_create(&checker.thread, NULL, checker_thread, (void *)mutex);
	pthread_detach(checker.thread);
}

void checker_main_set_login(struct user_login *user)
{
	strncpy(checker.ulogin.username, user->username, 99);
	strncpy(checker.ulogin.passwd_hash, user->passwd_hash, 128);
}

void checker_main_set_error_cb(void (*error)(const char *, uint8_t, void*), void *data)
{
	checker.error = error;
	checker.err_data = data;
}