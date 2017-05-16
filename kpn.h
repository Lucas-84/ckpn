#ifndef KPN_H_0418
#define KPN_H_0418

#include <stdio.h>
#include "skpn.h"
#include "tkpn.h"
#include "pskpn.h"
#include "ptkpn.h"
#include "nkpn.h"

#define CONCAT2(x, y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)
#define PREFIX(x) CONCAT(KPNLIB, CONCAT(_, x))

#define kpn_init PREFIX(init)
#define kpn_give PREFIX(give)
#define kpn_get PREFIX(get)
#define kpn_end PREFIX(end)
#define kpn_get PREFIX(get)
#define kpn_put PREFIX(put)
#define kpn_new_channel PREFIX(new_channel)
#define kpn_delete_channel PREFIX(delete_channel)
#define kpn_doco PREFIX(doco)
#define kpn_chan_t PREFIX(chan_t)
#define kpn_port_t PREFIX(port_t)
#define kpn_proc_t PREFIX(proc_t)

#endif
