/* Implémentation basée sur processus + mémoire partagée */
#ifndef PSKPN_H_0909
#define PSKPN_H_0909

#define MAX_FIFO_SZ 10
#define pskpn_init()
#define pskpn_give()
#define pskpn_end()

typedef struct pskpn_port_t pskpn_port_t;

typedef struct {
	pskpn_port_t *in;
	pskpn_port_t *out;
} pskpn_chan_t;

typedef void *(*pskpn_proc_t)(void *);

pskpn_chan_t *pskpn_new_channel(void);
int pskpn_delete_channel(pskpn_chan_t *);
int pskpn_put(void *, size_t, pskpn_port_t *);
int pskpn_get(void **, pskpn_port_t *);
int pskpn_doco(pskpn_proc_t *, void **, size_t);

#endif
