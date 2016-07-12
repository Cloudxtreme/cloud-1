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

#include "utils.h"
#include <string.h>
 #include <time.h>


void time_now(char *time_str)
{
    time_t t = time(NULL);
    struct tm *timeinfo;

    timeinfo = localtime (&t);
    strftime(time_str, TIME_SIZE, "%T", timeinfo);
}

void date_now(char *date_str)
{
    time_t t = time(NULL);
    struct tm *timeinfo;

    timeinfo = localtime (&t);
    strftime(date_str, TIME_SIZE, "%F", timeinfo);
}

void date_time_now(char *date_time)
{
    char time_str[TIME_SIZE];
    char date_str[TIME_SIZE];

    time_t t = time(NULL);
    struct tm *timeinfo;

    timeinfo = localtime (&t);
    strftime(date_str, TIME_SIZE, "%F", timeinfo);
    strftime(time_str, TIME_SIZE, "%T", timeinfo);

    strcpy(date_time, date_str);
    strcat(date_time, " ");
    strcat(date_time, time_str);
}