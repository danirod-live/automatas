CC=gcc

OBJS=hormiga.o conway.o rps.o Viewer.o
CFLAGS=-g
LDFLAGS=-framework SDL2

.PHONY:
	clean

all: hormiga conway rps

hormiga: Viewer.o hormiga.o
	$(CC) -o $@ $(LDFLAGS) $^

conway: Viewer.o conway.o
	$(CC) -o $@ $(LDFLAGS) $^

ppt: Viewer.o ppt.o
	$(CC) -o $@ $(LDFLAGS) $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f hormiga conway $(OBJS)
