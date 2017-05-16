/* Implémentation basée sur les threads */
#ifndef TKPN_H_0136
#define TKPN_H_0136

typedef struct tkpn_port_t tkpn_port_t;

#define tkpn_init()
#define tkpn_give()
#define tkpn_end()

typedef struct {
	tkpn_port_t *in;
	tkpn_port_t *out;
} tkpn_chan_t;

typedef void *(*tkpn_proc_t)(void *);

tkpn_chan_t *tkpn_new_channel(void);
int tkpn_delete_channel(tkpn_chan_t *);
int tkpn_put(void *, size_t, tkpn_port_t *);
int tkpn_get(void **, tkpn_port_t *);
int tkpn_doco(tkpn_proc_t *, void **, size_t);

#endif
