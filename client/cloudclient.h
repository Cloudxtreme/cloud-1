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

#include <stdint.h>
#include <stdbool.h>

enum configs_codes {
	CFG_OK,
	CFG_FILE_NOT_FOUND,
	CFG_MCC_INTERVAL_ERROR,
	CFG_LCC_INTERVAL_ERROR,
	CFG_SC_IP_ERROR,
	CFG_SC_PORT_ERROR
};

enum login_codes {
	LOGIN_OK,
	LOGIN_FAIL,
	LOGIN_LONG,
	LOGIN_EMPTY,
	LOGIN_SHA_INIT_ERR,
	LOGIN_SHA_UPDATE_ERR,
	LOGIN_SHA_FINAL_ERR,
	LOGIN_CONNECTION_ERR,
	LOGIN_SEND_ERR,
	LOGIN_ANSW_ERR
};

enum error_codes {
	ERR_CONNECT = 100,
	ERR_SEND_LOGIN = 101,
	ERR_RECV_LOGIN = 102,
};

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
 * returns configs_codes: if fail loading file
 * returns CFG_OK: if loading configs ok
 */
uint8_t cloud_client_load_cfg(const char *filename);

/**
 * Checking login information
 * @username: login of user
 * @passwd: password of user
 *
 * returns login_codes: if login fail
 * returns LOGIN_OK: if login ok
 */
uint8_t cloud_client_login(const char *username, const char *passwd);

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
void cloud_client_set_error_cb(void (*error)(const char*, uint8_t, void*), void *data);

/**
 * Set update status callback
 * @update_status: status function pointer
 * @data: data pointer for callback
 */
void cloud_client_set_update_status_cb(void (*update_status)(bool, void*), void *data);


#endif