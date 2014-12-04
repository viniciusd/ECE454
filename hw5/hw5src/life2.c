/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include <string.h>
/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/
void kill(char *mycell, int i, int j, int nrows, int ncols)
{
	int up, down, left, right;
	if (j == 0)
	{
		left = ncols-1;
	} else
	{
		left = -1;
	}
	if (i == 0)
	{
		up = ncols*(nrows-1);
	} else
	{
		up = -ncols;
	}
	if (j == (nrows-1))
	{
		right = -(ncols-1);
	} else
	{
		right = 1;
	}
	if (i == (nrows-1))
	{
		down = ncols*(1-nrows);
	} else
	{
		down = ncols;
	}
	*(mycell) &= ~0x01;
	*(mycell + up + left) -= 2;
	*(mycell + up) -= 2;
	*(mycell + up + right) -= 2;
	*(mycell + left) -= 2;
	*(mycell + right) -= 2;
	*(mycell + down + left) -= 2;
	*(mycell + down) -= 2;
	*(mycell + down + right) -= 2;
}

void alive(char *mycell, int i, int j, int nrows, int ncols)
{
	int up, down, left, right;

	if (j == 0)
	{
		left = ncols-1;
	} else
	{
		left = -1;
	}
	if (i == 0)
	{
		up = ncols*(nrows-1);
	} else
	{
		up = -ncols;
	}
	if (j == (nrows-1))
	{
		right = 1-ncols;
	} else
	{
		right = 1;
	}
	if (i == (nrows-1))
	{
		down = ncols*(1-nrows);
	} else
	{
		down = ncols;
	}
	*(mycell) |= 0x01;
	*(mycell + up + left) += 2;
	*(mycell + up) += 2;
	*(mycell + up + right) += 2;
	*(mycell + left) += 2;
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
	//const int LDA = nrows;
	int curgen, i, j;
	//char *mycell;
	char neighbor_count;

	for (curgen = 0; curgen < gens_max; curgen++/*, outboard -= nrows*ncols*/)
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
						kill(BOARD(inboard, i, j), i,j, nrows, ncols);
					}
				} else
				{
					if (neighbor_count == 3)
					{
						alive(BOARD(inboard, i, j), i,j, nrows, ncols);
					}
				}
				++outboard;
			} while (++j < ncols);
		}
		outboard -= nrows*ncols;
	}
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
	return inboard;
}


