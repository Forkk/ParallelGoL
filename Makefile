CC=/bin/gcc
CFLAGS=-std=c11 -c -Wall -g
LDFLAGS=-lglut -lGL -lpthread -lm

EXECUTABLE=life

SOURCES=main.c grid.c grid_updater.c
OBJECTS=$(SOURCES:.c=.o)
DEPS=$(SOURCES:.c=.d)

all:  life

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c

clean:
	rm -f *.o $(EXECUTABLE)

distclean:
	rm -f *.d

# FIXME: This should run before anything else, since the makefile needs these files.
%.d: %.c
	@set -e; rm -f $@; \
		$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

include $(DEPS)

