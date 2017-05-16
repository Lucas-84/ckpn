#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "fifo.h"
#include "my_alloc.h"

struct node_t {
	void *data;
	size_t sz;
	struct node_t *next;	
};

struct fifo_t {
	struct node_t *first;
	struct node_t *last;
	int length;
	int maxlength;
};

fifo_t *fifo_create(int maxlength) {
	fifo_t *p = my_malloc(sizeof *p);
	p->first = p->last = NULL;
	p->length = 0;
	p->maxlength = maxlength;
	return p;
}

int fifo_empty(fifo_t *p) {
	return p->length == 0;
}

int fifo_put(fifo_t *p, void *data, size_t sz) {
	assert(sz > 0);

	if (p->length == p->maxlength)
		return -1;

	struct node_t *new = my_malloc(sizeof *new);
	new->data = my_malloc(sz);
	new->sz = sz;
	memcpy(new->data, data, sz);
	new->next = NULL;

	if (fifo_empty(p))
		p->first = new;
	else
		p->last->next = new;

	p->last = new;
	p->length++;
	return 0;
}

void *fifo_get(fifo_t *p) {
	if (fifo_empty(p))
		return NULL;

	void *data = my_malloc(p->first->sz);
	memcpy(data, p->first->data, p->first->sz);
	struct node_t *new = p->first->next;
	my_free(p->first->data);
	my_free(p->first);
	p->first = new;

	if (new == NULL)
		p->last = NULL;

	p->length--;
	return data;
}

void fifo_release(fifo_t *p) {
	void *data;

	while ((data = fifo_get(p)) != NULL)
		my_free(data);

	my_free(p);
}
