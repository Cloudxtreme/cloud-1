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

struct main_checker_cfg {
	unsigned interval;	
};

struct life_checker_cfg {
	unsigned interval;	
};

/*
 * Checker timer configs
 */
struct main_checker_cfg *configs_get_main_checker();

/*
 * Server life timer configs
 */
struct life_checker_cfg *configs_get_life_checker();

/**
 * Loading configs from file to RAM
 * @filename: name of json configs file
 *
 * returns false: if fail loading
 * returns true: if succeful loading
 */
bool configs_load(const char *filename);


#endif