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
	cloud_client_init();
	if (!cloud_client_set_log("/var/log/cloudclient.log")) {
		puts("Log path is to long!");
		return -1;
	}
	if (!cloud_client_load_cfg("cloudclient.cfg"))
		return -1;

	cloud_client_start();

	for (;;) {
		sleep(1);
	}

	cloud_client_free();
	return 0;
}