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

#ifndef __LOCAL_BASE_H__
#define __LOCAL_BASE_H__

#include <stdbool.h>
#include <stdint.h>
#include "flist.h"


enum {
	LB_FILE_EXISTS,
	LB_FILE_NOT_FOUND,
	LB_SQL_ERROR,
	LB_STAT_MATCH,
	LB_STAT_NOT_MATCH,
	LB_SQL_UPDATED
};
/**
 * Open sqlite file
 * @filename: base file
 *
 * returns false: opening fail
 * returns true: if open ok
 */
bool local_base_open(const char *filename);

/**
 * Check file exists in base
 * @file: local file
 *
 * returns LB_FILE_NOT_FOUND: if file not found
 * returns LB_FILE_EXISTS: if file exists
 * returns LB_SQL_ERROR: if called sql error
 */
uint8_t local_base_check_file_exists(struct file *restrict file);

/**
 * Comparing files stats
 * @file: local file
 *
 * returns LB_STAT_NOT_MATCH: if stats not match
 * returns LB_STAT_MATCH: if stats match
 * returns LB_SQL_ERROR: if called sql error
 */
uint8_t local_base_compare_file_stat(struct file *restrict file);

/**
 * Add new file in base
 * @new_file: new adding file
 *
 * returns false: if fail adding to base
 * returns true: if adding ok
 */
bool local_base_add_file(struct file *restrict new_file);

/**
 * If file was chanched, update it
 * @changed_file: new changed file
 *
 * returns false: if updating fail
 * returns true: if updating ok
 */
bool local_base_update_file(struct file *restrict changed_file);

/*
 * Close local base
 */
void local_base_close();


#endif