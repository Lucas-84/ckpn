/* Implémentation basée sur les sockets */
#ifndef NKPN_H_0337
#define NKPN_H_0337

#define nkpn_init()
#define nkpn_give()
#define nkpn_end()

typedef int nkpn_port_t;

typedef struct {
	nkpn_port_t *in;
	nkpn_port_t *out;
} nkpn_chan_t;

typedef void *(*nkpn_proc_t)(void *);

nkpn_chan_t *nkpn_new_channel(void);
int nkpn_delete_channel(nkpn_chan_t *);
int nkpn_put(void *, size_t, nkpn_port_t *);
int nkpn_get(void **, nkpn_port_t *);
int nkpn_doco(nkpn_proc_t *, void **, size_t);

#endif
