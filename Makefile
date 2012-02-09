CFILES = $(shell find . -type f -name "*.c")
OBJFILES = $(shell find . -type f -name "*.o")

CFLAGS = -g -std=c99 -O0


all: fsmrom

fsmrom: $(CFILES) 
	gcc $(CFLAGS) -o fsmrom $(CFILES)

.PHONY: clean
clean:
	rm -rf fsmrom $(OBJFILES)

