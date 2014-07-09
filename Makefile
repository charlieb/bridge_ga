CC=gcc
CFLAGS=-std=c99 -pedantic -Wall
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
DEST=.
EXE=GA
INCLUDES=`sdl2-config --cflags`
LIBS= `sdl2-config --libs` -lm -lSDL2_gfx

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

exe: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o $(DEST)/$(EXE)

debug: CFLAGS += -g
debug: exe

release: CFLAGS += -O2
release: exe

release_NOX: CFLAGS += -O2 -DNOX
release_NOX: LIBS = -lm
release_NOX: INCLUDES =
release_NOX: exe

debug_NOX: CFLAGS += -g -DNOX
debug_NOX: LIBS = -lm
debug_NOX: INCLUDES =
debug_NOX: exe

clean:
	@ - rm $(DEST)/$(EXE) $(OBJECTS)
