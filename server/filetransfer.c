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
	if (ret_byte != 1)
		return false;
	/*
	 * Reading and sending file block
	 */
	if (!tcp_client_send(ftransfer->client, (const void *)data, data_size))
		return false;
	/*
	 * Generating file hash
	 */
	ret_val = SHA512_Update(sha_ctx, (const void *)data, data_size);
	if (!ret_val)
		return false;
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
	if (!tcp_client_recv(ftransfer->client, (void *)data, data_size))
		return false;

	ret_byte = fwrite((const void *)data, data_size, 1, file);
	if (ret_byte != 1)
		return false;
	/*
	 * Generating file hash
	 */
	ret_val = SHA512_Update(sha_ctx, (const void *)data, data_size);
	if (!ret_val)
		return false;
	return true;
}


void file_transfer_init(struct file_transfer *ftransfer, struct tcp_client *client)
{
	ftransfer->client = client;
}

uint8_t file_transfer_send_file(struct file_transfer *ftransfer, const char *filename)
{
	FILE *file;    
    int ret_val;    
    SHA512_CTX sha_ctx;
    struct stat finfo_buff;
    struct file_info finfo;
    struct sock_answ answ;
    struct file_hash fhash;

    if (strlen(filename) > 255)
    	return FT_NAME_LONG;

    strncpy(finfo.filename, filename, 255);

    file = fopen(filename, "rb");
    if (file == NULL)
    	return FT_FILE_NOT_FOUND;

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
    if (!tcp_client_send(ftransfer->client, (const void *)&finfo, sizeof(struct file_info))) {
    	fclose(file);
		return FT_SEND_FINFO_ERR;
    }

    ret_val = SHA512_Init(&sha_ctx);
	if (!ret_val) {
		fclose(file);
		return FT_SHA_INIT_ERR;
	}
	/*
     * Sending small file
     */
    if (finfo_buff.st_size < BLOCK_SIZE) {
    	if (!send_file_data(ftransfer, file, finfo_buff.st_size, &sha_ctx)) {
			fclose(file);             
			return FT_SEND_BLOCK_ERR;
		}
    	fclose(file);

    	ret_val = SHA512_Final(fhash.hash, &sha_ctx);
        if (!ret_val)
        	return FT_SHA_FINAL_ERR;
        /*
         * Sending file hash
         */
        if (!tcp_client_send(ftransfer->client, (const void *)&fhash, sizeof(fhash)))
        	return FT_SEND_HASH_ERR;
        /*
         * Receiving answ
         */
        if (!tcp_client_recv(ftransfer->client, (void *)&answ, sizeof(answ)))
        	return FT_HASH_RECV_ERR;
       
        if (answ.code != HR_OK)
        	return FT_SEND_FAIL;
        
    	return FT_SEND_OK;
    }
    /*
     * If size of file >= BLOCK_SIZE
     * Sending each block of file
     */     
    for (unsigned long i = 0; i < finfo.blocks; i++) {
    	if (!send_file_data(ftransfer, file, BLOCK_SIZE, &sha_ctx)) {
			fclose(file);            
			return FT_SEND_BLOCK_ERR;
		}
    }
    /*
     * Sending last block of file
     */
    if (finfo.last_block != 0) {
    	if (!send_file_data(ftransfer, file, finfo.last_block, &sha_ctx)) {
			fclose(file);
			return FT_SEND_LAST_BLOCK_ERR;
		}
    }
    ret_val = SHA512_Final(fhash.hash, &sha_ctx);
    if (!ret_val)
        return FT_SHA_FINAL_ERR;
    /*
     * Sending file hash
     */
    if (!tcp_client_send(ftransfer->client, (const void *)&fhash, sizeof(fhash)))
        return FT_SEND_HASH_ERR;
    /*
     * Receiving answ
     */
    if (!tcp_client_recv(ftransfer->client, (void *)&answ, sizeof(answ)))
        return FT_HASH_RECV_ERR;
       
    if (answ.code != HR_OK)
        return FT_SEND_FAIL;

    fclose(file);
    return FT_SEND_OK;
}

uint8_t file_transfer_recv_file(struct file_transfer *ftransfer, const char *path)
{
	FILE *file;    
    int ret_val;
    SHA512_CTX sha_ctx;
    struct file_info finfo;
    struct sock_answ answ;
    struct file_hash fhash;
    struct file_hash lhash;

    if (strlen(path) > 255)
    	return FT_NAME_LONG;
    /*
     * Receiving file info
     */
    if (!tcp_client_recv(ftransfer->client, (void *)&finfo, sizeof(struct file_info)))
    	return FT_RECV_FINFO_ERR;

    ret_val = SHA512_Init(&sha_ctx);
	if (!ret_val)
		return FT_SHA_INIT_ERR;

    file = fopen(path, "wb");
    if (file == NULL)
    	return FT_ACCESS_ERR;    
    /*
     * Receiving small file
     */
    if (finfo.blocks == 0) {
    	if (!recv_file_data(ftransfer, file, finfo.last_block, &sha_ctx)) {
			fclose(file);
			return FT_RECV_BLOCK_ERR;
		}
		fsync(fileno(file));
    	fclose(file);

    	ret_val = SHA512_Final(lhash.hash, &sha_ctx);
        if (!ret_val)
        	return FT_SHA_FINAL_ERR;    
        /*
         * Receiving file hash
         */
        if (!tcp_client_recv(ftransfer->client, (void *)&fhash, sizeof(fhash)))
        	return FT_RECV_HASH_ERR;
        
        if (!compare_hashes(lhash.hash, fhash.hash))
        	answ.code = HR_FAIL;
        answ.code = HR_OK;
        /*
         * Sending answ
         */
        if (!tcp_client_send(ftransfer->client, (const void *)&answ, sizeof(answ)))
        	return FT_HASH_SEND_ERR;
  
        if (answ.code != HR_OK)
            return FT_RECV_FAIL;
        
    	return FT_RECV_OK;
    }
    /*
     * If size of file >= BLOCK_SIZE
     * Receiving each block of file
     */
    for (unsigned long i = 0; i < finfo.blocks; i++) {
    	if (!recv_file_data(ftransfer, file, BLOCK_SIZE, &sha_ctx)) {
			fclose(file);
			return FT_RECV_BLOCK_ERR;
		}
    }
    /*
     * Receiving last block of file
     */
    if (finfo.last_block != 0) {
    	if (!recv_file_data(ftransfer, file, finfo.last_block, &sha_ctx)) {
			fclose(file);
			return FT_RECV_LAST_BLOCK_ERR;
		}
    }
    fsync(fileno(file));
    fclose(file);

    ret_val = SHA512_Final(lhash.hash, &sha_ctx);
    if (!ret_val)
       	return FT_SHA_FINAL_ERR;
    /*
     * Receiving file hash
     */
    if (!tcp_client_recv(ftransfer->client, (void *)&fhash, sizeof(fhash)))
       	return FT_RECV_HASH_ERR;

    if (!compare_hashes(lhash.hash, fhash.hash))
       	answ.code = HR_FAIL;
    answ.code = HR_OK;
    /*
     * Sending answ
     */
    if (!tcp_client_send(ftransfer->client, (const void *)&answ, sizeof(answ)))
       	return FT_HASH_SEND_ERR;
        
    if (answ.code != HR_OK)
       	return FT_RECV_FAIL;

    return FT_RECV_OK;
}