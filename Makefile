all: fsmrom

fsmrom: tokenizer.c tokenizer.h flat_tokens.c flat_tokens.h
	gcc -g -std=c99 -O0 -o fsmrom tokenizer.c flat_tokens.c

.PHONY: clean
clean:
	rm -rf fsmrom assemble
