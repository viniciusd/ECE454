/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include <string.h>

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
/*#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (&__board[(__i) + LDA*(__j)])
*/
/*

void kill(char *mycell, int i, int j, int nrows, int ncols)
{
	int xoleft, xoright, yoabove, yobelow;


	if (i == 0)
	{
		xoleft = ncols-1;
	} else
	{
		xoleft = -1;
	}
	if (j == 0)
	{
		yoabove = ncols*(nrows-1);
	} else
	{
		yoabove = -ncols;
	}
	if (i == (ncols-1))
	{
		xoright = -(ncols-1);
	} else
	{
		xoright = 1;
	}
	if (j == (nrows-1))
	{
		yobelow = ncols*(1-nrows);
	} else
	{
		yobelow = ncols;
	}
	*(mycell) &= ~0x01;
	*(mycell + yoabove + xoleft) -= 2;
	*(mycell + yoabove) -= 2;
	*(mycell + yoabove + xoright) -= 2;
	*(mycell + xoleft) -= 2;
	*(mycell + xoright) -= 2;
	*(mycell + yobelow + xoleft) -= 2;
	*(mycell + yobelow) -= 2;
	*(mycell + yobelow + xoright) -= 2;
}

void alive(char *mycell, int i, int j, int nrows, int ncols)
{
	int xoleft, xoright, yoabove, yobelow;


	if (i == 0)
	{
		xoleft = ncols-1;
	} else
	{
		xoleft = -1;
	}
	if (j == 0)
	{
		yoabove = ncols*(nrows-1);
	} else
	{
		yoabove = -ncols;
	}
	if (i == (nrows-1))
	{
		xoright = -(ncols-1);
	} else
	{
		xoright = 1;
	}
	if (j == (nrows-1))
	{
		yobelow = ncols*(1-nrows);
	} else
	{
		yobelow = ncols;
	}
	*(mycell) &= 0x01;
	*(mycell + yoabove + xoleft) += 2;
	*(mycell + yoabove) += 2;
	*(mycell + yoabove + xoright) += 2;
	*(mycell + xoleft) += 2;
	*(mycell + xoright) += 2;
	*(mycell + yobelow + xoleft) += 2;
	*(mycell + yobelow) += 2;
	*(mycell + yobelow + xoright) += 2;
}


char* sequential_game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
	const int LDA = nrows;
	int curgen, i, j;
	//char *pcell;
	char neighbor_count;

	for (curgen = 0; curgen < gens_max; curgen++)
	{
		memcpy(outboard, inboard, nrows*ncols);
		for (i = 0; i < nrows; i++)
		{
			j = 0;
			do
			{
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
				if (*outboard & 0x01)
				{
					if((neighbor_count != 2) && (neighbor_count != 3))
					{
						kill(BOARD(outboard, i, j), i,j, nrows, ncols);
					}
				} else
				{
					if (neighbor_count == 3)
					{
						alive(BOARD(outboard, i, j), i,j, nrows, ncols);
					}
				}
				++outboard;
			} while (++j < ncols);
		}
	} */
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     *//*
	SWAP_BOARDS(inboard, outboard);
	return inboard;
}

*/
