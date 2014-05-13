CC=/bin/gcc
CFLAGS=-std=c11 -c -Wall -g
LDFLAGS=-l glut -l GL -lpthread

SOURCES=life.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=life

all: life

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c

clean:
	rm $(OBJECTS) $(EXECUTABLE)

