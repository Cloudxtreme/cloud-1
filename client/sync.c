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
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>


bool sync_get_file_hash(const char *filename, char *hash)
{
	return true;
}

struct flist *sync_get_file_list(const char *path)
{
	DIR *dir;
	FILE *f;
	struct flist *flist = NULL;
	struct dirent *f_cur;

    if ((dir = opendir(path)) == NULL)
    	return NULL;
    
    while ((f_cur = readdir(dir)) != NULL) {
    	if (!strcmp(f_cur->d_name, ".") || !strcmp(f_cur->d_name, "..") || f_cur->d_name[0] == '.')
    		continue;

    	f = fopen(f_cur->d_name, "rb");
    	if (f == NULL)
    		continue;

    	struct stat sbuf;
    	struct file *file = (struct file *)malloc(sizeof(struct file));
    	strncpy(file->name, f_cur->d_name, 255);

    	fstat(fileno(f), &sbuf);
    	file->size = sbuf.st_size;
    	fclose(f);

    	flist = flist_append(flist, file);
    }
    closedir(dir);
	return flist;
}