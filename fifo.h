/* Files non bornées */
#ifndef FIFO_H_0037
#define FIFO_H_0037

typedef struct fifo_t fifo_t;

/* Crée une file */
fifo_t *fifo_create(int);
/* Renvoie 1 si la file est vide, 0 sinon */
int fifo_empty(fifo_t *);
/* Insère un élément de taille donnée dans la file */
int fifo_put(fifo_t *, void *, size_t);
/* Récupère un élément depuis la file. La mémoire doit être libérée après utilisation */
void *fifo_get(fifo_t *);
/* Libère toute la mémoire utilisée dans la file */
void fifo_release(fifo_t *);

#endif
