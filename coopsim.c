#include <alloca.h>
#include <setjmp.h>
#include "coopsim.h"
#include "my_alloc.h"

/* Taille du cadrede pile alloue */
#define FRAME_SIZE (1 << 12)

/* Pour éviter que la glibc vienne fouiller dans notre bazar */
#ifdef __GLIBC__
void __libc_longjmp(jmp_buf, int);
#define longjmp __libc_longjmp
#endif

static jmp_buf *envs;
static int *active;
static int nb_procs;

static void *(*curr_proc)(void *);
static void *curr_arg;
static int id_curr;

void coop_take_control(int id_next) {
	int id_prev = id_curr;

	if (!setjmp(envs[id_curr])) {
		id_curr = id_next;
		longjmp(envs[id_next], 1);
		return;
	}

	id_curr = id_prev;
}

static void init_procs(int n) {
	if (n == nb_procs) {
		longjmp(envs[0], 1);
		return;
	}

	if (!setjmp(envs[n])) {
		void *frame = alloca(FRAME_SIZE);
		__asm__ volatile("" :: "m" (frame));
		init_procs(n + 1);
		return;
	} 

	if (n != 0) {
		curr_proc(curr_arg);
		active[n] = 0;
		coop_take_control(0);
	}
}

void coop_init_all(int n) {
	id_curr = 0;
	nb_procs = n + 1;
	envs = my_malloc(nb_procs * sizeof *envs);
	active = my_malloc(nb_procs * sizeof *active);
	active[0] = 1;
	for (int i = 1; i < nb_procs; i++)
		active[i] = 0;

	init_procs(0);
}

void coop_init_proc(void *(*proc)(void *), void *arg) {
	curr_proc = proc;
	curr_arg = arg;

	for (int i = 0; i < nb_procs; i++)
		if (!active[i]) {
			active[i] = 1;
			coop_take_control(i);
			return;        
		}
}
