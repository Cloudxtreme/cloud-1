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
#include <stdint.h>
#include <stdio.h>
#include <openssl/sha.h>


enum {
	FIRST_LOGIN = 1,
	REGULAR_LOGIN = 0
};

enum {
	LOGIN_OK,
	LOGIN_FAIL,
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

	void *err_data;
	void (*error)(const char *message, void*);
} cloud = {
	.err_data = NULL,
	.error = NULL
};


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

bool cloud_client_load_cfg(const char *filename)
{
	if (!configs_load(filename)) {
		log_local("Fail loading configs.", LOG_ERROR);
		if (cloud.error != NULL)
			cloud.error("Fail loading configs.", cloud.err_data);
		return false;
	}
	return true;
}

bool cloud_client_set_log(const char *filename)
{
	if (!log_set_path(filename)) {
		if (cloud.error != NULL)
			cloud.error("Log path is to long.", cloud.err_data);
		return false;
	}
	return true;
}

bool cloud_client_login(const char *username, const char *passwd)
{
	int ret_val;
	SHA512_CTX sha_ctx;
	unsigned char hash[SHA512_DIGEST_LENGTH];
	struct login_data ldata;
	struct login_answ lansw;
	struct server_cfg *sc = (struct server_cfg *)configs_get_server();

	if (strlen(username) > 99 || strlen(passwd) > 99) {
		log_local("Login: Username or Password is to long.", LOG_ERROR);
		if (cloud.error != NULL)
			cloud.error("Login: Username or Password is to long.", cloud.err_data);
		return false;
	}

	if (!strcmp(username, "") || !strcmp(passwd, "")) {
		log_local("Login: Username or Paaword is empty.", LOG_ERROR);
		if (cloud.error != NULL)
			cloud.error("Login: Username or Password is empty.", cloud.err_data);
		return false;
	}	
	/*
	 * Generating sha512 hash passwd
	 */
	ret_val = SHA512_Init(&sha_ctx);
	if (!ret_val) {
		if (cloud.error != NULL)
			cloud.error("Login: SHA512 init fail.", cloud.err_data);
		return false;
	}
	/*
	 * Generating password hash
	 */
	ret_val = SHA512_Update(&sha_ctx, (const void *)passwd, strlen(passwd));
	if (!ret_val) {
		if (cloud.error != NULL)
			cloud.error("Login: Fail update passwd hash.", cloud.err_data);
		return false;
	}
	ret_val = SHA512_Final(hash, &sha_ctx);
    if (!ret_val) {
      	if (cloud.error != NULL) 
			cloud.error("Login: Fail finalization of sha512.", cloud.err_data);
		return false;
    }

    strncpy(cloud.username, username, 99);
    convert_hash(hash, cloud.passwd_hash);

	/*
	 * Sending login data to server
	 */
	 if (!tcp_client_connect(&cloud.login_client, sc->ip, sc->port)) {
		log_local("Fail connecting to login server!", LOG_ERROR);
		if (cloud.error != NULL)
			cloud.error("Fail connecting to login server!", cloud.err_data);
		return false;
	}

	strncpy(ldata.login, cloud.username, 99);
	strncpy(ldata.passwd_hash, cloud.passwd_hash, 128);
	ldata.first = FIRST_LOGIN;

	if (!tcp_client_send(&cloud.login_client, (const void *)&ldata, sizeof(ldata))) {
		tcp_client_close(&cloud.login_client);
		log_local("Fail sending login data.", LOG_ERROR);
		if (cloud.error != NULL)
			cloud.error("Fail sending login data.", cloud.err_data);
		return false;
	}
	if (!tcp_client_recv(&cloud.login_client, (void *)&lansw, sizeof(lansw))) {
		tcp_client_close(&cloud.login_client);
		log_local("Fail sreceiving login answare.", LOG_ERROR);
		if (cloud.error != NULL)
			cloud.error("Fail sending login data.", cloud.err_data);
		return false;
	}
	tcp_client_close(&cloud.login_client);

	/*
	 * Checking login answare
	 */
	if (lansw.code != LOGIN_OK) {
		log_local("Fail authentication.", LOG_ERROR);
		if (cloud.error != NULL)
			cloud.error("Fail authentication.", cloud.err_data);
		return false;
	}
	return true;
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


/*
 * Cloud Callbacks
 */

void cloud_client_set_error_cb(void (*error)(const char*, void*), void *data)
{
	cloud.err_data = data;
	cloud.error = error;
	checker_main_set_error_cb(error, data);
	checker_life_set_error_cb(error, data);
}

void cloud_client_set_update_status_cb(void (*update_status)(bool, void*), void *data)
{
	checker_life_set_update_status_cb(update_status, data);
}