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


static struct {
	sqlite3 *base;
} db;


bool local_base_open(const char *restrict filename)
{
	int ret_val;

	ret_val = sqlite3_open(filename, &db.base);
	if (ret_val)
		return false;
	return true;
}

bool local_base_check_file_exists(struct file *restrict file)
{
	return true;
}

bool local_base_compare_file_stat(struct file *restrict file)
{
	return true;
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
	return true;
}

void local_base_close()
{
	sqlite3_close(db.base);
}