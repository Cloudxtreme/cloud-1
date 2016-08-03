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

#ifndef __CONFIGS_H__
#define __CONFIGS_H__

#include <stdbool.h>
#include <stdint.h>

struct server_cfg {
	unsigned port;
	unsigned max_clients;
};

struct storage_cfg {
	char path[255];
};

/*
 * Server configs
 */
struct server_cfg *configs_get_server(void);

/*
 * Storage
 */
struct storage_cfg *configs_get_storage(void);

/**
 * Loading configs from file to RAM
 * @filename: name of json configs file
 *
 * returns error_code: if fail loading
 * returns CFG_OK: if succeful loading
 */
uint8_t configs_load(const char *filename);


#endif