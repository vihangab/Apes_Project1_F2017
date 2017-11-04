CC=gcc
CFLAGS= -g -O0 -pthread
LFLAGS= -lm -lrt

SSRCS= main.c sighand.c timer.c tempThread.c lightThread.c loggerThread.c initialise_queue.c
#headers=

PROGRAMS=msg_queue

#Default make command builds executable file
all: $(PROGRAMS)

msg_queue: $(SSRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

.PHONY: clean

clean:
	rm -rf $(PROGRAMS)