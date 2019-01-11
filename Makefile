CC = gcc
CFLAGS = -I./src -g -fPIC -Wall

RM = rm -f

SRCS := ./src/ini.c
OBJS := $(SRCS:.c=.o)

TEST_TARGET := test createINI parser

all: libini.so $(TEST_TARGET)

test: $(SRCS) ./test/test.c
	$(CC) -o ./test/$@ $^ $(CFLAGS)

createINI: $(SRCS) ./test/createINI.c
	$(CC) -o ./test/$@ $^ $(CFLAGS)

parser: $(SRCS) ./test/parser.c
	$(CC) -o ./test/$@ $^ $(CFLAGS)

libini.so: $(OBJS)
	$(CC) -shared -o $@ $^

clean:
	$(RM) libini.so $(OBJS)
	cd test && rm $(TEST_TARGET) && cd -

.PHONY: clean all test
