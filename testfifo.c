#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "my_alloc.h"
#include "static_fifo.h"

void fifo_put_int(static_fifo_t *p, int x) {
	assert(static_fifo_put(p, &x, sizeof x) == 0);
}

int fifo_get_int(static_fifo_t *p) {
	void *q = static_fifo_get(p);
	assert(q != NULL);
	int x = *(int *)q;
	my_free(q);
	return x;
}

void test_fifo(void) {
	static_fifo_t *q = static_fifo_create(10);
	assert(q != NULL);
	fifo_put_int(q, 34);
	fifo_put_int(q, 72);
	assert(fifo_get_int(q) == 34);
	assert(fifo_get_int(q) == 72);
	fifo_put_int(q, 1);
	fifo_put_int(q, 2);
	fifo_put_int(q, 3);
	assert(fifo_get_int(q) == 1);
	fifo_put_int(q, 4);
	fifo_put_int(q, 5);
	assert(fifo_get_int(q) == 2);
	assert(fifo_get_int(q) == 3);
	assert(fifo_get_int(q) == 4);
	assert(fifo_get_int(q) == 5);
}

int main(void) {
	test_fifo();
	return 0;
}
