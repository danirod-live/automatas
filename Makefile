OS=$(shell uname -s)

CC=gcc

OBJS=hormiga.o conway.o ppt.o Viewer.o

ifeq ($(OS),Darwin)
CFLAGS=-g
LDFLAGS=-framework SDL2
endif

ifeq ($(OS),Linux)
CFLAGS=-g -std=c11
LDFLAGS=-lSDL2
endif

.PHONY:
	clean

all: hormiga conway ppt

hormiga: Viewer.o hormiga.o
	$(CC) -o $@ $(LDFLAGS) $^

conway: Viewer.o conway.o
	$(CC) -o $@ $(LDFLAGS) $^

ppt: Viewer.o ppt.o
	$(CC) -o $@ $(LDFLAGS) $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f hormiga conway ppt $(OBJS)
