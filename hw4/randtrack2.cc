
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "defs.h"
#include "hash.h"

#define SAMPLES_TO_COLLECT   10000000
#define RAND_NUM_UPPER_BOUND   100000
#define NUM_SEED_STREAMS            4

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "HU3HU3HU3",                  /* Team name */

    "Arthur Borges de Oliveira Sousa",                    /* First member full name */
    "1001880084",                 /* First member student number */
    "arthur.borgesdeoliveirasousa@mail.utoronto.ca",                 /* First member email address */

    "Vinicius Dantas de Lima Melo",                           /* Second member full name */
    "1001879880",                           /* Second member student number */
    "vinicius.dantasdelimamelo@mail.utoronto.ca"                            /* Second member email address */
};

void *thread_start_routine(void *pthreadId);

unsigned num_threads;
unsigned samples_to_skip;

class sample;

class sample {
  unsigned my_key;
 public:
  sample *next;
  unsigned count;

  sample(unsigned the_key){my_key = the_key; count = 0;};
  unsigned key(){return my_key;}
  void print(FILE *f){printf("%d %d\n",my_key,count);}
};

// This instantiates an empty hash table
// it is a C++ template, which means we define the types for
// the element and key value here: element is "class sample" and
// key value is "unsigned".  
hash<sample,unsigned> h;

int  
main (int argc, char* argv[]){
  int i,j,k;
  int rnum;
  unsigned key;
  sample *s;

  // Print out team information
  printf( "Team Name: %s\n", team.team );
  printf( "\n" );
  printf( "Student 1 Name: %s\n", team.name1 );
  printf( "Student 1 Student Number: %s\n", team.number1 );
  printf( "Student 1 Email: %s\n", team.email1 );
  printf( "\n" );
  printf( "Student 2 Name: %s\n", team.name2 );
  printf( "Student 2 Student Number: %s\n", team.number2 );
  printf( "Student 2 Email: %s\n", team.email2 );
  printf( "\n" );

  // Parse program arguments
  if (argc != 3){
    printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
    exit(1);  
  }
  sscanf(argv[1], " %d", &num_threads); // not used in this single-threaded version
  sscanf(argv[2], " %d", &samples_to_skip);

  // initialize a 16K-entry (2**14) hash of empty lists
  h.setup(14);
 
  if(num_threads>0){

                 pthread_t pool_wkthreads[num_threads];

                int tc;
                long i;

                for(i=0; i< (num_threads); i++){

                    printf("creating thread %ld\n", i);


                    tc = pthread_create(&pool_wkthreads[i], NULL,thread_start_routine, (void *)i);

                    if (tc){
                       printf("ERROR; return code from pthread_create() is %d\n", tc);
                           exit(-1);
                    }
               }

  }

  // print a list of the frequency of all samples
  h.print();

}


  void *thread_start_routine(void *pthreadId)
  {

  	    int i,j,k;
            int rnum;
            unsigned key;
            sample *s;

	    long ptId;
	    ptId = (long)pthreadId;
	    printf("Thread %ld created\n", ptId);


	   while(1){

		 	  // process streams starting with different initial numbers
			  for (i=(NUM_SEED_STREAMS/num_threads)*ptId; i<(NUM_SEED_STREAMS/num_threads)*(ptId+1); i++){
			    rnum = i;

				    // collect a number of samples
				    for (j=0; j<SAMPLES_TO_COLLECT; j++){

					      // skip a number of samples
					      for (k=0; k<samples_to_skip; k++){
						rnum = rand_r((unsigned int*)&rnum);
					      }

					      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
					      key = rnum % RAND_NUM_UPPER_BOUND;

					      // if this sample has not been counted before
					      if (!(s = h.lookup(key))){
	
						// insert a new element for it into the hash table
						s = new sample(key);
						h.insert(s);
					      }

					      // increment the count for the sample
					      s->count++;
				    }

               		 }
	     }

        //caso queira otimizar, fa;a as threads pegando os processos em alternado

  } 
