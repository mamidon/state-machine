CFLAGS = -g -std=c99 -O0
all: fsmrom

fsmrom: tokenizer.o flat_tokens.o flat_gen.o 
	gcc $(CFLAGS) -o fsmrom tokenizer.o flat_tokens.o flat_gen.o
	
tokenizer.o: tokenizer.c tokenizer.h 
	gcc $(CFLAGS) -c tokenizer.c
	
flat_tokens.o: flat_tokens.c flat_tokens.h
	gcc $(CFLAGS) -c flat_tokens.c
	
flat_gen.o: flat_gen.c flat_gen.h flat_tokens.o 
	gcc $(CFLAGS) -c flat_gen.c
	
arraylist.o: arraylist.c arraylist.h
	gcc $(CFLAGS) -c arraylist.c

.PHONY: clean
clean:
	rm -rf fsmrom assemble
