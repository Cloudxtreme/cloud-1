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

#ifndef __FILE_TRANSFER_H__
#define __FILE_TRANSFER_H__

#include "tcpclient.h"
#include <stdbool.h>


struct file_transfer {
	void *err_data;

	void (*error)(const char *message, void *data);

	struct tcp_client *client;
};


/**
 * Initialization file transfer structure
 * @ftransfer: file transfer structure pointer
 * @client: tcp client structure pointer
 */
void file_transfer_init(struct file_transfer *ftransfer, struct tcp_client *client);

/**
 * Error callbackc function
 * @ftransfer: file transfer structure pointer
 * @error: error function pointer
 * @data: user data pointer
 */
void file_transfer_set_error_cb(struct file_transfer *ftransfer, void (*error)(const char *, void*), void *data);

/**
 * Sending file to other tcp socket
 * @ftransfer: file transfer structure pointer
 * @filename: sending file name
 *
 * returns false: if sending error
 * returns true: if sending ok
 */
bool file_transfer_send_file(struct file_transfer *ftransfer, const char *filename);

/**
 * Receiving file from tcp socket
 * @ftransfer: file transfer structure pointer
 * @localname: receiving local file name
 *
 * returns false: if sending error
 * returns true: if sending ok
 */
bool file_transfer_send_file(struct file_transfer *ftransfer, const char *localname);


#endif