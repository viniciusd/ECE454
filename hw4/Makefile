
CC = g++
CFLAGS = -O3

all: randtrack 

randtrack: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS) randtrack.cc -lpthread -fgnu-tm -o randtrack

randtrack_global_lock: list.h hash.h defs.h randtrack_global_lock.cc
	$(CC) $(CFLAGS) randtrack.cc -lpthread -fgnu-tm -o randtrack -D GLOBAL_LOCK

randtrack_list_lock: list.h hash.h defs.h randtrack_list_lock.cc
	$(CC) $(CFLAGS) randtrack.cc -lpthread -fgnu-tm -o randtrack -D LIST_LOCK

randtrack_tm: list.h hash.h defs.h randtrack_tm.cc
	$(CC) $(CFLAGS) randtrack_tm.cc -lpthread -fgnu-tm -o randtrack

clean:
	rm -f *.o randtrack randtrack_global_lock randtrack_tm randtrack_list_lock
