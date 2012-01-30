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
const char* opcodes[OPCODE_COUNT+1] = {
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
  /*  1111 NA */ "FETCH", // Does not have an actual opcode, is special
  ""
};

/* Signals (start at NEXT_BITS bits from 0, 1 bit per signal) */
/* The actual signal value is (1<<i), where (i) is the index into signals */
const char *signals[SIGNAL_COUNT+1] = {
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
  "IntAck",
  ""
};

/* The various types of transitions */
const char* transitions[TRANS_COUNT+1] = {
	"dispatch",
	"goto",
	"onZ",
	"onInt",
	"else",
	""
};

//Valid types for our microcode
typedef enum token_type_ {
	STATE,
	SIGNAL,
	TRANSITION
} token_type;

//Each state has an opcode (state) and a microstep (step)
typedef struct state_token_ {
	size_t state;
	size_t step;
} state_token;

//Each signal has a specific bit in value
typedef uint32_t signal_token;

/*	There are four possible transition types.
	normal = increment by one, simply moving to the next microstate.
	zero = set next field to point to the the onZ state.
	interrupt = set next field to point to the onInt state.
	fetch = set next field to point to the first microstate.
*/
typedef enum transition_type_ {
	fetch=1,
	normal=2,
	zero=4,
	interrupt=8,
	not_taken=16
} transition_token;

//Pairs a given string with it's token type
typedef struct token_ {
	size_t type;
	union {
		transition_token transition;
		signal_token signal;
		state_token state;
	};
} token;


/*	Function which will parse out a token, returning 0 on success or non-zero on failure.
	buffer = string to be parsed.
	count = number of characters in token.
	type = type of the token in question.
*/
//typedef uint8_t (*parse_token) (char* buffer, size_t* count, token* type);

uint8_t parse_state (char* buffer, size_t* count, void* type);
uint8_t parse_signal (char* buffer, size_t* count, void* type);
uint8_t parse_transition (char* buffer, size_t* count, void* type);

parse_token parsers[3] = { &parse_state, &parse_signal, &parse_transition };

#endif
