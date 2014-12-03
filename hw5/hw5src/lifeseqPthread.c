/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "life.h"
#include "util.h"

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)]) // he does like this because he is using memory as a unique line,
																//so LDA will take care to jump between this lines
#define NUM_PTHREADS 1


typedef struct arguments{

	char* outboard;
	char* inboard;
	int nrows;
	int ncols;
	int gens_max;
	pthread_mutex_t lock;

} arguments;

arguments * struct_alloc_initialize();
void struct_alloc_set_fields(arguments * args, char* outboard, char* inboard, const int nrows, const int ncols, const int gens_max);

const pthread_mutex_t init = PTHREAD_MUTEX_INITIALIZER;

void *thread_start_routine(void *pthreadId);

char* sequential_game_of_life (char* outboard,
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
	//Initializing variables

	arguments *args;

	args=struct_alloc_initialize(); //allocate and initialize struct

	struct_alloc_set_fields(args,outboard,inboard,nrows,ncols,gens_max);//allocate and set fields of struct


    //creating NUM_PTHREADS
    pthread_t pool_wkthreads[NUM_PTHREADS];
    int tc;
    long i;

    for(i=0; i< NUM_PTHREADS; i++)
    {
    			tc = pthread_create(&pool_wkthreads[i], NULL,thread_start_routine, (void *)args);
    			if (tc)
    			{
    				printf("ERROR; return code from pthread_create() is %d\n", tc);
    				exit(-1);
    			}
    }
    for (i = 0; i < NUM_PTHREADS; ++i)
    {
    			pthread_join( pool_wkthreads[i], NULL);
    }

    return args->inboard;

    // *OBS: Depois de um free na funcao que recebe isso
}

arguments * struct_alloc_initialize()
{
	arguments *args= malloc (sizeof (arguments));

    if (args == NULL){
        printf("ERROR when trying to allocate struct!\n");
    	exit(1);
    }

    args->outboard=NULL;
    args->inboard=NULL;
    args->nrows=0;
    args->ncols=0;
    args->gens_max=0;
    args->lock=	init;

    return args;

}

void struct_alloc_set_fields(arguments * args, char* outboard, char* inboard, const int nrows, const int ncols, const int gens_max)
{
    args->outboard=outboard;
    args->inboard=inboard;

    //args->nrows= malloc (sizeof (int));
    args->nrows=nrows;

    //args->ncols=malloc (sizeof (int));
    args->ncols=ncols;

    //args->gens_max=malloc (sizeof (int));
    args->gens_max=gens_max;

    /*if ((args->nrows == NULL) || (args->ncols == NULL) || (args->gens_max == NULL)){
        printf("ERROR when trying to allocate one of the fields!\n");
    	exit(1);
    }*/

}


void *thread_start_routine(void *myargs)
{
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
	arguments *args = (arguments *) myargs;

	pthread_mutex_lock(&args->lock);

    const int LDA = args->nrows;
    int curgen, i, j;

    for (curgen = 0; curgen < args->gens_max; curgen++)
    {
        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
        for (i = 0; i < args->nrows; i++)
        {
            for (j = 0; j < args->ncols; j++)
            {
                const int inorth = mod (i-1, args->nrows);
                const int isouth = mod (i+1, args->nrows);
                const int jwest = mod (j-1, args->ncols);
                const int jeast = mod (j+1, args->ncols);

                const char neighbor_count = 
                    BOARD (args->inboard, inorth, jwest) +
                    BOARD (args->inboard, inorth, j) +
                    BOARD (args->inboard, inorth, jeast) +
                    BOARD (args->inboard, i, jwest) +
                    BOARD (args->inboard, i, jeast) +
                    BOARD (args->inboard, isouth, jwest) +
                    BOARD (args->inboard, isouth, j) +
                    BOARD (args->inboard, isouth, jeast);

                BOARD(args->outboard, i, j) = alivep (neighbor_count, BOARD (args->inboard, i, j));

            }
        }
        SWAP_BOARDS( args->outboard, args->inboard );

    }
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */

	pthread_mutex_unlock(&args->lock);

}


