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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <stdio.h>


#define BLOCK_SIZE 512

enum {
    HR_OK = 10055,
    HR_FAIL = 10040
};

struct file_hash {
    unsigned char hash[SHA512_DIGEST_LENGTH];
};

struct sock_answ {
    unsigned code;
};

struct file_info {
    char filename[255];
    long unsigned int blocks;
    unsigned last_block;
};

static bool compare_hashes(const unsigned char *local, const unsigned char *remote)
{
    const unsigned char *p_local = &local[0];
    const unsigned char *p_remote = &remote[0];

    for (size_t i = 0; i < SHA512_DIGEST_LENGTH; i++, p_local++, p_remote++) {
        if (*p_local != *p_remote)
            return false;        
    }
    return true;
}

static bool send_file_data(struct file_transfer *ftransfer, FILE *file, unsigned data_size, SHA512_CTX *sha_ctx)
{
	int ret_val;
	unsigned ret_byte;
	char data[data_size];

	ret_byte = fread((void *)data, data_size, 1, file);
	if (ret_byte != 1) {
		if (ftransfer->error != NULL)
			ftransfer->error("Fail reading file block.", ftransfer->err_data);
		return false;
	}
	/*
	 * Reading and sending file block
	 */
	if (!tcp_client_send(ftransfer->client, (const void *)data, data_size)) {
		if (ftransfer->error != NULL)
			ftransfer->error("Fail sending file block.", ftransfer->err_data);
		return false;
	}
	/*
	 * Generating file hash
	 */
	ret_val = SHA512_Update(sha_ctx, (const void *)data, data_size);
	if (!ret_val) {
		if (ftransfer->error != NULL) 
			ftransfer->error("Fail update sha512 hash of sending block.", ftransfer->err_data);
		return false;
	}
	return true;
}

static bool recv_file_data(struct file_transfer *ftransfer, FILE *file, unsigned data_size, SHA512_CTX *sha_ctx)
{
	int ret_val;
	unsigned ret_byte;
	char data[data_size];

	/*
	 * Receiving and writing file block
	 */
	if (!tcp_client_recv(ftransfer->client, (void *)data, data_size)) {
		if (ftransfer->error != NULL)
			ftransfer->error("Fail receiving file block.", ftransfer->err_data);
		return false;
	}
	ret_byte = fwrite((const void *)data, data_size, 1, file);
	if (ret_byte != 1) {
		if (ftransfer->error != NULL)
			ftransfer->error("Fail writing file block.", ftransfer->err_data);
		return false;
	}	
	/*
	 * Generating file hash
	 */
	ret_val = SHA512_Update(sha_ctx, (const void *)data, data_size);
	if (!ret_val) {
		if (ftransfer->error != NULL) 
			ftransfer->error("Fail update sha512 hash of receiving block.", ftransfer->err_data);
		return false;
	}
	return true;
}


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
    int ret_val;    
    SHA512_CTX sha_ctx;
    struct stat finfo_buff;
    struct file_info finfo;
    struct sock_answ answ;
    struct file_hash fhash;

    if (strlen(filename) > 255) {
    	if (ftransfer->error != NULL) 
			ftransfer->error("File name of sending file is to long.", ftransfer->err_data);
		return false;
    }
    strncpy(finfo.filename, filename, 255);

    file = fopen(filename, "rb");
    if (file == NULL) {
    	if (ftransfer->error != NULL) 
			ftransfer->error("Can not open sending file.", ftransfer->err_data);
		return false;
    }
    fstat(fileno(file), &finfo_buff);

    if (finfo_buff.st_size < BLOCK_SIZE) {
        finfo.last_block = finfo_buff.st_size;
        finfo.blocks = 0;
    } else {
        finfo.last_block = finfo_buff.st_size % BLOCK_SIZE;
        finfo.blocks = (finfo_buff.st_size - finfo.last_block) / BLOCK_SIZE;
    }

    /*
     * Sending file info to server and checking answ
     */
    if (!tcp_client_send(ftransfer->client, (const void *)&finfo, sizeof(finfo))) {
    	if (ftransfer->error != NULL) 
			ftransfer->error("Fail sending file info of sending file.", ftransfer->err_data);
		fclose(file);
		return false;
    }

    ret_val = SHA512_Init(&sha_ctx);
	if (!ret_val) {
		if (ftransfer->error != NULL) 
			ftransfer->error("Fail init sha512 hash of sending file.", ftransfer->err_data);
		fclose(file);
		return false;
	}
	/*
     * Sending small file
     */
    if (finfo_buff.st_size < BLOCK_SIZE) {
    	if (!send_file_data(ftransfer, file, finfo_buff.st_size, &sha_ctx)) {
			if (ftransfer->error != NULL) 
				ftransfer->error("Fail sending small file.", ftransfer->err_data);
			fclose(file);
			return false;
		}
    	fclose(file);

    	ret_val = SHA512_Final(fhash.hash, &sha_ctx);
        if (!ret_val) {
        	if (ftransfer->error != NULL) 
				ftransfer->error("Fail finalization of sha512.", ftransfer->err_data);
			return false;
        }
        /*
         * Sending file hash
         */
        if (!tcp_client_send(ftransfer->client, (const void *)&fhash, sizeof(fhash))) {
        	if (ftransfer->error != NULL) 
				ftransfer->error("Fail sending hash of file.", ftransfer->err_data);
			return false;
        }
        /*
         * Receiving answ
         */
        if (!tcp_client_recv(ftransfer->client, (void *)&answ, sizeof(answ))) {
        	if (ftransfer->error != NULL) 
				ftransfer->error("Fail receiving file hash answare.", ftransfer->err_data);
			return false;
        }        
        if (answ.code != HR_OK) {
        	if (ftransfer->error != NULL) 
				ftransfer->error("Bad sended file checksum.", ftransfer->err_data);
			return false;
        }
    	return true;
    }
    return true;
}

bool file_transfer_recv_file(struct file_transfer *ftransfer, const char *path)
{
	FILE *file;    
    int ret_val;
    char filename[512];
    SHA512_CTX sha_ctx;
    struct file_info finfo;
    struct sock_answ answ;
    struct file_hash fhash;
    struct file_hash lhash;

    if (strlen(path) > 255) {
    	if (ftransfer->error != NULL) 
			ftransfer->error("File name of receiving file is to long.", ftransfer->err_data);
		return false;
    }    

    /*
     * Receiving file info
     */
    if (!tcp_client_recv(ftransfer->client, (void *)&finfo, sizeof(finfo))) {
    	if (ftransfer->error != NULL) 
			ftransfer->error("Fail sending file info of sending file.", ftransfer->err_data);
		return false;
    }

    ret_val = SHA512_Init(&sha_ctx);
	if (!ret_val) {
		if (ftransfer->error != NULL) 
			ftransfer->error("Fail init sha512 hash of sending file.", ftransfer->err_data);		
		return false;
	}	
    strcpy(filename, path);
    strcat(filename, finfo.filename);

    file = fopen(filename, "wb");
    if (file == NULL) {
    	if (ftransfer->error != NULL) 
			ftransfer->error("Can not open receiving file.", ftransfer->err_data);
		return false;
    }
    /*
     * Receiving small file
     */
    if (finfo.blocks == 0) {
    	if (!recv_file_data(ftransfer, file, finfo.last_block, &sha_ctx)) {
			if (ftransfer->error != NULL) 
				ftransfer->error("Fail receiving small file.", ftransfer->err_data);
			fclose(file);
			return false;
		}
    	fclose(file);

    	ret_val = SHA512_Final(lhash.hash, &sha_ctx);
        if (!ret_val) {
        	if (ftransfer->error != NULL) 
				ftransfer->error("Fail finalization of sha512.", ftransfer->err_data);
			return false;
        }
        /*
         * Receiving file hash
         */
        if (!tcp_client_recv(ftransfer->client, (void *)&fhash, sizeof(fhash))) {
        	if (ftransfer->error != NULL) 
				ftransfer->error("Fail receiving hash of file.", ftransfer->err_data);
			return false;
        }
        if (!compare_hashes(lhash.hash, fhash.hash))
        	answ.code = HR_FAIL;
        answ.code = HR_OK;
        /*
         * Sending answ
         */
        if (!tcp_client_send(ftransfer->client, (const void *)&answ, sizeof(answ))) {
        	if (ftransfer->error != NULL) 
				ftransfer->error("Fail sending file hash answare.", ftransfer->err_data);
			return false;
        }        
        if (answ.code != HR_OK) {
        	if (ftransfer->error != NULL) 
				ftransfer->error("Bad receiving file checksum.", ftransfer->err_data);
			return false;
        }
    	return true;
    }
    return true;
}