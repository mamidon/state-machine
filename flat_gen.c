#include "flat_gen.h"


/* Generates a flat rom binary image*/
void emit_binary() {}

/* Generates an address into the rom for the given parameters */
uint32_t emit_addr(size_t opcode, char zero, char interrupt, token transition) {
	return zero<<CHKZ_OFFSET | interrupt<<INT_OFFSET | opcode<<OP_OFFSET | next<<NEXT_OFFSET;
}

/* Generates the binary image */
void generate_binary(token* tokens, size_t count) {
	for(size_t i = 0; i < count; ) {
		size_t stage = get_stage(tokens, i, count-i);
		size_t trans = get_transition(tokens, stage, count-stage);
		signal_token signal = get_signal(tokens, stage, trans);
		
		generate_micro_state(tokens[stage], tokens[trans], signal);
		i += trans;
	}
}

/* Burns the appropriate signals at the appropriate indices in the ROM */
void generate_micro_state(token stage, token transition, signal_token signal) {
	uint32_t addr;
	//Each micro state will be repeated four times between chkZ and onInt
	//Changes in the microstate will vary based on the transition token
	
	// chkZ && onInt
	for(size_t i = 0; i < 4; i++) {
		addr = emit_addr(stage.state.state, i&2, i&1, transition);
		binary[addr] = signal;
	}
}

/* Gets the index for the next STAGE token, with count tokens left in the stream */
size_t get_stage(token* tokens, size_t begin, size_t count) {
	for(size_t i = begin; i < count; i++)
		if(tokens[i].type == STAGE)
			return i;
			
	printf("ERROR: Expected STAGE token\n");
	exit(0);
	return 0;
}

/* Gets the actual value of the signals for the given stage.
	It is assumed that all signals for each stage are found
	between the stage token and the corresponding transition token.
*/
signal_token get_signals(token* tokens, size_t stage, size_t transition) {
	signal_token ret = 0;
	for(size_t i = stage+1; i < transition; i++)
		if(tokens[i].type == SIGNAL) //should always be true
			ret |= tokens[i].signal;
	return ret;
}

/* Finds the next transition token, with count tokens left in the stream */
size_t get_transition(token* tokens, size_t begin, size_t count) {
	for(size_t i = begin; i < count; i++)
		if(tokens[i].type == TRANSITION)
			return i;
	
	printf("ERROR: Expected TRANSITION token\n");
	exit(0);
	return 0; //Shouldn't happen
}
