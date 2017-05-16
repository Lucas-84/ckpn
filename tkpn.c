#define _GNU_SOURCE
#include <pthread.h>
#include "fifo.h"
#include "my_alloc.h"
#include "tkpn.h"

struct tkpn_port_t {
	fifo_t *q;
	pthread_mutex_t m;	
};

tkpn_chan_t *tkpn_new_channel(void) {
	tkpn_chan_t *c = my_malloc(sizeof *c);
	tkpn_port_t *p = my_malloc(sizeof *p);
	p->q = fifo_create(-1);
	pthread_mutex_init(&p->m, NULL);
	c->in = c->out = p;
	return c;
}

int tkpn_close_port(tkpn_port_t *p) {
	fifo_release(p->q);
	return pthread_mutex_destroy(&p->m);
}

int tkpn_delete_channel(tkpn_chan_t *c) {
	if (tkpn_close_port(c->in) != 0)
		return -1;
	my_free(c->in);
	my_free(c);
	return 0;
}

int tkpn_put(void *data, size_t sz, tkpn_port_t *out) {
	if (pthread_mutex_lock(&out->m) != 0)
		return -1;
	if (fifo_put(out->q, data, sz) != 0)
		return -1;
	if (pthread_mutex_unlock(&out->m) != 0)
		return -1;
	return 0;
}

int tkpn_get(void **data, tkpn_port_t *in) {
	for (;;) {
		if (pthread_mutex_lock(&in->m) != 0)
			return -1;
		*data = fifo_get(in->q);
		if (pthread_mutex_unlock(&in->m) != 0)
			return -1;

		if (*data != NULL)
			break;	

		pthread_yield();
	}

	return 0;
}

int tkpn_doco(tkpn_proc_t *a, void **args, size_t nb) {
	pthread_t *ids = my_malloc(nb * sizeof *ids);

	for (size_t i = 0; i < nb; ++i)
		if (pthread_create(&ids[i], NULL, a[i], args[i]) != 0)
			return -1;

	for (size_t i = 0; i < nb; ++i)
		if (pthread_join(ids[i], NULL) != 0)
			return -1;

	my_free(ids);
	return 0;
}
