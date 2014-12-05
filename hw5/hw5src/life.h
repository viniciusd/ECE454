#ifndef _life_h
#define _life_h

#include <pthread.h>

#ifndef THREADS
#define THREADS 1 
#endif

#define BOARD( __board, __i, __j )  (__board + nrows*(__i) + (__j))

void *thread_start_routine(void *myargs);

/**
 * Given the initial board state in inboard and the board dimensions
 * nrows by ncols, evolve the board state gens_max times by alternating
 * ("ping-ponging") between outboard and inboard.  Return a pointer to 
 * the final board; that pointer will be equal either to outboard or to
 * inboard (but you don't know which).
 */
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max);

void kill(int initial_row, char *mycell, int i, int j, int nrows, int ncols, pthread_mutex_t *lock);
void spawn(int initial_row, char *mycell, int i, int j, int nrows, int ncols, pthread_mutex_t *lock);

/**
 * Same output as game_of_life() above, except this is not
 * parallelized.  Useful for checking output.
 */
char*
sequential_game_of_life (char* outboard, 
			 char* inboard,
			 const int nrows,
			 const int ncols,
			 const int gens_max);


#endif /* _life_h */
