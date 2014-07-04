CC=gcc
CFLAGS=-std=c99 -pedantic -Wall
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
DEST=.
EXE=GA
INCLUDES=`sdl2-config --cflags`
LIBS= `sdl2-config --libs` -lm

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

exe: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o $(DEST)/$(EXE)

debug: CFLAGS += -g
debug: exe

release: CFLAGS += -O2
release: exe

clean:
	@ - rm $(DEST)/$(EXE) $(OBJECTS)
