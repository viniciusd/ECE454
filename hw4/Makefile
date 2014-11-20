
CC = g++
CFLAGS = -g

all: randtrack 

randtrack: list.h hash.h defs.h randtrack_list_lock.cc
	$(CC) $(CFLAGS) randtrack_list_lock.cc -lpthread -fgnu-tm -o randtrack


clean:
	rm -f *.o randtrack randtrack_global_lock randtrack_tm randtrack_list_lock
