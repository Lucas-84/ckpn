/* Implémentation basée sur un parallélisme simulé */
#ifndef SKPN_H_0415
#define SKPN_H_0415

#include "coopsim.h"
#include "fifo.h"

#define skpn_init() \
	int __skpn_cnt = skpn_nb_envs++; \
	coop_take_control(0); 

#define skpn_give() \
	do { \
		if ((skpn_prev[__skpn_cnt] < 0 && skpn_sign == -1) || (skpn_next[__skpn_cnt] >= skpn_nb_envs && skpn_sign == +1)) \
			skpn_sign = -skpn_sign; \
		if (skpn_sign == +1 && skpn_next[__skpn_cnt] < skpn_nb_envs) \
			coop_take_control(skpn_next[__skpn_cnt] + 1); \
		else if (skpn_sign == -1 && skpn_prev[__skpn_cnt] >= 0) \
			coop_take_control(skpn_prev[__skpn_cnt] + 1); \
	} while (0)

#define skpn_end() \
	do { \
		skpn_next[skpn_prev[__skpn_cnt]] = skpn_next[__skpn_cnt]; \
		skpn_prev[skpn_next[__skpn_cnt]] = skpn_prev[__skpn_cnt]; \
		skpn_used[__skpn_cnt] = 0; \
	} while (0)

#define skpn_get(data, in) (__skpn_get((data), (in), __skpn_cnt))

typedef fifo_t skpn_port_t;

typedef struct {
	skpn_port_t *in;
	skpn_port_t *out;
} skpn_chan_t;

typedef void *(*skpn_proc_t)(void *);

skpn_chan_t *skpn_new_channel(void);
int skpn_delete_channel(skpn_chan_t *);
int __skpn_get(void **, skpn_port_t *, int);
int skpn_put(void *, size_t, skpn_port_t *);
int skpn_doco(skpn_proc_t *, void **, size_t);

/* non reentrant */
extern int skpn_sign;
extern int *skpn_next;
extern int *skpn_prev;
extern int *skpn_used;
extern int skpn_nb_envs;

#endif
