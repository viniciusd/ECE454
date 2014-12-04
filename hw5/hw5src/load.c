#include "load.h"
#include "life.h"
#include <assert.h>
#include <stdlib.h>

void birth(char *mycell, int i, int j, int nrows, int ncols)
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

char*
make_board (const int nrows, const int ncols)
{
  char* board = NULL;
  int i;

  /* Allocate the board and fill in with 'Z' (instead of a number, so
     that it's easy to diagnose bugs */
  board = malloc (nrows * ncols * sizeof (char));
  assert (board != NULL);
  for (i = 0; i < nrows * ncols; i++)
    board[i] = 0;

  return board;
}

static void
load_dimensions (FILE* input, int* nrows, int* ncols)
{
  int ngotten = 0;
  
  ngotten = fscanf (input, "P1\n%d %d\n", nrows, ncols);
  if (ngotten < 1)
    {
      fprintf (stderr, "*** Failed to read 'P1' and board dimensions ***\n");
      fclose (input);
      exit (EXIT_FAILURE);
    }
  if (*nrows < 1)
    {
      fprintf (stderr, "*** Number of rows %d must be positive! ***\n", *nrows);
      fclose (input);
      exit (EXIT_FAILURE);
    }
  if (*ncols < 1)
    {
      fprintf (stderr, "*** Number of cols %d must be positive! ***\n", *ncols);
      fclose (input);
      exit (EXIT_FAILURE);
    }
}

static char*
load_board_values (FILE* input, const int nrows, const int ncols)
{
	char* board = NULL;
	char entry;
	int ngotten = 0;
	int i = 0;
	
	/* Make a new board */
	board = make_board (nrows, ncols);
	
	/* Fill in the board with values from the input file */
	for (i = 0; i < nrows * ncols; i++)
	{
		ngotten = fscanf (input, "%c\n", &entry);
		if (ngotten < 1)
		{
			fprintf (stderr, "*** Ran out of input at item %d ***\n", i);
			fclose (input);
			exit (EXIT_FAILURE);
		}
		else
		{
		/* If it is a live cell, let us broadcast it to its neighbor cells*/	
			if(entry == '1')
			{
				birth(&board[i], i/ncols,i%ncols, nrows, ncols);
			}
		}

	}	
  return board;
}

char*
load_board (FILE* input, int* nrows, int* ncols)
{
  load_dimensions (input, nrows, ncols);
  return load_board_values (input, *nrows, *ncols);
}


