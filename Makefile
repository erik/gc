SRC := $(shell find . -name "*.c")
OBJ := $(SRC:.c=.o)
DEPS := $(SRC:.c=.d)

CC  := gcc

OPTIMIZE := -O2
LFLAGS := -lm
CFLAGS := -Wall -Wextra -pedantic -std=c99 -Wno-unused -Wno-unused-parameter
DFLAGS := -ggdb

.PHONY= all debug clean todo loc sloc check-syntax

all: $(OBJ)
	$(CC) $(OBJ) $(LFLAGS) -o test

-include $(SRC:.c=.d)

%.o: %.c
	$(CC) -c -MMD $(CFLAGS) $(OPTIMIZE) $< -o $@

debug: OPTIMIZE = -O0
debug: CFLAGS += -DDEBUG $(DFLAGS)
debug: all

clean:
	rm -f $(OBJ) $(DEPS) test

todo:
	@ack 'XXX|TODO|FIXME'
	@cat TODO

loc:
	@ack --type=cc -f | xargs wc -l | sort -h

check-syntax:
	$(CC) -o nul $(CFLAGS) -S $(CHK_SOURCES)
