/* Implémentation d'une horloge avec les KPN */
#define KPNLIB skpn
#include "kpn.h"
#include "my_alloc.h"
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

/* 
 * Envoie des tops d'horloge toutes les secondes, attend la réponse 
 * de l'afficheur.
 */
void *metronom(void *p) {
	kpn_chan_t *c = p;
	kpn_init();
	struct timeval prev, now;
	gettimeofday(&prev, NULL);
	
	for (;;) {
		gettimeofday(&now, NULL);
		unsigned long long nows = 1000 * 1000 * now.tv_sec + now.tv_usec;
		unsigned long long prevs = 1000 * 1000 * prev.tv_sec + prev.tv_usec;

		if (nows >= prevs + 1000 * 1000) {
			int yes = 1;
			puts("CLOCK");
			assert(kpn_put(&yes, sizeof yes, c->out) == 0);
			kpn_give();
			prev = now;
			void *data = NULL;
			assert(kpn_get(&data, c->in) == 0);
			puts("Written");
			my_free(data);
		}

		kpn_give();
	}

	kpn_end();
	return NULL;
}

/*
 * À chaque top d'horloge, génère la date sous une forme time_t et l'envoie
 * à l'afficheur.
 */
void *generator(void *p) {
	kpn_chan_t *c = p;
	kpn_init();

	for (;;) {
		void *data = NULL;
		assert(kpn_get(&data, c->in) == 0);
		puts("Generating time");
		my_free(data);
		time_t timer;
		time(&timer);
		assert(kpn_put(&timer, sizeof timer, c->out) == 0);
		kpn_give();
	}

	kpn_end();
}

/*
 * Reçoit la date du générateur, l'affiche sous un format agréable et envoie
 * un accusé de réception au métronome.
 */
void *printer(void *p) {
	kpn_chan_t *c = p;
	kpn_init();

	for (;;) {
		void *data;
		assert(kpn_get(&data, c->in) == 0);
		time_t timer = *(time_t *)data;
		my_free(data);
		char buffer[64];
		strftime(buffer, sizeof buffer, "%d-%m-%Y %H:%M:%S", localtime(&timer));
		puts(buffer);
		int yes = 1;
		assert(kpn_put(&yes, sizeof yes, c->out) == 0);
		kpn_give();
	}

	kpn_end();
}

int main(void) {
	kpn_chan_t *cmg = kpn_new_channel();
	kpn_chan_t *cgp = kpn_new_channel();
	kpn_chan_t *cpm = kpn_new_channel();
	kpn_proc_t procs[] = {metronom, generator, printer};	
	kpn_chan_t cm = {cpm->out, cmg->in};
	kpn_chan_t cg = {cmg->out, cgp->in};
	kpn_chan_t cp = {cgp->out, cpm->in};
	void *args[] = {&cm, &cg, &cp};
	kpn_doco(procs, args, 3);
	kpn_delete_channel(cmg);
	kpn_delete_channel(cgp);
	kpn_delete_channel(cpm);
	return 0;
}
