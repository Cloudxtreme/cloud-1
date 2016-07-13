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
	pthread_t thread;
	void (*update_status)(bool);
} checker;


static void checker_handle(void *data)
{
	pthread_mutex_t *mutex = (pthread_mutex_t *)data;

	if (checker.update_status != NULL)
		checker.update_status(true);
	puts("test life");
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

void checker_life_init(void)
{
	checker.update_status = NULL;
}

void checker_life_start(pthread_mutex_t *mutex)
{
	pthread_create(&checker.thread, NULL, checker_thread, (void *)mutex);
	pthread_detach(checker.thread);
}