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

#ifndef __CLOUD_SERVER_H__
#define __CLOUD_SERVER_H__

#include <stdbool.h>
#include <stdint.h>

enum {
    CFG_OK,
    CFG_FILE_NOT_FOUND,
    CFG_SC_PORT_ERROR,
    CFG_STC_PATH_ERROR
};


/*
 * Setting log path
 */
bool cloud_server_set_log(const char *filename);

/*
 * Loading configs
 */
uint8_t cloud_server_load_cfg(const char *filename);

/*
 * Starting cloud server
 */
bool cloud_server_start();


#endif