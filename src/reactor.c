#include "reactor.h"
#include "macros.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

static struct epoll_event *allocate_epoll_events(size_t event_count)
{
	if (unlikely(event_count == 0))
		return NULL;

	struct epoll_event *events = malloc(event_count * sizeof *events);

	if (unlikely(events == NULL)) {
		perror("malloc");
		return NULL;
	}

	printf("allocated %zu epoll events\n", event_count);

	return events;
}

static void free_epoll_events(struct epoll_event *events)
{
	if (unlikely(events == NULL))
		return;

	free(events);
}

int reactor_create(struct reactor *reactor, size_t capacity)
{
	if (unlikely(capacity == 0))
		return -1;

	int epfd = epoll_create1(0);

	if (unlikely(epfd == -1))
		return -errno;

	struct epoll_event *events = allocate_epoll_events(capacity);

	if (unlikely(events == NULL)) {
		close(epfd);
		return -1;
	}

	reactor->epfd = epfd;
	reactor->events = events;
	reactor->event_capacity = capacity;

	return 0;
}

/*
 * register intrested file descriptor and event into given reactor
 * returns:
 * -1 if fails
 *  0 if success
 * */
int reactor_register(struct reactor *reactor, int fd, uint32_t events)
{
	if (unlikely(events <= 0 || fd <= 0 || reactor->epfd <= 0))
		return -1;

	struct epoll_event event = { .events = events, .data.fd = fd };

	return epoll_ctl(reactor->epfd, EPOLL_CTL_ADD, fd, &event);
}

/*
 * polls event(s) from give reactor
 * return -1 if error otherwise returns N, where N == number of triggered events
 */
int reactor_poll(struct reactor *reactor, uint64_t timeout)
{
	return epoll_wait(reactor->epfd, reactor->events,
			  reactor->event_capacity, timeout);
}

/*
 * destroy the reactor
 */
void reactor_destroy(struct reactor *reactor)
{
	free_epoll_events(reactor->events);
}
