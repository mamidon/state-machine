#ifndef FLAT_TOKENS_H
#define FLAT_TOKENS_H

#include "tokenizer.h"

//Each opcode has 64 microstates, indexed by the 
//least significant bits addressing the ROM
#define NEXT_BITS 6

//The total number of signals per microstate
#define SIGNAL_COUNT 20

//The total number of opcodes (IN USE)
#define OPCODE_COUNT 16

//The total number of transition token types
#define TRANS_COUNT 5

/* Opcodes by instruction [opcode is the index into array] */
const char* opcodes[OPCODE_COUNT+1];

/* Signals (start at NEXT_BITS bits from 0, 1 bit per signal) */
/* The actual signal value is (1<<i), where (i) is the index into signals */
const char* signals[SIGNAL_COUNT+1];

/* The various types of transitions */
const char* transitions[TRANS_COUNT+1];

//Each signal has a specific bit in value
typedef uint32_t signal_token;

/*	There are four possible transition types.
	normal = increment by one, simply moving to the next microstate.
	zero = set next field to point to the the onZ state.
	interrupt = set next field to point to the onInt state.
	fetch = set next field to point to the first microstate.
*/
typedef enum transition_type_ {
	DISPATCH,
	GOTO,
	ONZ,
	ONINT,
	ELSE
} transition_type;

typedef enum current_parse_ {
	EMPTY,
	STATE_FILLED,
	SIGNALS_FILLED,
	TRANSITION_FILLING,
	TRANSITION_FILLED,
} current_parse;

/* Indicates the targets of any given transition */
typedef struct transition_token_ {
	transition_type type;
	current_parse first_step;
	size_t step_a;
	size_t step_b; 
} transition_token;

/* Represents a full state */
typedef struct macro_state_ {
	size_t opcode;
	size_t step;
	signal_token signal;
	transition_token transition;
	struct macro_state_* next;
	current_parse parse; //If incompletely parsed
} macro_state;


/*	Function which will parse out a token, returning 0 on success or non-zero on failure.
	buffer = string to be parsed.
	count = number of characters in token.
	type = type of the token in question.
*/
//typedef uint8_t (*parse_token) (char* buffer, size_t* count, token* type);

uint8_t parse_state (char* buffer, size_t* count);
uint8_t parse_signal (char* buffer, size_t* count);
uint8_t parse_transition (char* buffer, size_t* count);
void push_macro_state();

parse_token parsers[3];
macro_state* head;

#endif
