#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "fifo.h"
#include "my_alloc.h"
#include "ptkpn.h"

ptkpn_chan_t *ptkpn_new_channel(void) {
	ptkpn_chan_t *c = my_malloc(sizeof *c);
	c->in = my_malloc(sizeof *c->in);
	c->out = my_malloc(sizeof *c->out);
	int pipefd[2];

	if (pipe(pipefd) < 0)
		return NULL;

	*c->in = pipefd[1];
	*c->out = pipefd[0];
	return c;
}

int ptkpn_delete_channel(ptkpn_chan_t *c) {
	if (close(*c->in) != 0)
		return -1;
	if (close(*c->out) != 0)
		return -1;

	my_free(c->in);
	my_free(c->out);
	my_free(c);
	return 0;
}

int ptkpn_put(void *data, size_t sz, ptkpn_port_t *out) {
	ssize_t ret = 0;
	if ((ret = write(*out, &sz, sizeof sz)) != sizeof sz)
		return -1;
	if (write(*out, data, sz) != (int)sz)
		return -1;
	return 0;
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

int ptkpn_get(void **data, ptkpn_port_t *in) {
	size_t sz;
	*data = read_nbytes(*in, sizeof sz);
	if (*data == NULL)
		return -1;
	sz = *(size_t *)*data;
	my_free(*data);
	*data = read_nbytes(*in, sz);
	if (*data == NULL)
		return -1;
	return 0;
}

int ptkpn_doco(ptkpn_proc_t *a, void **args, size_t nb) {
	for (size_t i = 0; i < nb; ++i) {
		if (fork() == 0) {
			a[i](args[i]);
			exit(EXIT_SUCCESS);
		}
	}

	for (size_t i = 0; i < nb; ++i)
		wait(NULL);

	return 0;
}
