/*****************************************************************************
* life.c
* Parallelized and optimized implementation of the game of life resides here
* We changed the cell representation and use the own board to store the
* number of neighbors of each cell. This way we got a huge gain of
* performance. It is still possible to do some optimizations and get rid of * redundant variables. We should keep working on it despite the deadline.
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
	pthread_rwlock_t *lock;
} arguments;

pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;

/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/
 /************************************************************************
* The kill function broadcast to the neighbors saying that a specific cell
* has died. In addition, it changes the state of this cell.
 ************************************************************************/
void kill(int initial_row, char *mycell, int i, int j, int nrows, int ncols, pthread_rwlock_t *lock)
{
	const int up = (i==0) ? ncols*(nrows-1) : -ncols,
		down = (i==(nrows-1)) ? ncols*(1-nrows) : ncols,
		left = (j == 0) ? ncols-1 : -1,
		right = (j==(nrows-1)) ? 1-ncols : 1;
	if (i - 1 < initial_row)
	{
		pthread_rwlock_rdlock(lock);
	}
	*(mycell + up + left) -= 2;
	*(mycell + up) -= 2;
	*(mycell + up + right) -= 2;
	if (i-1 < initial_row)
	{
		pthread_rwlock_unlock(lock);
	}
	*(mycell + left) -= 2;
	*(mycell) &= ~0x01;
	*(mycell + right) -= 2;
	if (i + 1 > initial_row+nrows/THREADS)
	{
		pthread_rwlock_rdlock(lock);
	}
	*(mycell + down + left) -= 2;
	*(mycell + down) -= 2;
	*(mycell + down + right) -= 2;
	if (i + 1 > initial_row+nrows/THREADS)
	{
		pthread_rwlock_unlock(lock);
	}
}
 /************************************************************************
* The spawn function broadcast to the neighbors saying that a specific cell
* has born. In addition, it changes the state of this cell.
 ************************************************************************/
void spawn(int initial_row, char *mycell, int i, int j, int nrows, int ncols, pthread_rwlock_t *lock)
{
	const int up = (i==0) ? ncols*(nrows-1) : -ncols,
		down = (i==(nrows-1)) ? ncols*(1-nrows) : ncols,
		left = (j == 0) ? ncols-1 : -1,
		right = (j==(nrows-1)) ? 1-ncols : 1;
	if (i - 1 < initial_row)
	{
		pthread_rwlock_rdlock(lock);
	}
	*(mycell + up + left) += 2;
	*(mycell + up) += 2;
	*(mycell + up + right) += 2;
	if (i - 1 < initial_row)
	{
		pthread_rwlock_unlock(lock);
	}
	*(mycell + left) += 2;
	*(mycell) |= 0x01;
	*(mycell + right) += 2;
	if (i + 1 > initial_row+nrows/THREADS)
	{
		pthread_rwlock_rdlock(lock);
	}
	*(mycell + down + left) += 2;
	*(mycell + down) += 2;
	*(mycell + down + right) += 2;
	if (i + 1 > initial_row+nrows/THREADS)
	{
		pthread_rwlock_unlock(lock);
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

/*************************************************************************
* Here is the body of the function that creates the threads
*************************************************************************/
char* sequential_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
	const pthread_rwlock_t init = PTHREAD_RWLOCK_INITIALIZER; 
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
		args->lock = (pthread_rwlock_t *) malloc(sizeof(pthread_rwlock_t));
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

/**************************************************************************
* Here is the routine executed by each thread. The board is divided among
* them. It only works up to 2 threads for now.
**************************************************************************/
void *thread_start_routine(void *myargs)
{
	arguments *args = (arguments *) myargs;
	const int id = args->i;
	pthread_mutex_unlock(&global_lock);
	char *inboard = args->inboard;
	char *outboard = args->outboard;
	const char *init = outboard;
	const int nrows = args->nrows;
	const int ncols = args->ncols;
	const int gens_max = args->gens_max;	
	pthread_barrier_t *barrier = args->barrier;
	pthread_rwlock_t *lock = args->lock;
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
			for (; *outboard==0 && i < (id+1)*(nrows/THREADS)*ncols; ++i, ++outboard);
			if (i >= (id+1)*(nrows/THREADS)*ncols)
			{
				break;
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
