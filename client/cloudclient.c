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


static struct cloud_client {
	char username[100];
	char passwd[200];
	pthread_mutex_t mutex;
} cloud;


void cloud_client_init(void)
{
	pthread_mutex_init(&cloud.mutex, NULL);
	checker_life_init();
}

bool cloud_client_load_cfg(const char *filename)
{
	if (!configs_load(filename)) {
		log_local("Fail loading configs.", LOG_ERROR);
		return false;
	}
	return true;
}

bool cloud_client_set_log(const char *filename)
{
	return log_set_path(filename);
}

bool cloud_client_login(const char *username, const char *passwd)
{
	if (strlen(username) > 99 || strlen(passwd) > 99) {
		log_local("Login: Username or Passord is to long.", LOG_ERROR);
		return false;
	}

	if (!strcmp(username, "") || !strcmp(passwd, ""))
		return false;

	strncpy(cloud.username, username, 99);
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