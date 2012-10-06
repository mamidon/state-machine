CFILES=$(wildcard *.c)
OBJFILES=$(CFILES:%.c=%.o)
DEPFILES=$(OBJFILES:%.o=%.d)

CFLAGS = -g -std=c99 -O0 -MMD

%.o: %.c 
	gcc -c -o $@ $< $(CFLAGS)

fsmrom: $(OBJFILES)
	gcc -o $@ $^ $(CFLAGS)


-include $(DEPFILES)


.PHONY: clean
clean:
	rm -rf fsmrom $(OBJFILES) *.d

