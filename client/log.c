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

#include "log.h"
#include <stdio.h>
#include <string.h>
#include "utils.h"

#define PATH_SZIE 255

static struct {
	char path[PATH_SZIE];
	char sync_path[PATH_SZIE];
} log;


bool log_set_path(const char *path)
{
	if (strlen(path) >= PATH_SZIE)
		return false;
	strncpy(log.path, path, PATH_SZIE);
	return true;
}

bool log_set_sync_path(const char *path)
{
	if (strlen(path) >= PATH_SZIE)
		return false;
	strncpy(log.sync_path, path, PATH_SZIE);
	return true;	
}

bool log_local(const char *message, unsigned log_type)
{
	FILE *file;
	char out_msg[255];
	char date[DATE_SIZE];
	char time[TIME_SIZE];	

	date_now(date);
	time_now(time);

	strcpy(out_msg, "[");
	strcat(out_msg, date);
	strcat(out_msg, "][");
	strcat(out_msg, time);
	strcat(out_msg, "][");

	switch(log_type) {
		case LOG_ERROR: {
			strcat(out_msg, "ERROR] ");
			break;
		}
		case LOG_WARNING: {
			strcat(out_msg, "WARNING] ");
			break;
		}
		case LOG_INFO: {
			strcat(out_msg, "INFO] ");
			break;
		}
	}
	strcat(out_msg, message);
	puts(out_msg);

	file = fopen(log.path, "a");
	if (file == NULL)
		return false;
	if (!fputs(out_msg, file)) {
		fclose(file);
		return false;
	}
	fclose(file);
	return true;
}

void log_sync(const char *message, const char *filename)
{
	FILE *file;
	char out_msg[255];
	char date[DATE_SIZE];
	char time[TIME_SIZE];	

	date_now(date);
	time_now(time);

	strcpy(out_msg, "[");
	strcat(out_msg, date);
	strcat(out_msg, "][");
	strcat(out_msg, time);
	strcat(out_msg, "] ");
	strcat(out_msg, message);
	if (filename != NULL) {
		strcat(out_msg, " ");
		strcat(out_msg, filename);
	}
	puts(out_msg);

	file = fopen(log.sync_path, "a");
	if (file == NULL)
		return;

	if (!fputs(out_msg, file)) {
		fclose(file);
		return;
	}
	fclose(file);
}