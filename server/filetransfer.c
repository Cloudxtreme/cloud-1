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

#include "filetransfer.h"


void file_transfer_init(struct file_transfer *ftransfer, struct tcp_client *client)
{
	ftransfer->error = NULL;
	ftransfer->err_data = NULL;
	ftransfer->client = client;
}

void file_transfer_set_error_cb(struct file_transfer *ftransfer, void (*error)(const char *, void*), void *data)
{
	ftransfer->error = error;
	ftransfer->err_data = data;
}

bool file_transfer_send_file(struct file_transfer *ftransfer, const char *filename)
{
	FILE *file;

	file = fopen(filename, "rb");

	fclose(file);
	return true;
}

bool file_transfer_send_file(struct file_transfer *ftransfer, const char *localname)
{
	FILE *file;

	file = fopen(localname, "wb");

	fclose(file);
	return true;
}