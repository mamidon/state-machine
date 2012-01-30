#include <strings.h>

#include "flat_tokens.h"
#include "tokenizer.h"
#include "arraylist.h"

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Usage: fsmrom filename\n");
		exit(0);
	}
	
	size_t count;
	arraylist alist;
	
	init_array(&alist, sizeof(token), 10);
	register_token_parsers(parsers, 3);
	parse_file(argv[1], &alist, &count);	
}

/*	Function which will parse out a token, returning 0 on success or non-zero on failure.
	buffer = string to be parsed.
	count = number of characters in token.
	type = type of the token in question.
*/
//typedef uint8_t (*parse_token) (char* buffer, size_t* count, token* type);

//Each state is '\w+\d+:' or '\w+\d+'
uint8_t parse_state (char* buffer, size_t* count, arraylist* alist) {
	token tok;
	tok.type = STATE;
	
	size_t len = strlen(opcodes[0]);
	for(size_t i = 0; i < OPCODE_COUNT; i++, len = strlen(opcodes[i]))
		if(strncasecmp(buffer, opcodes[i], len) == 0) {
			if(strncasecmp(buffer, opcodes[2], 4) == 0 && i == 0) { //Check for a substring issue
				i = 2;
				len = 4;
			}
			//Match found -- parse microstate
			tok.state.state = i;
			tok.state.step = atoi(&buffer[len]); 
			tok.state.step > 9 ? len+=2 : len++;
			
			if(strstr(&buffer[len], ":") == &buffer[len])
				len++; //If a ':' follows this token, lets drop it.
			*count = len;
			
			append_back(alist, (char*) &tok); 
			
			return 0;
		}
		*count = 0;
		return 1; //Failure
}

//Each signal is simply '\w+' 
uint8_t parse_signal (char* buffer, size_t* count, arraylist* alist) {
	token tok;
	tok.type = SIGNAL;
	
	size_t len = strlen(signals[0]);
	for(size_t i = 0; i < SIGNAL_COUNT; i++, len = strlen(signals[i])) 
		if(strncasecmp(buffer, signals[i], len) == 0) {
			tok.signal = 1<<i;
			*count = len;
			
			append_back(alist, (char*) &tok);
			return 0;
		}
		
	*count = 0;
	return 1; //Failure
}


/* This is a more complicated function.
	Lines are of one of the following formats:
	goto STATE 				[NORMAL]
	dispatch				[FETCH]
	onz STATE else STATE	[ZERO]
	onint STATE else STATE	[INTERRUPT]
	
	state parsing will take care every STATE, we need to catch
	GOTO, DISPATCH, ONZ, ONINT, and ELSE.  
*/
uint8_t parse_transition (char* buffer, size_t* count, arraylist* alist) {
	token tok;
	tok.type = TRANSITION;
	
	size_t len = strlen(signals[0]);
	for(size_t i = 0; i < TRANS_COUNT; i++, len = strlen(transitions[i])) {
		if(strncasecmp(buffer, transitions[i], len) == 0) {
			tok.transition = 1<<i; //Flip the appropriate bit
			*count = len;
			
			append_back(alist, (char*) &tok);
			return 0;
		}
	}
	
	*count = 0;
	return 1; //Failure
}























