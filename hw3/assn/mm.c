/*
 * Our implementation utilize a circular linked list to store
 * the free blocks, using the last bit of the size as the marker
 * of allocation. We are using an explicit list, over which the
 * allocator iterates and find the first block that fits on the 
 * the research.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team */
    "HU3HU3HU3",
    /* First member's full name */
    "Arthur Borges de Oliveira Sousa",
    /* First member's email address */
    "arthur.borgesdeoliveirasousa@mail.utoronto.ca",
    /* Second member's full name (leave blank if none) */
    "Vinicius Dantas de Lima Melo",
    /* Second member's email address (leave blank if none) */
    "vinicius.dantasdelimamelo@mail.utoronto.ca"
};

/********************************************************
 * Struct for the header of each memmory block
 *******************************************************/

typedef struct header {
	size_t size;	
	struct header *next;
	struct header *prev;
} blockHdr;

/*************************************************************************
 * Basic Constants and Macros
 * You are not required to use these macros but may find them helpful.
*************************************************************************/
#define WSIZE       sizeof(void *)            /* word size (bytes) */
#define DSIZE       (2 * WSIZE)            /* doubleword size (bytes) */
#define CHUNKSIZE   (1<<7)      /* initial heap size (bytes) */

#define MAX(x,y) ((x) > (y)?(x) :(y))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)     (((p)->size) & ~(DSIZE - 1))
#define GET_ALLOC(p)    (((p)->size) & 0x1)

/* Some useful macros */
/* This first one aligns the given size, i.e., gives a multiple of WSIZE*/
#define ALIGN(size) (((size) + (DSIZE-1))& ~0xF)

/* This second one gives us the size of the block header*/
#define BLK_HDR_SIZE ALIGN(sizeof(blockHdr))

/* Define the block as used */
#define PLACE(bp) (bp->size |= 1)

/**********************************************************
 * mm_init
 * Initialize the heap, start a circular list at the first 
 * block of the heap
 **********************************************************/
 int mm_init(void)
 {
	blockHdr *p = mem_sbrk(BLK_HDR_SIZE);
	p->size = BLK_HDR_SIZE;
	p->next = p;
	p->prev = p;
 	return 0;
}

/**********************************************************
 * coalesce
 * Covers the 4 cases discussed in the text:
 * - both neighbours are allocated
 * - the next block is available for coalescing
 * - the previous block is available for coalescing
 * - both neighbours are available for coalescing
 **********************************************************/
void *coalesce(void *ptr)
{
	blockHdr *bp = ptr-BLK_HDR_SIZE;	
	size_t prev_alloc = GET_ALLOC(bp->prev);
	size_t next_alloc = GET_ALLOC(bp->next);

	if (prev_alloc && next_alloc)
	{       // Case 1
		return bp;
	}

	else if (prev_alloc && !next_alloc)
	{ // Case 2
		bp->size += GET_SIZE(bp->next);
		return (bp);
	}

	else if (!prev_alloc && next_alloc)
	{ // Case 3
		bp->prev->size += GET_SIZE(bp->prev);
		return (bp->prev);
	}

	else
	{            // Case 4
        	bp->prev->size += GET_SIZE(bp->prev)  + GET_SIZE(bp->next);
        	return (bp->prev);
	}
}

/**********************************************************
 * extend_heap
 * Extend the heap by "words" words, maintaining alignment
 * requirements of course. Reorganize the last block
 * and reallocate its new header
 **********************************************************/
void *extend_heap(size_t words)
{
	blockHdr *bp;
	size_t size;

	/* Allocate an even number of words to maintain alignments */
	size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
	if ( (bp = (blockHdr *)mem_sbrk(size)) == (void *)-1 )
		return NULL;

	/* Sets the size of the new last block and make the list circular again  */
 	bp->size = size;
	bp->next = mem_heap_lo();

	/* Coalesce if the previous block was free */
	return coalesce(bp);
}


/**********************************************************
 * find_fit
 * Traverse the heap searching for a free block to fit asize
 * Return NULL if no free blocks can handle that size
 * Assumed that asize is aligned
 **********************************************************/
blockHdr * find_fit(size_t asize)
{
	blockHdr *bp;
	for (bp = ((blockHdr *)mem_heap_lo())->next; bp !=  mem_heap_lo() && bp->size < asize; bp = bp->next);
	if(bp!=mem_heap_lo())
	{
		return bp;
	} else
	{
		return NULL;
	}
}

/**********************************************************
 * mm_free
 * Free the block and coalesce with neighbouring blocks
 **********************************************************/
void mm_free(void *ptr)
{
	blockHdr *bp = ptr-BLK_HDR_SIZE, *head=mem_heap_lo();
	bp->size &= ~1;
	bp->next = head->next;
	bp->prev = head;
	head->next = bp;
	bp->next->prev = bp;
}


/**********************************************************
 * mm_malloc
 * Allocate a block of size bytes.
 * The type of search is determined by find_fit
 * The decision of splitting the block, or not is determined
 *   in PLACE(..)
 * If no block satisfies the request, the heap is extended
 **********************************************************/
void *mm_malloc(size_t size)
{
	int newsize = ALIGN(BLK_HDR_SIZE + size);
	blockHdr *bp = find_fit(newsize);
	if (bp)
	{
		bp->size |= 1;
		bp->prev->next = bp->next;
		bp->next->prev = bp->prev;
	} else
	{
		bp = mem_sbrk(newsize);
		if((long)bp == -1)
		{
			return NULL;
		} else
		{
			bp->size = newsize | 1;
		}
	}
	return (char*) bp + BLK_HDR_SIZE;
}

/**********************************************************
 * mm_realloc
 * Implemented simply in terms of mm_malloc and mm_free
 *********************************************************/
void *mm_realloc(void *ptr, size_t size)
{
	blockHdr *bp = ptr-BLK_HDR_SIZE;
	void *newptr = mm_malloc(size);
	if(newptr == NULL)
	{
		return NULL;
	}
	int copySize = bp->size-BLK_HDR_SIZE;
	if (size < copySize)
	{
		copySize = size;
	}
	memcpy(newptr, ptr, copySize);
	mm_free(ptr);
	return newptr;
}

/**********************************************************
 * mm_check
 * Check the consistency of the memory heap
 * Return nonzero if the heap is consistant.
 *********************************************************/
int mm_check(void){
  return 1;
}
