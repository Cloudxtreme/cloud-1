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
 * Setting error callbacl function
 * @error: error function pointer
 * @data: user data
 */
void checker_life_set_error_cb(void (*error)(const char *, uint8_t, void*), void *data);

/**
 * Setting update status callback function
 * @update_status: update status function pointer
 * @data: user data
 */
void checker_life_set_update_status_cb(void (*update_status)(bool, void*), void *data);


#endif