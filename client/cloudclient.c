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

#include "cloudclient.h"
#include <pthread.h>
#include <string.h>
#include "configs.h"
#include "log.h"
#include "checkermain.h"
#include "checkerlife.h"
#include "tcpclient.h"
#include "localbase.h"
#include <stdint.h>
#include <stdio.h>
#include <openssl/sha.h>


enum {
	FIRST_LOGIN = 1,
	REGULAR_LOGIN = 0
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


static struct cloud_client {
	char username[100];
	char passwd_hash[129];

	pthread_mutex_t mutex;
	struct tcp_client login_client;
} cloud;


static void convert_hash(const unsigned char *hash, char *out)
{
	char sym[3];

	for (size_t i = 0; i < SHA512_DIGEST_LENGTH; i++, hash++, out += 2) {
		sprintf(sym, "%02X", *hash);
		*out = sym[0];
		*(out + 1) = sym[1];
	}
	*(out + 1) = '\0';
}

void cloud_client_init(void)
{
	pthread_mutex_init(&cloud.mutex, NULL);
}

uint8_t cloud_client_load_cfg(const char *filename)
{
	return configs_load(filename);
}

bool cloud_client_set_log(const char *filename)
{
	return log_set_path(filename);
}

uint8_t cloud_client_login(const char *username, const char *passwd)
{
	int ret_val;
	SHA512_CTX sha_ctx;
	struct login_data ldata;
	struct login_answ lansw;
	struct user_login ulogin;
	unsigned char hash[SHA512_DIGEST_LENGTH];
	struct server_cfg *sc = (struct server_cfg *)configs_get_server();
	struct user_cfg *uc = (struct user_cfg *)configs_get_user();

	if (strlen(username) > 99 || strlen(passwd) > 99) {
		log_local("Login: Username or Password is to long.", LOG_ERROR);
		return LOGIN_LONG;
	}

	if (!strcmp(username, "") || !strcmp(passwd, "")) {
		log_local("Login: Username or Paaword is empty.", LOG_ERROR);
		return LOGIN_EMPTY;
	}	
	/*
	 * Generating sha512 hash passwd
	 */
	ret_val = SHA512_Init(&sha_ctx);
	if (!ret_val) {
		log_local("Login: SHA512 init fail.", LOG_ERROR);
		return LOGIN_SHA_INIT_ERR;
	}
	/*
	 * Generating password hash
	 */
	ret_val = SHA512_Update(&sha_ctx, (const void *)passwd, strlen(passwd));
	if (!ret_val) {
		log_local("Login: Fail update passwd hash.", LOG_ERROR);
		return LOGIN_SHA_UPDATE_ERR;
	}
	ret_val = SHA512_Final(hash, &sha_ctx);
    if (!ret_val) {
    	log_local("Login: Fail finalization of sha512.", LOG_ERROR);
    	return LOGIN_SHA_FINAL_ERR;
    }

    strncpy(ulogin.username, username, 99);
    convert_hash(hash, ulogin.passwd_hash);
    checker_main_set_login(&ulogin);

    strncpy(ldata.login, ulogin.username, 99);
	strncpy(ldata.passwd_hash, ulogin.passwd_hash, 128);
	ldata.first = FIRST_LOGIN;
	ldata.id = uc->id;
	/*
	 * Sending login data to server
	 */
	if (!tcp_client_connect(&cloud.login_client, sc->ip, sc->port)) {
		log_local("Fail connecting to login server!", LOG_ERROR);
		return LOGIN_CONNECTION_ERR;
	}
	if (!tcp_client_send(&cloud.login_client, (const void *)&ldata, sizeof(ldata))) {
		tcp_client_close(&cloud.login_client);
		log_local("Fail sending login data.", LOG_ERROR);
		return LOGIN_SEND_ERR;
	}
	if (!tcp_client_recv(&cloud.login_client, (void *)&lansw, sizeof(lansw))) {
		tcp_client_close(&cloud.login_client);
		log_local("Fail sreceiving login answare.", LOG_ERROR);
		return LOGIN_ANSW_ERR;
	}
	tcp_client_close(&cloud.login_client);

	/*
	 * Checking login answare
	 */
	if (lansw.code != LOGIN_OK) {
		log_local("Fail authentication.", LOG_ERROR);
		return LOGIN_FAIL;
	}
	return LOGIN_OK;
}

void cloud_client_start(void)
{
	checker_main_start(&cloud.mutex);
	checker_life_start(&cloud.mutex);
}

void cloud_client_free(void)
{
	pthread_mutex_destroy(&cloud.mutex);
}

bool cloud_client_clean_base(void)
{
	bool ret_val;
	struct db_cfg *dbc = configs_get_database();

	if (!local_base_open(dbc->lb_path))
		return false;

	ret_val = local_base_clean();
	local_base_close();
	return ret_val;
}

/*
 * Cloud Callbacks
 */

void cloud_client_set_error_cb(void (*error_cb)(const char *message, uint8_t code, void *data), void *data)
{
	checker_main_set_error_cb(error_cb, data);
	checker_life_set_error_cb(error_cb, data);
}

void cloud_client_set_update_status_cb(void (*update_status_cb)(bool status, void *data), void *data)
{
	checker_life_set_update_status_cb(update_status_cb, data);
}