#ifndef FLAT_GEN_H
#define FLAT_GEN_H

#include "flat_tokens.h"

 //Defines the max number of micro steps for ifetch
#define FETCH_STEP_COUNT 10

//Offsets for the parameters that go into addressing the ROM
#define CHKZ_OFFSET 12
#define INT_OFFSET 11
#define OP_OFFSET 7
#define NEXT_OFFSET 0
#define ROM_ADDR_BITS (CHKZ_OFFSET+1)

//Offsets for values that go into the ROM
#define SIG_OFFSET 7

//Constants for logisim
#define USE_LOGISIM 1
#define LOGISIM_VER "2.0"
#define OUT_TYPE "raw"
uint32_t binary[(1<<ROM_ADDR_BITS)-1]; //this represents the ROM


/* Generates a flat rom binary image*/
void emit_binary();

/* Generates an address into the rom for the given parameters */
uint32_t emit_addr(size_t opcode, char zero, char interrupt, char next);

/* Generates the binary image */
void generate_binary(macro_state* head, char *outfile);

/* Burns the appropriate signals at the appropriate indices in the ROM */
void generate_micro_state(macro_state* state);
void generate_micro_fetch(macro_state* fetch_step);
void generate_micro_inst(macro_state* inst_step);

/* Burns the signals and grabs the next field from the specified
	locations 
*/
void burn_signals(size_t opcode, size_t step, signal_token signals, 
									size_t reg, size_t is_onz, size_t on_branch);


#endif
