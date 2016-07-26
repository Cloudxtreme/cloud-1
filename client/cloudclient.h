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

#ifndef __CLOUD_CLIENT_H__
#define __CLOUD_CLIENT_H__

#include <stdbool.h>


/*
 * Cloud client initialization
 */
void cloud_client_init(void);

/**
 * Setting log file path
 * @filename: path to log file
 *
 * returns false: if fail filename length and call error_cb
 * returns true: if ok
 */
bool cloud_client_set_log(const char *filename);

/**
 * Loading cloud client configs JSON file
 * @filename: path to configs file
 *
 * returns false: if fail loading file and call error_cb
 * returns true: if ok
 */
bool cloud_client_load_cfg(const char *filename);

/**
 * Checking login information
 * @username: login of user
 * @passwd: password of user
 *
 * returns false: if login fail and call error_cb
 * returns true: if login ok
 */
bool cloud_client_login(const char *username, const char *passwd);

/*
 * Starting all threads of cloud client
 */
void cloud_client_start(void);

/*
 * Free memory
 */
void cloud_client_free(void);


/*
 * Cloud client callbacks
 * If not used, all signals are NULL
 */

/**
 * Set error callback for all modules
 * @error: error function pointer
 * @data: data pointer for callback
 */
void cloud_client_set_error_cb(void (*error)(const char*, void*), void *data);

/**
 * Set update status callback
 * @update_status: status function pointer
 * @data: data pointer for callback
 */
void cloud_client_set_update_status_cb(void (*update_status)(bool, void*), void *data);


#endif