CC = g++
CFLAGS ?= -g -std=c++11
CFLAGS += -Wall -Werror -Wextra

all: monitor

monitor: monitor.cpp
	$(CC) $^ -o $@ $(CFLAGS)

monitor_thread: monitor_thread.cpp
	$(CC) $^ -o $@ $(CFLAGS)

cif:
	clang-format -i monitor.cpp monitor_thread.cpp

clean:
	rm -rf monitor monitor_thread