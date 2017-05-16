/*
 * Génère les nombres premiers jusqu'à une borne maximale avec une méthode naïve.
 */
#define KPNLIB skpn
#include "kpn.h"
#include "my_alloc.h"
#include <assert.h>
#include <stdio.h>
#define UPPER_BOUND 100

/* Retourne 1 si n est premier, 0 sinon. */
int isprime(int n) {
	if (n == 2)
		return 1;
	else if (n % 2 == 0 || n < 2)
		return 0;

	for (int i = 3; i * i <= n; i += 2)
		if (n % i == 0)
			return 0;

	return 1;
}

/* Génère tous les entiers jusqu'à UPPER_BOUND sur le canal de sortie */
void *gen_integers(void *p) {
	kpn_init();
	kpn_port_t *out = p;

	for (int i = 0; i < UPPER_BOUND; i++) {
		assert(kpn_put(&i, sizeof i, out) == 0);
		kpn_give();
	}

	kpn_end();
	return NULL;
}

/* Récupère les entiers sur le canal d'entrée, et les affiche lorsqu'ils sont premiers */
void *gen_primes(void *p) {
	kpn_init();
	kpn_port_t *in = p;

	for (;;) {
		void *data;
		assert(kpn_get(&data, in) == 0);
		int x = *(int *)data;
		my_free(data);
		if (isprime(x))
			printf("%d\n", x);
	}

	kpn_end();
	return NULL;
}

int main(void) {
	kpn_chan_t *c = kpn_new_channel();
	kpn_proc_t procs[] = {gen_integers, gen_primes};
	void *args[] = {c->in, c->out};
	kpn_doco(procs, args, 2);
	kpn_delete_channel(c);
	return 0;
}
