#define _GNU_SOURCE

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "macros.h"
#include "reactor.h"
#include "server.h"

int main(void)
{
	struct server_configuration server_config = { .queue_size = 64,
						      .port = 8080,
						      .domain = AF_INET,
						      .type = SOCK_STREAM,
						      .protocol = 0 };

	struct server server = { 0 };

	if (unlikely(create_server(&server, &server_config) == -1)) {
		fprintf(stderr, "failed to create server");
		goto err_reactor;
	}

	struct reactor server_reactor = { 0 };

	if (unlikely(reactor_create(&server_reactor, 16) == -1)) {
		fprintf(stderr, "failed to create reactor for server");
		goto err_reactor;
	}

	printf("server_fd = %d\n", server.fd);
	printf("epfd      = %d\n", server_reactor.epfd);
	printf("listening on port %d\n", server_config.port);

	if (unlikely(reactor_register(&server_reactor, server.fd, EPOLLIN) ==
		     -1)) {
		fprintf(stderr, "failed to register reactor");
		goto err_register;
	}

	for (;;) {
		int n = reactor_poll(&server_reactor, -1);

		if (unlikely(n == -1)) {
			fprintf(stderr, "failed to poll server reactor");
			goto err_register;
		}

		printf("epoll_wait returned %d event(s)\n", n);

		for (int i = 0; i < n; i++) {
			int fd = server_reactor.events[i].data.fd;

			if (fd != server.fd)
				continue;

			int client_fd = accept(server.fd, NULL, NULL);

			if (client_fd == -1) {
				perror("accept");
				continue;
			}

			printf("client connected: fd=%d\n", client_fd);
		}
	}

	return 0;

err_register:
	reactor_destroy(&server_reactor);
err_reactor:
	server_destroy(&server);
	return -1;
}
