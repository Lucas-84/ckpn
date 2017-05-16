#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "my_alloc.h"

void *my_malloc(size_t sz) {
	void *p = malloc(sz);

	if (p == NULL) {
		fprintf(stderr, "Memory allocation error of %zu bytes\n", sz);
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	return p;
}

void *my_shared_malloc(size_t sz) {
	void *p = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);

	if (p == MAP_FAILED) {
		fprintf(stderr, "Shared memory allocation error of %zu bytes\n", sz);
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	return p;
}

void my_free(void *p) {
	free(p);
}
