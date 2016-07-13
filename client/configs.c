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

#include "configs.h"
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>


static struct {
    struct main_checker_cfg mcc;
    struct life_checker_cfg lcc;
} cfg;


static bool get_string_value(json_t *root, json_t *object, const char *value, char *out_str, unsigned size)
{
    json_t *jsobj;

    jsobj = json_object_get(object, value);
    if (jsobj == NULL) {
        json_decref(jsobj);
        json_decref(object);
        json_decref(root);
        return false;
    }
    strncpy(out_str, json_string_value(jsobj), size);
    json_decref(jsobj);
    return true;
}

static bool get_integer_value(json_t *root, json_t *object, const char *value, int *out_val)
{
    json_t *jsobj;

    jsobj = json_object_get(object, value);
    if (jsobj == NULL) {
        json_decref(jsobj);
        json_decref(object);
        json_decref(root);
        return false;
    }
    *out_val = json_integer_value(jsobj);
    json_decref(jsobj);
    return true;
}

bool configs_load(const char *filename)
{
    json_t *root;
    json_t *jdata;

    root = json_load_file(filename, 0, NULL);
    if (root == NULL) 
        return false;

    /*
     * Main checker timer cfg
     */
    jdata = json_object_get(root, "MainChecker");
    if (jdata == NULL) {
        json_decref(root);
        return false;
    }
    if (!get_integer_value(root, jdata, "Interval", (int *)&cfg.mcc.interval))
        return false;
    json_decref(jdata);

    /*
     * Life checker timer cfg
     */
    jdata = json_object_get(root, "LifeChecker");
    if (jdata == NULL) {
        json_decref(root);
        return false;
    }
    if (!get_integer_value(root, jdata, "Interval", (int *)&cfg.lcc.interval))
        return false;

    json_decref(jdata);
    json_decref(root);
    return true;
}

struct main_checker_cfg *configs_get_main_checker()
{
    return &cfg.mcc;
}

struct life_checker_cfg *configs_get_life_checker()
{
    return &cfg.lcc;
}