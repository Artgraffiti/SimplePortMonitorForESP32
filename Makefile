CC = g++
CFLAGS ?= -g -std=c++11
CFLAGS += -Wall -Werror -Wextra

SRCS = main.cpp

all: monitor

monitor: $(SRCS)
	$(CC) $^ -o $@ $(CFLAGS)

cif:
	clang-format -i main.cpp

clean:
	rm -rf monitor