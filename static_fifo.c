#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "static_fifo.h"
#include "my_alloc.h"

struct node_t {
	void *data;
	size_t sz;
};

struct static_fifo_t {
	struct node_t *data;
	int head;
	int tail;
	int length;
	int maxlength;
};

static_fifo_t *static_fifo_create(int maxlength) {
	static_fifo_t *p = my_shared_malloc(sizeof *p);
	p->data = my_shared_malloc(maxlength * sizeof *p->data); 

	for (int i = 0; i < maxlength; i++)
		p->data[i].data = my_shared_malloc(MAX_DATA_SZ);

	p->maxlength = maxlength;
	p->length = 0;
	p->head = p->tail = 0;
	return p;
}

int static_fifo_empty(static_fifo_t *p) {
	return p->length == 0; 
}

int static_fifo_put(static_fifo_t *p, void *data, size_t sz) {
	if (sz <= 0 || sz > MAX_DATA_SZ || p->length == p->maxlength)
		return -1;

	memcpy(p->data[p->tail].data, data, sz);
	p->data[p->tail].sz = sz;
	p->tail = (p->tail + 1) % p->maxlength;
	p->length++;
	return 0;
}

void *static_fifo_get(static_fifo_t *p) {
	if (static_fifo_empty(p))
		return NULL;

	void *data = my_malloc(p->data[p->head].sz);
	memcpy(data, p->data[p->head].data, p->data[p->head].sz);
	p->head = (p->head + 1) % p->maxlength;
	p->length--;
	return data;
}

void static_fifo_release(static_fifo_t *p) {
	while (!static_fifo_empty(p))
		my_free(static_fifo_get(p));
}
