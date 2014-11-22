CC = g++
CFLAGS = -O3

all: randtrack 

randtrack: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS) randtrack.cc -o randtrack

randtrack_global_lock: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS) randtrack.cc -lpthread -o randtrack_global_lock -D GLOBAL_LOCK

randtrack_tm: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS) randtrack.cc -lpthread -fgnu-tm -o randtrack_tm -D TRANSACTIONAL_MEMORY

randtrack_list_lock: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS) randtrack.cc -lpthread -o randtrack_list_lock -D LIST_LOCK

clean:
	rm -f *.o randtrack randtrack_global_lock randtrack_tm randtrack_list_lock
