
#ifndef FLAT_GEN_H
#define FLAT_GEN_H

#include "flat_tokens.h"

#define FETCH_STEP_COUNT 10 //Defines the max number of micro steps for ifetch

//Offsets for the parameters that go into addressing the ROM
#define CHKZ_OFFSET 11
#define INT_OFFSET 10
#define OP_OFFSET 6
#define NEXT_OFFSET 0


uint32_t binary[(1<<12)-1]; //this represents the ROM


/* Generates a flat rom binary image*/
void emit_binary();

/* Generates an address into the rom for the given parameters */
uint32_t emit_addr(size_t opcode, char zero, char interrupt, char next);

/* Generates the binary image */
void generate_binary(token* tokens, size_t count);

/* Burns the appropriate signals at the appropriate indices in the ROM */
void generate_micro_state(token stage, token transition, signal_token signal);

/* Gets the index for the next STAGE token, with count tokens left in the stream */
size_t get_stage(token* tokens, size_t begin, size_t count);

/* Gets the actual value of the signals for the given stage.
	It is assumed that all signals for each stage are found
	between the stage token and the corresponding transition token.
*/
signal_token get_signals(token* tokens, size_t stage, size_t transition);

/* Finds the next transition token, with count tokens left in the stream */
size_t get_transition(token* tokens, size_t begin, size_t count);

#endif
