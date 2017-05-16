#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include "fifo.h"
#include "my_alloc.h"
#include "nkpn.h"

nkpn_chan_t *nkpn_new_channel(void) {
	nkpn_chan_t *c = my_malloc(sizeof *c);
	c->in = my_malloc(sizeof *c->in);
	c->out = my_malloc(sizeof *c->out);
	int fd;
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return NULL;

	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	char name[64];
	int id_socket = 0;

	/* potential infinite loop here */
	do {
		snprintf(name, sizeof name, "socket%d", id_socket);
		id_socket++;
	} while (access(name, F_OK) != -1);

	strcpy(addr.sun_path, name);
	int yes = 1;

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0)
		return NULL;
	if (bind(fd, (struct sockaddr *)&addr, sizeof addr) < 0)
		return NULL;
	if (listen(fd, 1) < 0)
		return NULL;
	if ((*c->out = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return NULL;
	if (setsockopt(*c->out, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0)
		return NULL;
	if (connect(*c->out, (struct sockaddr *)&addr, sizeof addr) < 0)
		return NULL;
	if ((*c->in = accept(fd, NULL, NULL)) == -1)
		return NULL;
	return c;
}

int nkpn_delete_channel(nkpn_chan_t *c) {
	if (shutdown(*c->in, SHUT_RDWR) != 0)
		return -1;
	if (shutdown(*c->out, SHUT_RDWR) != 0)
		return -1;
	my_free(c->in);
	my_free(c->out);
	my_free(c);
	return 0;
}

int nkpn_put(void *data, size_t sz, nkpn_port_t *out) {
	if (write(*out, &sz, sizeof sz) != sizeof sz)
		return -1;

	return write(*out, data, sz) == (int)sz ? 0 : -1;
}

static void *read_nbytes(int fd, size_t n) {
	void *buffer = my_malloc(n);
	char *p = buffer;

	while (n > 0) {
		ssize_t r = read(fd, buffer, n);

		if (r == -1)
			return NULL;

		n -= r;
		p += r;
	}

	return buffer; 
}

int nkpn_get(void **data, nkpn_port_t *in) {
	size_t sz;

	if ((*data = read_nbytes(*in, sizeof sz)) == NULL)
		return -1;

	sz = *(size_t *)*data;
	my_free(*data);
	*data = read_nbytes(*in, sz);

	return *data == NULL ? -1 : 0;
}

int nkpn_doco(nkpn_proc_t *a, void **args, size_t nb) {
	for (size_t i = 0; i < nb; ++i) {
		int pid = fork();
		if (pid == -1)
			return -1;
		else if (pid == 0) {
			a[i](args[i]);
			exit(EXIT_SUCCESS);
		}
	}

	for (size_t i = 0; i < nb; ++i)
		if (wait(NULL) == -1)
			return -1;

	return 0;
}
