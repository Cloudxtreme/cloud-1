/* File Cloud project
 * Console application
 *
 * Copyright (C) 2016 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence 3
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 */

#include "../cloudclient.h"
#include <stdio.h>
#include <string.h>


void error_handle(const char *message, void *data)
{
	//puts(message);
}

void update_status_handle(bool status, void *data)
{
	puts("update status");
}


int main(void)
{
	int ret_val;
	char cmd[255];
	char login[100];
	char passwd[100];

	puts("Cloud client.");
	cloud_client_init();
	if (!cloud_client_set_log("cloudclient.log"))
		return -1;

	cloud_client_set_error_cb(error_handle, NULL);
	cloud_client_set_update_status_cb(update_status_handle, NULL);

	ret_val = cloud_client_load_cfg("../cloudclient.cfg");
	if (ret_val != CFG_OK) {
		printf("%s", "Configs: ");

		switch(ret_val) {
			case CFG_FILE_NOT_FOUND:
				puts("file not found.");
				break;
			case CFG_MCC_INTERVAL_ERROR:
				puts("main checker interval reading error.");
				break;
			case CFG_LCC_INTERVAL_ERROR:
				puts("life checker interval reading error.");
				break;
			case CFG_SC_IP_ERROR:
				puts("server ip reading error.");
				break;
			case CFG_SC_PORT_ERROR:
				puts("server port reading error.");
		}
		return -1;
	}

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

	ret_val = cloud_client_login(login, passwd);
	if (ret_val != LOGIN_OK) {
		printf("%s", "Fail login: ");

		switch(ret_val) {
			case LOGIN_FAIL:
				puts("bad login or password;");
				break;
			case LOGIN_LONG:
				puts("login is to long");
				break;
			case LOGIN_EMPTY:
				puts("login or password is empty.");
				break;
			case LOGIN_SHA_INIT_ERR:
				puts("sha512 init error.");
				break;
			case LOGIN_SHA_UPDATE_ERR:
				puts("sha512 update error.");
				break;
			case LOGIN_SHA_FINAL_ERR:
				puts("sha512 final error.");
				break;
			case LOGIN_CONNECTION_ERR:
				puts("connection to server error.");
				break;
			case LOGIN_SEND_ERR:
				puts("sending login error.");
				break;
			case LOGIN_ANSW_ERR:
				puts("receiving answare error.");
				break;
		}
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