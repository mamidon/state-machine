#include <strings.h>

#include "flat_tokens.h"
#include "tokenizer.h"
#include "arraylist.h"
#include "flat_gen.h"

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Usage: fsmrom filename\n");
		exit(0);
	}
	
	size_t count;
	
	head = malloc(sizeof(macro_state));
	memset(head, 0, sizeof(macro_state));

	register_token_parsers(parsers, 3);
	parse_file(argv[1], &count);

	if(head == NULL) {
		printf("Nothing parsed!\n");
		exit(0);
	}

	//Drop the last (invalid) state
	macro_state* h = head->next;
	free(head);
	head = h;
	
	generate_binary(head);
}

/*	Function which will parse out a token, returning 0 on success or non-zero on failure.
	buffer = string to be parsed.
	count = number of characters in token.
	type = type of the token in question.
*/
//typedef uint8_t (*parse_token) (char* buffer, size_t* count, token* type);

//Each state is '\w+\d+:' or '\w+\d+'
uint8_t parse_state (char* buffer, size_t* count) {
	//Make sure we only update what we're supposed to.
	if(head->parse != EMPTY && head->parse != TRANSITION_FILLING)
		return 1;

	size_t index = best_match(buffer, opcodes, OPCODE_COUNT);
	if(index == OPCODE_COUNT)	
		return 1;

	*count = strlen(opcodes[index]);
	//Match found -- parse microstate
	size_t stp = atoi(buffer+*count); //Grab the micro number
	stp > 9 ? (*count)+= 2 : (*count)++;
	if(head->parse == EMPTY) {
		head->opcode = index;
		head->step = stp;
	} else if(head->parse == TRANSITION_FILLING) {
		transition_type t = head->transition.type;
		if(head->transition.first_step == EMPTY) {

			head->transition.step_a = stp + 
				(index == FETCH_OPCODE ? 0 : FETCH_STEP_COUNT);

			if(t == GOTO) {head->parse = TRANSITION_FILLED;} 
			else {head->transition.first_step = TRANSITION_FILLING;}
		} else {
			head->transition.step_b = stp + 
				(index == FETCH_OPCODE ? 0 : FETCH_STEP_COUNT);
			head->parse = TRANSITION_FILLED;
		}

		if(head->parse == TRANSITION_FILLED)
			push_macro_state();
		return 0;
	}		
			

	head->parse = STATE_FILLED;
			
	return 0;
}

//Each signal is simply '\w+' 
uint8_t parse_signal (char* buffer, size_t* count) {
	size_t len = 0;

	//Make sure we only update when we're supposed to
	if(head->parse != STATE_FILLED && head->parse != SIGNALS_FILLED)	
		return 1;

	size_t index = best_match(buffer, signals, SIGNAL_COUNT);
	if(index == SIGNAL_COUNT)
		return 1;		

	//Match found -- parse signal
	len = strlen(signals[index]);
	head->signal |= 1<<index;

	head->parse = SIGNALS_FILLED;

	*count = len;
	return 0; //Success
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
uint8_t parse_transition (char* buffer, size_t* count) {
	//Make sure we only update when were supposed to
	if(head->parse != SIGNALS_FILLED && head->parse != TRANSITION_FILLING)
		return 1;
			
	size_t index = best_match(buffer, transitions, TRANS_COUNT);
	if(index == TRANS_COUNT)
		return 1;

	head->transition.type = index;
	if(index == DISPATCH) {
		head->parse = TRANSITION_FILLED;
		head->transition.step_a = 
			head->transition.step_b = FETCH_STEP_COUNT;
		push_macro_state();
	} else //GOTO, ONZ, ONINT, ELSE
		head->parse = TRANSITION_FILLING;
	
		
	*count = strlen(transitions[index]);

	return 0; //Success
}

void push_macro_state() {
	macro_state* n = malloc(sizeof(macro_state));
	memset(n, 0, sizeof(macro_state));

	n->next = head;

	head = n;
}


const char* opcodes[OPCODE_COUNT] = {
  /* opcode     */
  /*  0000 ADD  */ "ADD",
  /*  0001 NAND */ "NAND",
  /*  0010 ADDI */ "ADDI",
  /*  0011 LW   */ "LW",
  /*  0100 SW   */ "SW",
  /*  0101 BEQ  */ "BEQ",
  /*  0110 JALR */ "JALR",
  /*  0111 HALT */ "HALT",
  /*  1000 BONR */ "BONR",
  /*  1001 BONO */ "BONO",
  /*  1010 EI   */ "EI",
  /*  1011 DI   */ "DI",
  /*  1100 RETI */ "RETI",
  /*  1101 BONI */ "BONI",
  /*  1110 BONJ */ "BONJ",
  /*  1111 NA */ "FETCH" // Does not have an actual opcode, is special
};

/* Signals (start at NEXT_BITS bits from 0, 1 bit per signal) */
const char *signals[SIGNAL_COUNT] = {
  "DrREG",
  "DrMEM",
  "DrALU",
  "DrPC",
  "DrOFF",
  "LdPC",
  "LdIR",
  "LdMAR",
  "LdA",
  "LdB",
  "LdZ",
  "WrREG",
  "WrMEM",
  "IntEn",
  "RegSelLo",
  "RegSelHi",
  "ALULo",
  "ALUHi",
  "LdIE",
  "IntAck"
};

/* The various types of transitions */
const char* transitions[TRANS_COUNT] = {
  "dispatch",
  "goto",
  "onZ",
  "onInt",
  "else"
};






parse_token parsers[3] = { &parse_transition, &parse_signal, &parse_state };
