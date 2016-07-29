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

#ifndef __CHECKER_MAIN_H__
#define __CHECKER_MAIN_H__

#include <pthread.h>


struct user_login {
	char username[100];
	char passwd_hash[129];
};


/*
 * Setting user login
 */
void checker_main_set_login(struct user_login *user);

/**
 * Start main application timer
 * @mutex: main thread locker
 */
void checker_main_start(pthread_mutex_t *mutex);

/*
 * Callbacks
 */

/**
 * Setting error callback function
 * @error_cb: error callback pointer
 * 		@message: error message
 *		@code: error code
 *		@data: user data
 * @data: user data
 */
void checker_main_set_error_cb(void (*error_cb)(const char *message, uint8_t code, void *data), void *data);


#endif