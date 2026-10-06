#pragma once

#include <stddef.h>
#include <stdint.h>

struct reactor {
	int epfd;
	struct epoll_event *events;
	size_t event_capacity;
};

int reactor_create(struct reactor *reactor, size_t capacity);
int reactor_register(struct reactor *reactor, int fd, uint32_t events);
int reactor_poll(struct reactor *reactor, uint64_t timeout);
void reactor_destroy(struct reactor *reactor);
