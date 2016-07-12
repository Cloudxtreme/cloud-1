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
 * returns false: if fail filename length
 * returns true: if ok
 */
bool cloud_client_set_log(const char *filename);

/**
 * Loading cloud client configs JSON file
 * @filename: path to configs file
 *
 * returns false: if fail loading file
 * returns true: if ok
 */
bool cloud_client_load_cfg(const char *filename);

/*
 * Starting all threads of cloud client
 */
void cloud_client_start(void);

/*
 * Free memory
 */
void cloud_client_free(void);


#endif