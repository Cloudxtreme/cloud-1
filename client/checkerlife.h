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

#ifndef __CHECKER_LIFE_H__
#define __CHECKER_LIFE_H__

#include <pthread.h>


/**
 * Starting check life timer
 * @mutex: main thread locker
 */
void checker_life_start(pthread_mutex_t *mutex);


/*
 * Callbacks
 */

/**
 * Set error callback for checker life
 * @error_cb: error callback pointer
 *		@message: error message
 *		@code: error code
 *		@data: user data
 * @data: data pointer for callback
 */
void checker_life_set_error_cb(void (*error_cb)(const char *message, uint8_t code, void *data), void *data);

/**
 * Set update status callback
 * @update_status_cb: status callback pointer
 *		@status: server online status
 *		@data: user data
 * @data: user data pointer for callback
 */
void checker_life_set_update_status_cb(void (*update_status_cb)(bool status, void *data), void *data);


#endif