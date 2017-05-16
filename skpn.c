#include <stdlib.h>
#include "fifo.h"
#include "my_alloc.h"
#include "skpn.h"

int skpn_sign;
int *skpn_next;
int *skpn_prev;
int *skpn_used;
int skpn_nb_envs;

skpn_chan_t *skpn_new_channel(void) {
	skpn_chan_t *c = my_malloc(sizeof *c);
	c->in = c->out = fifo_create(-1);
	return c;
}

int skpn_delete_channel(skpn_chan_t *c) {
	fifo_release(c->in);
	my_free(c);
	return 0;
}

int skpn_put(void *data, size_t sz, skpn_port_t *out) {
	return fifo_put(out, data, sz);
}

int __skpn_get(void **data, skpn_port_t *in, int __skpn_cnt) {
	while ((*data = fifo_get(in)) == NULL)
		skpn_give();

	return 0;
}

int skpn_doco(skpn_proc_t *a, void **args, size_t nb) {
	coop_init_all(nb);
	skpn_next = my_malloc(nb * sizeof *skpn_next);
	skpn_prev = my_malloc(nb * sizeof *skpn_prev);
	skpn_used = my_malloc(nb * sizeof *skpn_used);
	skpn_nb_envs = 0;	
	skpn_sign = +1;

	for (size_t i = 0; i < nb; i++) {
		skpn_next[i] = i + 1;
		skpn_prev[i] = (int)i - 1; 
		skpn_used[i] = 1;
	}

	for (size_t i = 0; i < nb; i++)
		coop_init_proc(a[i], args[i]);

	int found;
	do {
		found = 0;
		for (size_t i = 0; i < nb; i++)
			if (skpn_used[i]) {
				coop_take_control(i + 1);
				found = 1;
			}
	} while (found);
	return 0;
}
