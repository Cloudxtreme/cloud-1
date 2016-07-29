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

#ifndef __LOG_H__
#define __LOG_H__

#include <stdbool.h>

enum {
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFO
};


/**
 * Setting path for main log file
 * @path: path with filename
 *
 * returns true: if setted
 * returns false: if path size to long
 */
bool log_set_path(const char *path);

/**
 * Setting path for sync operations lof file
 * 
 * returrns true: if setted
 * returns false: if path size to long
 */
bool log_set_sync_path(const char *path);

/**
 * Writing message to local log file
 * @message: writing message
 * @log_type: type of log message
 *
 * returns true: if message writed
 * returns false: if message writing error
 */
bool log_local(const char *message, unsigned log_type);

/**
 * Writting message to local sync log file
 * @message: log message
 */
void log_sync(const char *message, const char *filename);


#endif