CC=gcc
CFLAGS= -g -O0 -pthread
LFLAGS= -lm -lrt

SSRCS=
#headers=

PROGRAMS=test

#Default make command builds executable file
all: $(PROGRAMS)

test: $(SSRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

.PHONY: clean

clean:
	rm -rf $(PROGRAMS)
