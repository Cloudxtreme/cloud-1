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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


struct main_checker_cfg {
	unsigned interval;
};

struct life_checker_cfg {
	unsigned interval;
};

struct server_cfg {
	char ip[16];
	unsigned port;
};

struct user_cfg {
	unsigned id;
	char path[255];
};

struct db_cfg {
	char lb_path[255]; /* local base path */
};


/**
 * Loading configs from file to RAM
 * @filename: name of json configs file
 *
 * returns CFG_OK: if reading ok
 * returns error code: if fail reading
 */
uint8_t configs_load(const char *filename);


/*
 * Server configs
 */
struct server_cfg *configs_get_server(void);

/*
 * Checker timer configs
 */
struct main_checker_cfg *configs_get_main_checker(void);

/*
 * Server life timer configs
 */
struct life_checker_cfg *configs_get_life_checker(void);

/*
 * User configs
 */
struct user_cfg *configs_get_user(void);

/*
 * User configs
 */
struct db_cfg *configs_get_database(void);


#endif