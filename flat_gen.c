#include <errno.h>
#include <stdio.h>

#include "flat_gen.h"

uint32_t binary[(1<<ROM_ADDR_BITS)-1];

/* Generates a flat rom binary image*/
void emit_binary() {}

/* Generates an address into the rom for the given parameters */
uint32_t emit_addr(size_t opcode, char zero, char interrupt, char step) {
	uint32_t tmp = zero<<CHKZ_OFFSET | interrupt<<INT_OFFSET | opcode<<OP_OFFSET 
					| step<<NEXT_OFFSET;

	return tmp;
}

/* Generates the binary image */
void generate_binary(macro_state* head) {
	memset(binary, 0, (1<<ROM_ADDR_BITS)-1);
	while(head != NULL) {
		generate_micro_state(head);
		head = head->next;
	}

	//Now write the binary out to file
	FILE* f = fopen("out.hex", "w");
	if(f == NULL) {
		printf("Error writing out binary! %i\n", errno);
		exit(0);
	}

	for(size_t i = 0; i < (1<<ROM_ADDR_BITS)-1; i++)
		fprintf(f, "%08X ", binary[i]);

	if(fclose(f) != 0) {
		printf("Close failed! %i\n", errno);
		exit(0);
	}
}

/* Burns the appropriate signals at the appropriate indices in the ROM */
void generate_micro_state(macro_state* state) {

	//Each micro state will be repeated four times between chkZ and onInt
	//Changes in the microstate will vary based on the transition token
	
	if(state->opcode == FETCH_OPCODE)
		generate_micro_fetch(state);
	else
		generate_micro_inst(state);
}


void generate_micro_fetch(macro_state* fetch_step) {
	macro_state st = *fetch_step;
	size_t a = st.transition.step_b; //Regular, dispatch, else
	size_t b = st.transition.step_a; //onint, onz

	for(size_t i = 0; i < FETCH_OPCODE; i++)
	switch(st.transition.type) {
		case(DISPATCH):
			burn_signals(i, st.step, st.signal, a, 0, a);
			break;
		case(GOTO):
			burn_signals(i, st.step, st.signal, a, 0, a);
			break;
		case(ONZ):
			burn_signals(i, st.step, st.signal, a, 1, b);
			break;
		case(ONINT):
			burn_signals(i, st.step, st.signal, a, 0, b);
			break;
	}

	return;
}

void generate_micro_inst(macro_state* inst_step) {
	macro_state st = *inst_step;
	st.step += FETCH_STEP_COUNT;
	size_t a = st.transition.step_a; //Regular, dispatch, else
	size_t b = st.transition.step_b; //onint, onz

	switch(st.transition.type) {
		case(DISPATCH):
			burn_signals(st.opcode, FETCH_STEP_COUNT, st.signal, a, 0, a);
			break;
		case(GOTO):
			burn_signals(st.opcode, st.step, st.signal, a, 0, a);
			break;
		case(ONZ):
			burn_signals(st.opcode, st.step, st.signal, a, 1, b);
			break;
		case(ONINT):
			burn_signals(st.opcode, st.step, st.signal, a, 0, b);
			break;
	}

	return;

}

void burn_signals(size_t opcode, size_t step, signal_token signals, 
									size_t reg, size_t is_onz, size_t on_branch) {
	for(size_t i = 0; i < 2; i++)
		for(size_t z = 0; z < 2; z++) {
			size_t br = is_onz && z || !is_onz && i ? on_branch : reg;
			binary[emit_addr(opcode, z, i, step)] = 
				(signals << SIG_OFFSET) | br;
	}
}






