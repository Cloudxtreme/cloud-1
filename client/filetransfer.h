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


enum send_file_codes {
	FT_NAME_LONG,
	FT_FILE_NOT_FOUND,
	FT_SEND_FINFO_ERR,
	FT_SHA_INIT_ERR,
	FT_SEND_BLOCK_ERR,
	FT_SEND_LAST_BLOCK_ERR,
	FT_SHA_FINAL_ERR,
	FT_SEND_HASH_ERR,
	FT_HASH_RECV_ERR,
	FT_SEND_FAIL,
	FT_SEND_OK
};

enum recv_file_codes {
	FT_ACCESS_ERR,
	FT_RECV_FINFO_ERR,
	FT_RECV_BLOCK_ERR,
	FT_RECV_LAST_BLOCK_ERR,
	FT_RECV_HASH_ERR,
	FT_HASH_SEND_ERR,
	FT_RECV_FAIL,
	FT_RECV_OK
};

struct file_transfer {
	struct tcp_client *client;
};


/**
 * Initialization file transfer structure
 * @ftransfer: file transfer structure pointer
 * @client: tcp client structure pointer
 */
void file_transfer_init(struct file_transfer *ftransfer, struct tcp_client *client);

/**
 * Sending file to other tcp socket
 * @ftransfer: file transfer structure pointer
 * @filename: sending file name
 *
 * returns false: if sending error
 * returns true: if sending ok
 */
uint8_t file_transfer_send_file(struct file_transfer *ftransfer, const char *filename);

/**
 * Receiving file from tcp socket
 * @ftransfer: file transfer structure pointer
 * @path: receiving local path
 *
 * returns false: if sending error
 * returns true: if sending ok
 */
uint8_t file_transfer_recv_file(struct file_transfer *ftransfer, const char *path);


#endif