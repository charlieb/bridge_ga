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

NOX: INCLUDES =
NOX: CFLAGS += -DNOX
NOX: LIBS = -lm
NOX: EXE = GA_nox
NOX: exe

release_NOX: CFLAGS += -O2
release_NOX: NOX

debug_NOX: CFLAGS += -g
debug_NOX: NOX

profile_NOX: CFLAGS += -pg
profile_NOX: NOX

clean:
	@ - rm $(OBJECTS)
