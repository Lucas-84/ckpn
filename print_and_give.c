/*
 * Incrémente un compteur de manière distribuée, où les processus se donnent
 * la main selon un ordre aléatoire fixé.
 */
#define KPNLIB tkpn
#include "kpn.h"
#include "my_alloc.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHECK(x) assert((x) == 0)
#define S_UPPER_BOUND 100
#define NB_PROCS 5

struct print_and_give_arg {
	int id;
	kpn_chan_t q;
};

void *print_and_give(void *p0) {
	kpn_init();
	struct print_and_give_arg *p = p0;
	kpn_chan_t q = p->q;
	int s = 0;

	for (;;) {
		void *data;
		CHECK(kpn_get(&data, q.in));
		s = *(int *)data;
		my_free(data);

		if (s > S_UPPER_BOUND)
			break;

		printf("#%d says %d\n", p->id, s);
		s++;
		CHECK(kpn_put(&s, sizeof s, q.out));
	}

	CHECK(kpn_put(&s, sizeof s, q.out));
	kpn_end();
	return NULL;
}

/* Mélange le tableau a, de taille n */
void shuffle(int *a, size_t n) {
	for (size_t i = 1; i < n; i++) {
		int j = rand() % i;
		int t = a[j];
		a[j] = a[i];
		a[i] = t;
	}
}

int main(void) {
	srand(time(NULL));

	kpn_proc_t procs[NB_PROCS];
	kpn_chan_t *chans[NB_PROCS];
	int next[NB_PROCS];
	int perm[NB_PROCS];

	for (int i = 0; i < NB_PROCS; i++)
		perm[i] = i;

	shuffle(perm, NB_PROCS);

	for (int i = 0; i < NB_PROCS; i++)
		next[perm[i]] = perm[(i + 1) % NB_PROCS];

	struct print_and_give_arg rargs[NB_PROCS];
	void *args[NB_PROCS];

	for (int i = 0; i < NB_PROCS; i++) {
		procs[i] = print_and_give;
		assert((chans[i] = kpn_new_channel()) != NULL);
	}

	for (int i = 0; i < NB_PROCS; i++) {
		rargs[i].id = i;
		rargs[i].q = (kpn_chan_t){chans[i]->out, chans[next[i]]->in};
		args[i] = &rargs[i];
	}

	int init = 0;
	CHECK(kpn_put(&init, sizeof init, chans[rand() % NB_PROCS]->in));
	CHECK(kpn_doco(procs, args, NB_PROCS));

	for (int i = 0; i < NB_PROCS; i++)
		CHECK(kpn_delete_channel(chans[i]));

	return 0;
}
