/*****************************************************************************
* life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
	char *inboard;
	char *outboard;
	int nrows;
	int ncols;
	int gens_max;
	int i;
	pthread_barrier_t *barrier;
	pthread_mutex_t *lock;
} arguments;

/*void memcpy(char *dst, char *src, int n)
{
	while(n--)
	{
		*dst++ = *src++;
	}
}
*/

pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;

/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/
void kill(int initial_row, char *mycell, int i, int j, int nrows, int ncols, pthread_mutex_t *lock)
{
	const int up = (i==0) ? ncols*(nrows-1) : -ncols,
		down = (i==(nrows-1)) ? ncols*(1-nrows) : ncols,
		left = (j == 0) ? ncols-1 : -1,
		right = (j==(nrows-1)) ? 1-ncols : 1;
	if (i - 1 < initial_row)
	{
		pthread_mutex_lock(lock);
	}
	*(mycell + up + left) -= 2;
	*(mycell + up) -= 2;
	*(mycell + up + right) -= 2;
	if (i-1 < initial_row)
	{
		pthread_mutex_unlock(lock);
	}
	*(mycell + left) -= 2;
	*(mycell) &= ~0x01;
	*(mycell + right) -= 2;
	if (i + 1 > initial_row+nrows/THREADS)
	{
		pthread_mutex_lock(lock);
	}
	*(mycell + down + left) -= 2;
	*(mycell + down) -= 2;
	*(mycell + down + right) -= 2;
	if (i + 1 > initial_row+nrows/THREADS)
	{
		pthread_mutex_unlock(lock);
	}
}

void spawn(int initial_row, char *mycell, int i, int j, int nrows, int ncols, pthread_mutex_t *lock)
{
	const int up = (i==0) ? ncols*(nrows-1) : -ncols,
		down = (i==(nrows-1)) ? ncols*(1-nrows) : ncols,
		left = (j == 0) ? ncols-1 : -1,
		right = (j==(nrows-1)) ? 1-ncols : 1;
	if (i - 1 < initial_row)
	{
		pthread_mutex_lock(lock);
	}
	*(mycell + up + left) += 2;
	*(mycell + up) += 2;
	*(mycell + up + right) += 2;
	if (i - 1 < initial_row)
	{
		pthread_mutex_unlock(lock);
	}
	*(mycell + left) += 2;
	*(mycell) |= 0x01;
	*(mycell + right) += 2;
	if (i + 1 > initial_row+nrows/THREADS)
	{
		pthread_mutex_lock(lock);
	}
	*(mycell + down + left) += 2;
	*(mycell + down) += 2;
	*(mycell + down + right) += 2;
	if (i + 1 > initial_row+nrows/THREADS)
	{
		pthread_mutex_unlock(lock);
	}
}


/*****************************************************************************
 * Game of life implementation
 ****************************************************************************/
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max)
{
  return sequential_game_of_life (outboard, inboard, nrows, ncols, gens_max);
}

char* sequential_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
	const pthread_mutex_t init = PTHREAD_MUTEX_INITIALIZER; 
	arguments *args = (arguments *) malloc(sizeof(arguments));
	args->outboard = outboard;
	args->inboard = inboard;
	args->ncols = ncols;
	args->nrows = nrows;
	args->gens_max = gens_max;
	
	if(THREADS>0)
	{
		pthread_t pool_wkthreads[THREADS];
		int tc, i;
		//long i;
		
		pthread_barrier_t *barrier = (pthread_barrier_t *) malloc(sizeof(pthread_barrier_t));

		pthread_barrier_init(barrier, NULL, THREADS);
		args->barrier = barrier;
		args->lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
		*(args->lock) = init;
		for(i=0; i< THREADS; ++i)
		{
			pthread_mutex_lock(&global_lock);
			args->i = i;
			tc = pthread_create(&pool_wkthreads[i], NULL,thread_start_routine, (void *)args);
			if (tc)
			{
				printf("ERROR; return code from pthread_create() is %d\n", tc);
				exit(-1);
			}
		}
		for (i = 0; i < THREADS; ++i)
		{
			pthread_join( pool_wkthreads[i], NULL);
		}
	}

    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
	return inboard;
}

void *thread_start_routine(void *myargs)
{
	arguments *args = (arguments *) myargs;
	const int id = args->i;
	pthread_mutex_unlock(&global_lock);
	char *inboard = args->inboard;
	char *outboard = args->outboard;
	char *init = outboard;
	const int nrows = args->nrows;
	const int ncols = args->ncols;
	const int gens_max = args->gens_max;	
	//const int id = args->i;
	pthread_barrier_t *barrier = args->barrier;
	pthread_mutex_t *lock = args->lock;
	int curgen, i, j, k;
	char neighbor_count;
	for (curgen = 0; curgen < gens_max; curgen++, outboard = init)
	{
		if (id == 0)
		{
			memcpy(outboard, inboard, nrows*ncols);
		}
		pthread_barrier_wait(barrier);
		outboard += (nrows/THREADS)*(ncols*id);	
		for (i = (ncols*id)*(nrows/THREADS); i < (id+1)*(nrows/THREADS)*ncols; ++i)
		{	
			//for (; *outboard==0 && ++j < ncols; ++outboard);
			for (; *outboard==0 && i < (id+1)*(nrows/THREADS)*ncols; ++i, ++outboard);
			if (i >= (id+1)*(nrows/THREADS)*ncols)
			{
				continue;
			}
			neighbor_count = *outboard >> 1;
			if ((*outboard&0x01) == 0)
			{
				if(neighbor_count==3)
				{
					spawn(id*nrows/THREADS, inboard+i, i/ncols, i%ncols, nrows, ncols, lock);
				}
			} else if ((neighbor_count != 2) && (neighbor_count != 3))
			{
				kill(id*nrows/THREADS, inboard+i, i/ncols,i%ncols, nrows, ncols, lock);
			}
			++outboard;
		}
		pthread_barrier_wait(barrier);
	}
}
