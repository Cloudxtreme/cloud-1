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

#include "flist.h"
#include <stdlib.h>


static struct flist *flist_last(struct flist *restrict list)
{
    if (list)
        while (list->next)
            list = list->next;
    return list;
}


struct flist *flist_append(struct flist *list, struct file *file)
{
    struct flist *new_list;
    struct flist *last;

    new_list = (struct flist *)malloc(sizeof(new_list));
    new_list->file = file;
    new_list->next = NULL;

    if (list) {
        last = flist_last(list);
        last->next = new_list;
        return list;
    }
    return new_list;
}

struct file *flist_get_file(struct flist *list)
{
    return list->file;
}

struct flist *flist_next(struct flist *list)
{
    return list->next;
}

void flist_free_all(struct flist *list)
{
    struct flist *tmp;

    if (!list)
        return;

    while (list) {
        tmp = list;
        list = list->next;
        free(tmp);
    }
}