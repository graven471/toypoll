#include "server.h"
#include "macros.h"
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int create_server(struct server *server,
		  const struct server_configuration *config)
{
	server->fd = socket(config->domain, config->type, config->protocol);

	if (unlikely(server->fd) == -1) {
		perror("socket");
		return -1;
	}

	struct sockaddr_in addr = { .sin_family = AF_INET,
				    .sin_port = htons(config->port),
				    .sin_addr.s_addr = htonl(INADDR_ANY) };

	if (unlikely(bind(server->fd, (struct sockaddr *)&addr, sizeof(addr)) ==
		     -1)) {
		close(server->fd);
		perror("bind");
		return -1;
	}

	/*
   * we can only call listen on these two types of sockets
   */
	if (likely(config->type == SOCK_STREAM) ||
	    unlikely(config->type == SOCK_SEQPACKET)) {
		if (unlikely(listen(server->fd, config->queue_size == 0 ?
							1 :
							config->queue_size) ==
			     -1)) {
			close(server->fd);
			perror("listen");
			return -1;
		}
	}

	return 0;
}

void server_destroy(struct server *server)
{
	if (unlikely(server->fd < 0))
		return;

	close(server->fd);
}
