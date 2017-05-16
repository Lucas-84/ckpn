#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "static_fifo.h"
#include "my_alloc.h"
#include "pskpn.h"

struct pskpn_port_t {
	static_fifo_t *q;
	pthread_mutex_t m;	
	pthread_mutexattr_t attrm;
};

pskpn_chan_t *pskpn_new_channel(void) {
	pskpn_chan_t *c = my_malloc(sizeof *c);
	pskpn_port_t *p = my_malloc(sizeof *p);
	p->q = static_fifo_create(MAX_FIFO_SZ);

	if (pthread_mutexattr_init(&p->attrm) != 0 ||
		pthread_mutexattr_setpshared(&p->attrm, PTHREAD_PROCESS_SHARED) != 0 ||
		pthread_mutex_init(&p->m, &p->attrm) != 0)
		return NULL;

	c->in = c->out = p;
	return c;
}

int pskpn_delete_channel(pskpn_chan_t *c) {
	static_fifo_release(c->in->q);

	if (pthread_mutex_destroy(&c->in->m) != 0 ||
		pthread_mutexattr_destroy(&c->in->attrm) != 0)
		return -1;

	my_free(c->in);
	my_free(c);
	return 0;
}

int pskpn_put(void *data, size_t sz, pskpn_port_t *out) {
	int ret;

	do {
		if (pthread_mutex_lock(&out->m) != 0)
			return -1;
	
		ret = static_fifo_put(out->q, data, sz);

		if (pthread_mutex_unlock(&out->m) != 0)
			return -1;
	} while (ret == -1);

	return ret;
}

int pskpn_get(void **data, pskpn_port_t *in) {
	for (;;) {
		if (pthread_mutex_lock(&in->m) != 0)
			return -1;

		*data = static_fifo_get(in->q);

		if (pthread_mutex_unlock(&in->m) != 0)
			return -1;

		if (*data != NULL)
			break;

		pthread_yield();
	}

	return 0;
}

int pskpn_doco(pskpn_proc_t *a, void **args, size_t nb) {
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
