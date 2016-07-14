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

#ifndef __UTILS_H__
#define __UTILS_H__

#define TIME_SIZE 15
#define DATETIME_SIZE 30

/*
 * Errors structure
 */
struct error {
	int code;
	char message[255];
};


/*
 * Getting now time string
 */
void time_now(char *time);

/*
 * Getting now date string
 */
void date_now(char *date);

/*
 * Getting now date and time string
 */
void date_time_now(char *date_time);


#endif