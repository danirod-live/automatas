CC=gcc

SRCS=hormiga.c
OBJS=hormiga.o
CFLAGS=-g -Wall
LDFLAGS=-framework SDL2

.PHONY:
	clean

hormiga: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f hormiga $(OBJS)
