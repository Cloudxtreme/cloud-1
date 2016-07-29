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

#include "localbase.h"
#include <sqlite3.h>
#include <string.h>


struct flists {
	size_t *count;
	struct flist *flist;
	struct flist *dlist;
};

struct stat_cb_data {
	const struct file *file; /* In data */
	bool *match;	   		 /* Out data */
};

static struct {
	sqlite3 *base;
} db;


static int delete_file_cb(void *data, int argc, char **argv, char **col_name)
{
	struct flists *lists = (struct flists *)data;

	for (struct flist *l = lists->flist; l != NULL; l = flist_next(l)) {
		struct file *f = flist_get_file(l);
		if (!strcmp(argv[1], f->name))
			return 0;
	}
	struct file *dfile = (struct file *)malloc(sizeof(struct file));
	
	strcpy(dfile->name, argv[1]);
	sscanf(argv[2], "%lu", &dfile->size);
	strcpy(dfile->change_date, argv[3]);
	lists->dlist = flist_append(lists->dlist, dfile);
	(*lists->count)++;
	return 0;
}

static int check_file_cb(void *data, int argc, char **argv, char **col_name)
{
	size_t *count = (size_t *)data;
	*count = argc;
	return 0;
}

static int compare_stat_cb(void *data, int argc, char **argv, char **col_name)
{
	unsigned long sz = 0;
	struct stat_cb_data *sdata = (struct stat_cb_data *)data;

	sscanf(argv[2], "%lu", &sz);

	if (!strcmp(argv[1], sdata->file->name) && (sz == sdata->file->size)) {
		*sdata->match = true;
		return 0;
	}
	*sdata->match = false;
	return 0;
}

bool local_base_open(const char *filename)
{
	int ret_val;

	ret_val = sqlite3_open(filename, &db.base);
	if (ret_val)
		return false;
	return true;
}

uint8_t local_base_check_file_exists(struct file *restrict file)
{
	int ret_val;
	size_t count = 0;
	char sql[512];

	strcpy(sql, "SELECT * FROM files WHERE name=\"");
	strcat(sql, file->name);
	strcat(sql, "\"");

	ret_val = sqlite3_exec(db.base, sql, check_file_cb, (void *)&count, NULL);
	if (ret_val != SQLITE_OK)
		return LB_SQL_ERROR;

	if (count == 0)
		return LB_FILE_NOT_FOUND;
	return LB_FILE_EXISTS;
}

uint8_t local_base_compare_file_stat(struct file *restrict file)
{
	int ret_val;
	bool match = false;
	char sql[512];
	struct stat_cb_data sdata;

	sdata.file = file;
	sdata.match = &match;

	strcpy(sql, "SELECT * FROM files WHERE name=\"");
	strcat(sql, file->name);
	strcat(sql, "\"");

	ret_val = sqlite3_exec(db.base, sql, compare_stat_cb, (void *)&sdata, NULL);
	if (ret_val != SQLITE_OK)
		return LB_SQL_ERROR;

	if (!match)
		return LB_STAT_NOT_MATCH;
	return LB_STAT_MATCH;
}

bool local_base_add_file(struct file *restrict new_file)
{
	int ret_val;
	char sql[512];
	char num[100];

	sprintf(num, "%lu", new_file->size);

	strcpy(sql, "INSERT INTO files(name, size, change_date) VALUES(\"");
	strcat(sql, new_file->name);
	strcat(sql, "\", \"");
	strcat(sql, num);
	strcat(sql, "\", \"");
	strcat(sql, new_file->change_date);
	strcat(sql, "\")");

	ret_val = sqlite3_exec(db.base, sql, NULL, 0, NULL);
	if (ret_val != SQLITE_OK)
		return false;
	return true;
}

bool local_base_update_file(struct file *restrict changed_file)
{
	int ret_val;
	char sql[512];
	char num[100];

	sprintf(num, "%lu", changed_file->size);

	strcpy(sql, "UPDATE files SET size=");
	strcat(sql, num);
	strcat(sql, ", change_date=\"");
	strcat(sql, changed_file->change_date);
	strcat(sql, "\" WHERE name=\"");
	strcat(sql, changed_file->name);
	strcat(sql, "\"");

	ret_val = sqlite3_exec(db.base, sql, NULL, 0, NULL);
	if (ret_val != SQLITE_OK)
		return LB_SQL_ERROR;
	return LB_SQL_UPDATED;
}

struct flist *local_base_find_deleted(struct flist *file_list)
{
	int ret_val;
	size_t count = 0;
	char sql[512];
	struct flists lists;
	struct flist *del_files = NULL;

	lists.flist = file_list;
	lists.dlist = del_files;
	lists.count = &count;
	strcpy(sql, "SELECT * FROM files");

	ret_val = sqlite3_exec(db.base, sql, delete_file_cb, (void *)&lists, NULL);
	if (ret_val != SQLITE_OK)
		return NULL;

	if (count == 0)
		return NULL;

	return lists.dlist;
}

bool local_base_remove(struct file *restrict file)
{
	int ret_val;
	char sql[512];

	strcpy(sql, "DELETE FROM files WHERE name=\"");
	strcat(sql, file->name);
	strcat(sql, "\"");

	ret_val = sqlite3_exec(db.base, sql, NULL, 0, NULL);
	if (ret_val != SQLITE_OK)
		return false;
	return true;
}

bool local_base_clean(void)
{
	int ret_val;
	char sql[512];

	strcpy(sql, "DELETE FROM files");

	ret_val = sqlite3_exec(db.base, sql, NULL, 0, NULL);
	if (ret_val != SQLITE_OK)
		return false;
	return true;	
}

void local_base_close(void)
{
	sqlite3_close(db.base);
}