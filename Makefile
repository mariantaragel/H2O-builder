EXECUTABLE = h2o-builder
CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
ZIP_FILE = proj2.zip
.PHONY: all pack

all: main.c
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $^

pack: $(ZIP_FILE)

$(ZIP_FILE): *.c *.h Makefile
	zip -j $@ $^

clean:
	rm -f $(EXECUTABLE) *.o $(ZIP_FILE)