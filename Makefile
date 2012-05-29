## THIS IS A GENERIC MAKEFILE, NEEDS TO BE CHANGED TO ACTUALLY BUILD THIS PROJECT

SRC := $(shell find . -name "*.c")
OBJ := $(SRC:.c=.o)
DEPS := $(SRC:.c=.d)

CC  := gcc

DFLAGS := -ggdb -O0
LFLAGS := -lreadline -lm
CFLAGS := -Wall -Wextra -pedantic -std=c99 -Wno-unused -Wno-unused-parameter

.PHONY= all debug clean todo loc sloc check-syntax

all: $(OBJ)
	$(CC) $(OBJ) $(LFLAGS)

-include $(SRC:.c=.d)

%.o: %.c
	$(CC) -c -MMD $(CFLAGS) $< -o $@

debug:
	$(MAKE) all "CFLAGS=$(CFLAGS) $(DFLAGS)"

clean:
	rm -f $(OBJ) $(DEPS)

todo:
	@ack 'XXX|TODO|FIXME'
	@cat TODO

loc:
	@ack --type=cc -f | xargs wc -l | sort -h

sloc:
	@cloc .

check-syntax:
	$(CC) -o nul $(CFLAGS) -S $(CHK_SOURCES)
