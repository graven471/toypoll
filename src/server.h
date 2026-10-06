#pragma once

#include <stdint.h>

struct server {
	int fd;
};

struct server_configuration {
	uint64_t queue_size;
	uint32_t port;
	int domain;
	int type;
	int protocol;
};

int create_server(struct server *server,
		  const struct server_configuration *config);
void server_destroy(struct server *server);
