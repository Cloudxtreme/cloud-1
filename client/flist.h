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

#ifndef __FILE_LIST_H__
#define __FILE_LIST_H__

#include <stdio.h>


struct file {
	char name[255];
	unsigned long size;
	char change_date[50];
};


struct flist {
    struct file *file;	/* file */
    struct flist *next; /* next element */
};

/**
 * Add new element in end of list
 * @list: single-linked list struct
 * @data: user's data
 *
 * returns single-linked list with added element
 */
struct flist *flist_append(struct flist *list, struct file *file);

/*
 * Next element of list
 */
struct flist *flist_next(struct flist *list);

/*
 * Getting current file element
 */
struct file *flist_get_file(struct flist *list);

/*
 * Free list elements
 */
void flist_free_all(struct flist *list);


#endif