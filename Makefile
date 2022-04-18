EXECUTABLE = h2o-builder
CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread
ZIP_FILE = proj2.zip
.PHONY: all pack clean

all: main.c
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $^

pack: $(ZIP_FILE)

$(ZIP_FILE): *.c *.h Makefile
	zip -j $@ $^

clean:
	rm -f $(EXECUTABLE) *.o $(ZIP_FILE)