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

	for (curgen = 0; curgen < gens_max; curgen++, outboard -= nrows*ncols)
	{
		memcpy(outboard, inboard, nrows*ncols);
		for (i = 0; i < nrows; i++)
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


