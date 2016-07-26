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

/*
 * Server configs
 */
struct server_cfg *configs_get_server();

/**
 * Loading configs from file to RAM
 * @filename: name of json configs file
 *
 * returns error_code: if fail loading
 * returns CFG_OK: if succeful loading
 */
uint8_t configs_load(const char *filename);


#endif