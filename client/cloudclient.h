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

/*
 * Configs load returns codes
 */
enum {
    CFG_OK,
    CFG_FILE_NOT_FOUND,
    CFG_MCC_INTERVAL_ERROR,
    CFG_LCC_INTERVAL_ERROR,
    CFG_SC_IP_ERROR,
    CFG_SC_PORT_ERROR,
    CFG_UC_ID_ERROR,
    CFG_UC_PATH_ERROR,
    CFG_LB_PATH_ERROR
};

/**
 * Loading cloud client configs JSON file
 * @filename: path to configs file
 *
 * returns configs_codes: if fail loading file
 * returns CFG_OK: if loading configs ok
 */
uint8_t cloud_client_load_cfg(const char *filename);


/*
 * Login returns codes
 */
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

/**
 * Cloud client clean databases
 *
 * returns false: if error cleaning database
 * returns true: if cleaning ok
 */
bool cloud_client_clean_base(void);

/*
 * Free memory
 */
void cloud_client_free(void);


/*
 * Cloud client callbacks
 * If not used, all signals are NULL
 */


/*
 * Error callback codes
 */
enum error_codes {
	ERR_CONNECT = 100,
	ERR_SEND_LOGIN = 101,
	ERR_RECV_LOGIN = 102,
	ERR_LSQL_OPEN = 103,
	ERR_LSQL_SELECT = 104,
	ERR_LSQL_INSERT = 105,
	ERR_LSQL_UPDATE = 106,
	ERR_LSQL_REMOVE = 107,
	ERR_LSQL_CLEAN = 108,
	ERR_SEND_CMD = 109,
	ERR_SEND_FILE = 110
};


/**
 * Set error callback for all modules
 * @error_cb: error callback pointer:
 *		@message: error message
 *		@code: error code
 *		@data: user data
 * @data: data pointer for callback
 */
void cloud_client_set_error_cb(void (*error_cb)(const char *message, uint8_t code, void *data), void *data);

/**
 * Set update status callback
 * @update_status_cb: status callback pointer:
 *		@status: server online status
 *		@data: user data
 * @data: user data pointer for callback
 */
void cloud_client_set_update_status_cb(void (*update_status_cb)(bool status, void *data), void *data);


#endif