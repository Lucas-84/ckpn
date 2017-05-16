ckpn:
	gcc $(file) tkpn.c fifo.c my_alloc.c ptkpn.c nkpn.c skpn.c coopsim.c static_fifo.c pskpn.c -Wall -Wextra -std=gnu99 -lpthread -o ckpn

clean:
	rm -f socket*
	rm -f ckpn 
