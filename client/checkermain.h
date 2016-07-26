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

#ifndef __CHECKER_MAIN_H__
#define __CHECKER_MAIN_H__

#include <pthread.h>


void checker_main_start(pthread_mutex_t *mutex);

void checker_main_set_error_cb(void (*error)(const char *message, void*), void *data);


#endif