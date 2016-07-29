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
#include "localbase.h"
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
	ERR_LSQL_OPEN = 103,
	ERR_LSQL_SELECT = 104,
	ERR_LSQL_INSERT = 105,
	ERR_LSQL_UPDATE = 106,
	ERR_LSQL_REMOVE = 107,
	ERR_LSQL_CLEAN = 108
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
	void (*error)(const char*, uint8_t, void*);
} checker = {
	.is_connected = false,
	.err_data = NULL,
	.error = NULL
};


static void call_error(const char *message, uint8_t code, pthread_mutex_t *mutex)
{
	pthread_mutex_lock(mutex);
	log_local(message, LOG_ERROR);
	if (checker.error != NULL)
		checker.error(message, code, checker.err_data);
	pthread_mutex_unlock(mutex);
}

static void checker_handle(void *data)
{
	uint8_t ret_val;
	char full_path[512];
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
		call_error("Fail connecting to login server!", ERR_CONNECT, mutex);
		checker.is_connected = false;
		return;
	}
	if (!checker.is_connected) {
		checker.is_connected = true;
		puts("Connection successful!");
	}
	if (!tcp_client_send(&checker.client, (const void *)&ldata, sizeof(ldata))) {
		call_error("Fail sending login data.", ERR_SEND_LOGIN, mutex);
		tcp_client_close(&checker.client);
		return;
	}
	if (!tcp_client_recv(&checker.client, (void *)&lansw, sizeof(lansw))) {
		call_error("Fail receiving login answare.", ERR_RECV_LOGIN, mutex);
		tcp_client_close(&checker.client);
		return;
	}
	/*
	 * Sync files
	 */
	struct db_cfg *dbc = configs_get_database();

	if (!local_base_open(dbc->lb_path)) {
		call_error("Can not open local database.", ERR_LSQL_OPEN, mutex);
		tcp_client_close(&checker.client);
	}

	struct flist *files = sync_get_file_list(uc->path);

	for (struct flist *fs = files; fs != NULL; fs = flist_next(fs)) {
		struct file *cur_file = flist_get_file(fs);
		strcpy(full_path, uc->path);
		strcat(full_path, cur_file->name);

		/*
		 * Search file in base
		 */
		ret_val = local_base_check_file_exists(cur_file);
		if (ret_val == LB_SQL_ERROR) {
			call_error("Error sql select query.", ERR_LSQL_SELECT, mutex);
			continue;
		}
		if (ret_val == LB_FILE_NOT_FOUND) {
			if (!local_base_add_file(cur_file)) {
				call_error("Error sql insert query.", ERR_LSQL_INSERT, mutex);
				continue;
			}
			log_sync("New file added", cur_file->name);
			//remote base add file
			//send file to server
			continue;
		}
		/*
		 * If file exists check it
		 */
		ret_val = local_base_compare_file_stat(cur_file);
		if (ret_val == LB_SQL_ERROR) {
			call_error("Error compare files sql select query", ERR_LSQL_SELECT, mutex);
			continue;
		}
		if (ret_val == LB_STAT_MATCH)
			continue;

		ret_val = local_base_update_file(cur_file);
		if (ret_val == LB_SQL_ERROR) {
			call_error("Error update file sql query.", ERR_LSQL_UPDATE, mutex);
			continue;
		}

		log_sync("File was changed", cur_file->name);
		//remote base update hash date
		//send file
	}
	/*
	 * Removing files as needed
	 */
	struct flist *del_files = local_base_find_deleted(files);
	if (del_files == NULL) {
		flist_free_all(files);
		local_base_close();
		tcp_client_close(&checker.client);
		return;
	}

	for (struct flist *l = del_files; l != NULL; l = flist_next(l)) {
		struct file *f = flist_get_file(l);
		if (!local_base_remove(f))
			call_error("Can not remove file from base", ERR_LSQL_REMOVE, mutex);
		//Delete from remote base
		//Delete file from remote storage
		log_sync("File was deleted", f->name);
	}

	flist_free_all(del_files);
	flist_free_all(files);
	local_base_close();
	tcp_client_close(&checker.client);
}

static void *checker_thread(void *data)
{
	struct main_checker_cfg *mcc = configs_get_main_checker();

	for (;;) {
		struct timeval tv = {mcc->interval, 0};
		checker_handle(data);

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

void checker_main_set_error_cb(void (*error_cb)(const char *message, uint8_t code, void *data), void *data)
{
	checker.error = error_cb;
	checker.err_data = data;
}