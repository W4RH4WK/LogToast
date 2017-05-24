CFLAGS = -Wall -Wextra -std=c99 -O2

.PHONY: all
all: example

.PHONY: clean
clean:
	$(RM) example example.o logger.o

example: example.o logger.o

example.o: example.c logger.h
logger.o: logger.c logger.h
