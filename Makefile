EXECUTABLE = proj2
CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
LDLIBS = -pthread
ZIP_FILE = proj2.zip
.PHONY: all pack clean

all: main.c
	$(CC) $(CFLAGS) $(LDLIBS) -o $(EXECUTABLE) $^

pack: $(ZIP_FILE)

$(ZIP_FILE): *.c *.h Makefile
	zip -j $@ $^

clean:
	rm -f $(EXECUTABLE) *.o $(ZIP_FILE) *.out proj2.out.strace