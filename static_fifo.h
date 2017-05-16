/*
 * Implémentation d'une file de taille fixée. 
 * Elle est stockée en mémoire partagée.
 */
#ifndef STATIC_FIFO_H_0921
#define STATIC_FIFO_H_0921

/* Taille maximale d'un élément inséré. */
#define MAX_DATA_SZ 512

typedef struct static_fifo_t static_fifo_t;

/* Même interface que fifo.h */
static_fifo_t *static_fifo_create(int);
int static_fifo_empty(static_fifo_t *);
int static_fifo_put(static_fifo_t *, void *, size_t);
void *static_fifo_get(static_fifo_t *);
void static_fifo_release(static_fifo_t *);

#endif
