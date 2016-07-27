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

#include <stdio.h>
#include <string.h>
#include "cloudserver.h"


int main(void)
{
	int ret_val;

	puts("Cloud server.");
	if (!cloud_server_set_log("cloudserver.log"))
		return -1;

	ret_val = cloud_server_load_cfg("cloudsrv.conf");
	if (ret_val != CFG_OK) {
		printf("%s", "Configs: ");

		switch(ret_val) {
			case CFG_FILE_NOT_FOUND:
				puts("file not found.");
				break;
			case CFG_SC_PORT_ERROR:
				puts("server port reading error.");
		}
		return -1;
	}

	if (!cloud_server_start()) {
		puts("Fail binding server port.");
		return -1;
	}
	return 0;
}