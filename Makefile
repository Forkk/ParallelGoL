CC=/bin/gcc
CFLAGS=-std=c11 -c -Wall -g
LDFLAGS=-lglut -lGL -lGLU -lpthread -lm

EXECUTABLE=life

SOURCES=main.c grid.c grid_updater.c life_file.c args.c
OBJECTS=$(SOURCES:.c=.o)

all:  life

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC)  $(CFLAGS) -c  $*.c -o $*.o
	@$(CC) $(CFLAGS) -MM $*.c  > $*.d

clean:
	rm -f *.{o,d} $(EXECUTABLE)


-include $(OBJECTS:.o=.d)

.PHONY: clean

