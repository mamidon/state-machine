#include "flat_gen.h"


/* Generates a flat rom binary image*/
void emit_binary() {}

void print_stream(token* tokens, size_t begin, size_t end);

void print_stream(token* tokens, size_t begin, size_t end) {
	for(size_t i = begin; i < end; i++)
		if(tokens[i].type == STATE)
			printf("STAGE ");
		else if(tokens[i].type == SIGNAL)
			printf("SIGNAL ");
		else if(tokens[i].type == TRANSITION)
			printf("TRANS ");

	printf("\n\n\n");
}

/* Generates an address into the rom for the given parameters */
uint32_t emit_addr(size_t opcode, char zero, char interrupt, char next) {
	return zero<<CHKZ_OFFSET | interrupt<<INT_OFFSET | opcode<<OP_OFFSET | next<<NEXT_OFFSET;
}

/* Generates the binary image */
void generate_binary(token* tokens, size_t count) {
	for(size_t i = 0; i < count; ) {
		size_t stage = get_stage(tokens, i, count);
		size_t trans = get_transition(tokens, stage, count);
		signal_token signal = get_signals(tokens, stage, trans);

		i = generate_micro_state(stage, trans, signal, tokens);
		i++;
		printf("%i %i\n", i, count);
		print_stream(tokens, i, count);
	}
}

/* Burns the appropriate signals at the appropriate indices in the ROM */
size_t generate_micro_state(size_t stage, size_t trans, signal_token signal, token* tokens) {
	uint32_t addr;
  size_t lookahead = trans;

	//Look ahead into the stream and parse out transition targets
	switch(tokens[trans].transition.type) {
		case DISPATCH: //No look ahead
		break;
		
		case GOTO: //goto STATE
		tokens[trans].transition.target_a = tokens[trans+1].state;
		lookahead = trans + 1;
		break;

		case ONZ: //onz STATE else STATE
		case ONINT:
		tokens[trans].transition.target_a = tokens[trans+1].state;
		tokens[trans].transition.target_b = tokens[trans+3].state;
		lookahead = trans + 3;
		break;

		case ELSE: //Shouldn't actually parse this, error out.
		printf("Unexpected 'else' statement.\n");
		exit(0);
		break;
	}

	//Each micro state will be repeated four times between chkZ and onInt
	//Changes in the microstate will vary based on the transition token
	
	// chkZ && onInt
	token state = tokens[stage];

	if(state.state.opcode != OPCODE_COUNT-1) //If not IFETCH
		printf("");//generate_micro_inst(state, signal, transition);
	else
		generate_fetch_step(tokens[stage], signal, tokens[trans]);


	return lookahead;
}


void generate_fetch_step(token stage, signal_token signal, token trans) {
	printf("FETCH%i:\n", stage.state.step);
}

/* Gets the index for the next STAGE token, with count tokens left in the stream */
size_t get_stage(token* tokens, size_t begin, size_t end) {
	for(size_t i = begin; i < end; i++)
		if(tokens[i].type == STATE)
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
size_t get_transition(token* tokens, size_t begin, size_t end) {
	for(size_t i = begin; i < end; i++)
		if(tokens[i].type == TRANSITION)
			return i;
	
	printf("ERROR: Expected TRANSITION token\n");
	exit(0);
	return 0; //Shouldn't happen
}
