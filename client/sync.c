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

#include "sync.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>


#define BUF_SIZE 512


static int pos(const char *restrict str, const char symb)
{
    int p = 0;

    while (*str) {
        if (*str == symb)
            return p;
        str++;
        p++;
    }
    return -1;
}

static void convert_hash(const unsigned char *restrict hash, char *out)
{
	char sym[3];

	for (size_t i = 0; i < SHA512_DIGEST_LENGTH; i++, hash++, out += 2) {
		sprintf(sym, "%02X", *hash);
		*out = sym[0];
		*(out + 1) = sym[1];
	}
	*out = '\0';
}

static void date_time_string(time_t *restrict time, char *out_str)
{
	char time_str[20];
    char date_str[20];
    struct tm *timeinfo;

    timeinfo = localtime(time);
    strftime(date_str, 20, "%F", timeinfo);
    strftime(time_str, 20, "%T", timeinfo);

    strcpy(out_str, date_str);
    strcat(out_str, " ");
    strcat(out_str, time_str);
}


bool sync_get_file_hash(const char *filename, char *hash)
{
	int ret_val;
	unsigned bytes;
	SHA512_CTX sha_ctx;
	char buffer[BUF_SIZE + 1];
	unsigned char uhash[SHA512_DIGEST_LENGTH];

	ret_val = SHA512_Init(&sha_ctx);
	if (!ret_val)
		return false;

	int fd = open(filename, O_RDONLY);

	while ((bytes = read(fd, buffer, BUF_SIZE)) > 0) {
		buffer[bytes] = 0;
		ret_val = SHA512_Update(&sha_ctx, (const void *)buffer, bytes);
		if (!ret_val)
			return false;
	}
	close(fd);

	ret_val = SHA512_Final(uhash, &sha_ctx);
	if (!ret_val)
		return false;

	convert_hash(uhash, hash);
	return true;
}

struct flist *sync_get_file_list(const char *path)
{
	int fd;
	DIR *dir;
	char full_path[512];

	struct flist *flist = NULL;
	struct dirent *f_cur;

    if ((dir = opendir(path)) == NULL)
    	return NULL;
    
    while ((f_cur = readdir(dir)) != NULL) {
    	/*
    	 * Ignoring hidden files and up folders
    	 */
    	if (!strcmp(f_cur->d_name, ".") || !strcmp(f_cur->d_name, "..") || f_cur->d_name[0] == '.')
    		continue;

    	/*
    	 * It is a folder
    	 */
    	if (pos(f_cur->d_name, '.') == -1)
    		continue;

		strcpy(full_path, path);
		strcat(full_path, f_cur->d_name);

		fd = open(full_path, O_RDONLY);
		if (fd == -1)
    		continue;

    	struct stat sbuf;
    	struct file *file = (struct file *)malloc(sizeof(struct file));
    	strncpy(file->name, f_cur->d_name, 255);

    	fstat(fd, &sbuf);
    	file->size = sbuf.st_size;
    	date_time_string(&sbuf.st_mtime, file->change_date);
    	close(fd);

    	flist = flist_append(flist, file);
    }
    closedir(dir);
	return flist;
}