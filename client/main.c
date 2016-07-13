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

#include "cloudclient.h"
#include <stdio.h>
#include <string.h>


int main(void)
{
	char cmd[255];
	char login[100];
	char passwd[100];

	puts("Cloud client.");
	cloud_client_init();
	if (!cloud_client_set_log("/var/log/cloudclient.log")) {
		puts("Log path is to long!");
		return -1;
	}
	if (!cloud_client_load_cfg("cloudclient.cfg"))
		return -1;

	puts("Login:");
	if (scanf("%s", login) == 0) {
		puts("Incorrect login.");
		return -1;
	}

	puts("Password:");
	if (scanf("%s", passwd) == 0) {
		puts("Incorrect password.");
		return -1;
	}

	if (!cloud_client_login(login, passwd)) {
		puts("Fail. Bad login.");
		return -1;
	}

	puts("Starting cloud client...");
	cloud_client_start();

	for (;;) {
		printf("#>");
		if (scanf("%s", cmd) == 0)
			puts("Incorrect command.");
	}

	cloud_client_free();
	return 0;
}