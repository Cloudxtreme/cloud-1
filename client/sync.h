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

#ifndef __SYNC_H__
#define __SYNC_H__

#include <stdio.h>
#include <stdbool.h>
#include "flist.h"


/**
 * Calculating hash of file
 * @filename: name of file
 * @hash: out hash of file
 *
 * returns true: if calculating hash ok
 * returns false: if error calculating
 */ 
bool sync_get_file_hash(const char *filename, char *hash);

/**
 * Read files in sync directory
 * @path: sync directory
 *
 * returns true: if getting ok
 * returns false: if getting error
 */
struct flist *sync_get_file_list(const char *path);


#endif