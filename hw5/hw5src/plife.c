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
	const int nrows;
	const int ncols;
	const int gens_max;
	int i;
	pthread_barrier_t *barrier;
} arguments;


/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/
void kill(char *mycell, int i, int j, int nrows, int ncols)
{
	const int up = (i==0) ? ncols*(nrows-1) : -ncols,
		down = (i==(nrows-1)) ? ncols*(1-nrows) : ncols,
		left = (j == 0) ? ncols-1 : -1,
		right = (j==(nrows-1)) ? 1-ncols : 1;
	*(mycell + up + left) -= 2;
	*(mycell + up) -= 2;
	*(mycell + up + right) -= 2;
	*(mycell + left) -= 2;
	*(mycell) &= ~0x01;
	*(mycell + right) -= 2;
	*(mycell + down + left) -= 2;
	*(mycell + down) -= 2;
	*(mycell + down + right) -= 2;
}

void spawn(char *mycell, int i, int j, int nrows, int ncols)
{
	const int up = (i==0) ? ncols*(nrows-1) : -ncols,
		down = (i==(nrows-1)) ? ncols*(1-nrows) : ncols,
		left = (j == 0) ? ncols-1 : -1,
		right = (j==(nrows-1)) ? 1-ncols : 1;
	*(mycell + up + left) += 2;
	*(mycell + up) += 2;
	*(mycell + up + right) += 2;
	*(mycell + left) += 2;
	*(mycell) |= 0x01;
	*(mycell + right) += 2;
	*(mycell + down + left) += 2;
	*(mycell + down) += 2;
	*(mycell + down + right) += 2;
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
	int curgen, i, j;
	char neighbor_count;
	pthread_barrier_t bar;
	arguments *args = (arguments *) malloc(sizeof(arguments));
	args->outboard = outboard;
	args->inboard = inboard;
	args->ncols = ncols;
	args->nrows = nrows;
	args->gens_max = gens_max;
	
	if(THREADS>0)
	{
		pthread_t pool_wkthreads[num_threads];
		int tc;
		long i;
		
		pthread_barrier_t *barrier;

		pthread_barrier_init(barrier, NULL, THREADS);
		args->barrier = barrier;
		for(i=0; i< THREADS; ++i)
		{
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
	char *inboard = args->inboard;
	char *outboard = args->outboard;
	const int nrows = args->nrows;
	const int ncols = args->ncols;
	const int gens_max = args->gens_max;	
	const int id = args->i;
	pthread_barrier_t *barrier = args->barrier;
	for (curgen = 0; curgen < gens_max; curgen++, outboard -= nrows*ncols)
	{
		if (id == 0)
		{
			memcpy(outboard, inboard, nrows*ncols);
			if (curgen)
			{
				pthread_barrier_init(barrier, NULL, THREADS);
			}
		}
		for (i = id*nrows/THREADS; i < (id+1)*nrows/THREADS; ++i)
		{
			j = 0;
			do
			{
				//for (; *outboard==0 && ++j < ncols; ++outboard);
				while (*outboard==0)
				{
					++outboard;
					if (++j >= ncols)
					{
						break;
					}
				}
				if (j >= ncols)
				{
					break;
				}
				neighbor_count = *outboard >> 1;
				if ((*outboard&0x01) == 0)
				{
					if(neighbor_count==3)
					{
						spawn(BOARD(inboard, i, j), i, j, nrows, ncols);
					}
				} else if ((neighbor_count != 2) && (neighbor_count != 3))
				{
					kill(BOARD(inboard, i, j), i,j, nrows, ncols);
				}
				++outboard;
			} while (++j < ncols);
			pthread_barrier_wait(barrier);
		}
	}
}
