CFLAGS = -Wall -Wextra -std=c99 -pthread -DLOG_MULTI_THREADED -O2
LDLIBS = -pthread

.PHONY: all
all: example

.PHONY: clean
clean:
	$(RM) example example.o logger.o

example: example.o logger.o

example.o: example.c logger.h
logger.o: logger.c logger.h
