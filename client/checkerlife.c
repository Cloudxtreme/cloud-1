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

#include "configs.h"
#include "checkerlife.h"
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>


static struct {
	bool status;
	pthread_t thread;

	void *update_data;
	void (*update_status)(bool, void*);

	void *err_data;
	void (*error)(const char*, uint8_t, void*);
} checker = {
	.status = false,
	.err_data = NULL,
	.error = NULL,
	.update_status = NULL,
	.update_data = NULL
};


static void checker_handle(void *data)
{
	bool status = false;
	pthread_mutex_t *mutex = (pthread_mutex_t *)data;

	if (checker.status != status) {
		checker.status = status;
		if (checker.update_status != NULL) {
			pthread_mutex_lock(mutex);
			checker.update_status(true, checker.update_data);
			pthread_mutex_unlock(mutex);
		}
	}
}

static void *checker_thread(void *data)
{
	struct life_checker_cfg *lcc = configs_get_life_checker();

	for (;;) {
		struct timeval tv = {lcc->interval, 0};
		if (select(0, NULL, NULL, NULL, &tv) == -1)
			if (EINTR == errno)
				continue;
		checker_handle(data);
	}
	return NULL;
}

void checker_life_start(pthread_mutex_t *mutex)
{
	pthread_create(&checker.thread, NULL, checker_thread, (void *)mutex);
	pthread_detach(checker.thread);
}

void checker_life_set_error_cb(void (*error_cb)(const char *message, uint8_t code, void *data), void *data)
{
	checker.error = error_cb;
	checker.err_data = data;
}

void checker_life_set_update_status_cb(void (*update_status_cb)(bool status, void *data), void *data)
{
	checker.update_status = update_status_cb;
	checker.update_data = data;
}