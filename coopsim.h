/* Implémente un multitâche coopératif */
#ifndef COOPSIM_H_0842
#define COOPSIM_H_0842

/* Donne la main au processus d'id donné */
void coop_take_control(int);
/* Initialise le processus d'argument donné */
void coop_init_proc(void *(*)(void *), void *);
/* Initialise le système avec un nb de processus fixé */
void coop_init_all(int);

#endif
