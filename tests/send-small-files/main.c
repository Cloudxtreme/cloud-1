/* File Cloud test
 *
 * Copyright (C) 2016 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence 3
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 */

#include "../../server/filetransfer.h"
#include "../../server/tcpclient.h"
#include "../../server/tcpserver.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/stat.h>


struct server_data {
	struct tcp_server *server;
	bool start_err;
};


void client_transfer_error(const char *message, void *data)
{
	bool *is_err = (bool *)data;
	*is_err = true;
	printf("[FAIL] %s\n", message);
}

void server_transfer_error(const char *message, void *data)
{
	bool *is_err = (bool *)data;
	*is_err = true;
	printf("[FAIL] %s\n", message);
}

void accept_error(void *data)
{
	bool *is_err = (bool *)data;
	*is_err = true;
	puts("[FAIL] Accepting new client error!");
}

void new_session(struct tcp_client *client, void *data)
{
	bool *is_err = (bool *)data;
	struct file_transfer ftransfer;

	file_transfer_init(&ftransfer, client);
	file_transfer_set_error_cb(&ftransfer, server_transfer_error, (void *)is_err);

	if (!file_transfer_recv_file(&ftransfer, "out/")) {
		*is_err = true;
		puts("[FAIL] Error receiving file.");
		return;
	}
	puts("[OK] File received.");
}

void *server_thread(void *data)
{
	struct server_data *sdata = (struct server_data *)data;
	if (!tcp_server_bind(sdata->server, 5000, 10)) {
		sdata->start_err = true;
	}
	return NULL;
}

void exit_fail()
{
	puts("-------------------------");
	puts("[ERROR] Test crashed!");
	rmdir("out");
	exit(-1);
}


int main(void)
{
	bool is_err = false;
	pthread_t srv_th;
	struct tcp_client client;
	struct tcp_server server;
	struct file_transfer ftransfer;

	file_transfer_init(&ftransfer, &client);
	file_transfer_set_error_cb(&ftransfer, client_transfer_error, (void *)&is_err);

	tcp_server_init(&server);
	tcp_server_set_newsession_cb(&server, new_session, &is_err);
	tcp_server_set_accepterr_cb(&server, accept_error, &is_err);

	struct server_data sdata;
	sdata.server = &server;
	sdata.start_err = false;
	pthread_create(&srv_th, NULL, server_thread, (void *)&sdata);
	pthread_detach(srv_th);

	puts("Starting test...");
	puts("-------------------------");
	mkdir("out", 0777);

	if (sdata.start_err) {
		puts("[FAIL] Error binding server.");
		exit_fail();
	}
	puts("[OK] Server started.");
	sleep(1);

	if (!tcp_client_connect(&client, "127.0.0.1", 5000)) {
		puts("[FAIL] Can not connect to server.");
		exit_fail();
	}
	puts("[OK] Client connected.");
	if (!file_transfer_send_file(&ftransfer, "test.txt")) {
		puts("[FAIL] Error sending file.");
		exit_fail();
	}
	sleep(1);
	tcp_client_close(&client);
	if (is_err)
		exit_fail();
	puts("-------------------------");
	puts("[PASSED] Test finished!");
	rmdir("out");
	return 0;
}