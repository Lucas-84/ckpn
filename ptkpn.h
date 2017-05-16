/* Implémentation basée sur processus + tubes */
#ifndef PTKPN_H_0219
#define PTKPN_H_0219

#define ptkpn_init()
#define ptkpn_give()
#define ptkpn_end()

typedef int ptkpn_port_t;

typedef struct {
	ptkpn_port_t *in;
	ptkpn_port_t *out;
} ptkpn_chan_t;

typedef void *(*ptkpn_proc_t)(void *);

ptkpn_chan_t *ptkpn_new_channel(void);
int ptkpn_delete_channel(ptkpn_chan_t *);
int ptkpn_put(void *, size_t, ptkpn_port_t *);
int ptkpn_get(void **, ptkpn_port_t *);
int ptkpn_doco(ptkpn_proc_t *, void **, size_t);

#endif
