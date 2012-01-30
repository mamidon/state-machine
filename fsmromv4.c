/*
 * CS2200 Fall 2008
 * Project 1, LC2200-16
 * FSM ROM Translator
 *
 * 13 Jan. 2005, Initial Revision, Kyle Goodwin
 * 21 Jan. 2005, Completed without CC, Kyle Goodwin
 * 24 Jan. 2005, Updated to include CC in dispatch, Kyle Goodwin
 * 30 Jan. 2005, Fixed some bugs with CC, Kyle Goodwin
 * 7  Jun. 2006, Updated error messages, Kane Bonnette
 * 30 Aug. 2006, Changed to 16-bit (rem LdIRLo/Hi), Kane Bonnette
 * 28 Aug. 2008, Added support for DOS and Macintosh files, Matt Bigelow
 * 3 Feb. 2011, Modified to support 32 bit and interrupts
 */

/* 
Translator grammar:
   FSM        -> STATE FSM | STATE
   STATE      -> STATE_NAME:
                 STATE_DEF
                 goto STATE_NAME |
		 onz STATE_NAME else STATE_NAME |
		 dispatch
   STATE_NAME -> (an element from the states array below)
   STATE_DEF  -> SIGNAL STATE_DEF | SIGNAL
   SIGNAL     -> (an element from the control array below)
   
   Each state is specified in three lines (may have leading whitespace)...
   FETCH0:
       DrPC LdMAR
       goto FETCH1

   When you want to go to a next state based upon some other signal input
   besides the current state such as the opcode or the state of the Z
   register use the alternative transitions shown below:

   Go to BEQ3 if Z is set, BEQ2 otherwise:
   onz BEQ3 else BEQ2

   Choose the next state based upon the opcode input:
   dispatch

   For the special case of SPOP, the CC signal is also used to determine
   the proper state for a dispatch.

   This syntax uses the predefined base states for the various instructions
   to determine the next state.

   Each state follows directly after the previous one with no space between.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
/* Uncomment the following line to enable verbose debugging output: */
/* #define DEBUG_VERBOSE */

/* The actual rom, 12 (word) address bits and 32 bits per word */
static unsigned int rom[(1 << 13) - 1];

/* Defines for the various input signal states */
#define Z_SET   (1 << 12)
#define CCL_SET (1 << 10)
#define CCH_SET (1 << 11)

/* Defines for discerning possible next states
	See parsetransition */
#define ZI_TRANS ( 48)
#define I_TRANS (32)
#define Z_TRANS (16)
#define D_TRANS (0)

/* Table of signals */

/* STATES (room for 7 bits, 128 possible states) */
const char *states[105] = {
  /*  0 */ "DUMMY",
  /*  1 */ "FETCH0", "FETCH1", "FETCH2", "FETCH3", "FETCH4",
           "FETCH5", "FETCH6", "FETCH7", "FETCH8", "FETCH9",
  /* 11 */ "DECODE",
  /* 12 */ "ADD0", "ADD1", "ADD2", "ADD3",
  /* 16 */ "ADDI0", "ADDI1", "ADDI2", "ADDI3",
  /* 20 */ "NAND0", "NAND1", "NAND2", "NAND3",
  /* 24 */ "LW0", "LW1", "LW2", "LW3",
  /* 28 */ "SW0", "SW1", "SW2", "SW3",
  /* 32 */ "BEQ0", "BEQ1", "BEQ2", "BEQ3", "BEQ4",
           "BEQ5", "BEQ6", "BEQ7", "BEQ8",
  /* 41 */ "JALR0", "JALR1", "JALR2", "JALR3", "JALR4",
  /* 46 */ "EI0", "EI1", "EI2", "EI3", "EI4",
  /* 51 */ "DI0", "DI1", "DI2", "DI3", "DI4",
  /* 56 */ "RETI0", "RETI1", "RETI2", "RETI3", "RETI4", "RETI5",
  /* 62 */ "RESERVED",
  /* 63 */ "HALT",
  /* 64 */ "BONI0", "BONI1", "BONI2", "BONI3", "BONI4", 
  	   "BONI5", "BONI6", "BONI7", "BONI8", "BONI9", 
  /* 74 */ "BONJ0", "BONJ1", "BONJ2", "BONJ3", "BONJ4", 
  	   "BONJ5", "BONJ6", "BONJ7", "BONJ8", "BONJ9", 
  /* 84 */ "BONR0", "BONR1", "BONR2", "BONR3", "BONR4", 
  	   "BONR5", "BONR6", "BONR7", "BONR8", "BONR9", 
  /* 94 */ "BONO0", "BONO1", "BONO2", "BONO3", "BONO4", 
  	   "BONO5", "BONO6", "BONO7", "BONO8", "BONO9", 
  /* 104 */ NULL
};

#define DISPATCH_STATE 62

/* INITIAL STATES BY INSTRUCTION */
const unsigned int dispatchstate[16] = {
  /* opcode     */
  /*   000 ADD  */ 12,
  /*   001 NAND */ 20,
  /*   010 ADDI */ 16,
  /*   011 LW   */ 24,
  /*   100 SW   */ 28,
  /*   101 BEQ  */ 32,
  /*   110 JALR */ 41,
  /*  0111 HALT */ 63,
  /*  1000 BONR */ 84,
  /*  1001 BONO */ 94,
  /*  1010 EI   */ 46,
  /*  1011 DI   */ 51,
  /*  1100 RETI */ 56,
  /*  1101 BONI */ 64,
  /*  1110 BONJ */ 74,
  /*  1111 NA */ 0,
};

/* INITIAL STATES FOR SPOP BY CC VALUE */
/*const unsigned int spopstate[4] = { */
        /* CC */
//        /* 00 */ 38,
 //       /* 01 */ 46,
    //    /* 10 */ 51,
     //   /* 11 */ 56
//};
        

#define OP_OFFSET 7
#define CC_OFFSET 10
#define CONTROL_BASE 7

/* CONTROL (start at CONTROL_BASE bits from 0, 1 bit per signal) */
const char *control[21] = {
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
  NULL
};

/* Holds the line number of the line currently being parsed */
static unsigned int lineno = 0;

/* Sets the bit associated with the specified control signal */
void addcontrol(char *sig, unsigned int *output) {
        unsigned int i = 0;
        while (control[i] != NULL) {
                if (!strcmp(sig, control[i])) {
                        *output += 1 << (i + CONTROL_BASE);
                        return;
                }

                i++;
        }

        fprintf(stderr, "Line %d: Invalid control signal '%s'\n",
                lineno, sig);
        exit(1);
}

/* Parses a line (supposedly) containing a state header and returns the
   numerical value of the state */
unsigned int parsestate(char *line) {
        unsigned int i = 0;
        char *tmp;

        while (*line <= 40) line++;
        
        tmp = strchr(line, ':');
        
        if (tmp == NULL) {
                fprintf(stderr,
                        "Line %d: Malformed state header '%s' missing ':'\n",
                        lineno, line);
                exit(1);
        }

        *tmp = '\0';
        
        while (states[i] != NULL) {
                if (!strcmp(line, states[i]))
                        return i;
                i++;
        }

        fprintf(stderr, "Line %d: Invalid state name '%s'\n", lineno, line);
        exit(1);
}

/* Parses a line (supposedly) containing a state definition with multiple
   signals into several discrete signal strings which may then be parsed
   into their numerical equivilents to set the bits of the complete
   control signal specified by the state */
unsigned int parsesignals(char *line) {
        unsigned int signal = 0;
        char *cur;
	while (*line <= 40) line++;

        while (*(cur = strsep(&line, " ")) != '\0') {
	  addcontrol(cur, &signal);

#ifdef DEBUG_VERBOSE
	  printf("Signal: %X\n", signal);
#endif

	  if (line == NULL) break;
	}

        return signal;
}

/* Parses a line (supposedly) containing a state transition, optionally
   referencing the value of the z input or causing a dispatch to occur */
/* In the spirit of shitty coding practices, I will be packing several return
   values into the UINT64_T transition */
/* Formatted by 16b shorts as follows:
   | Trans for Z&I | Trans for I | Trans for Z | Default | */
uint64_t parsetransition(char *line) {

  char *cur;
  uint64_t transition = 0xdeadbeef;
  uint64_t i = 0;

  /* increment line till it's at 40.. is it relative to some offset? */
  while (*line <= 40) line++;

  if (!strncmp(line, "goto", 4)) {
    strsep(&line, " ");
    
    while (states[i] != NULL) {
      if (!strcmp(line, states[i])) {
	    /* the upper half of the transition specifies the next state
	       if z is set, the lower half if z is unset; these are both
	       the same in the case of a plain goto state tranition */

	    transition = (i << ZI_TRANS) + (i << I_TRANS) + (i << Z_TRANS) + (i << D_TRANS);
	    return transition;
      }

      i++;
    }

    fprintf(stderr, "Line %d: Invalid state name in transition '%s'",
	    lineno, line);

    exit(1);
  }

  /* is there an 'onz' statement here? */
  if (!strncmp(line, "onz", 3)) {
    strsep(&line, " ");
    cur = strsep(&line, " ");

    /* yes, but there is no next state specified */
    if (*cur == '\0') {
      fprintf(stderr, "Line %d: Invalid onz transition statement", lineno);

      exit(1);
    }

    /* dig through the states; burn that state into transition */
    while (states[i] != NULL) {
      if (!strcmp(cur, states[i])) {
	transition = (i << Z_TRANS) + (i << ZI_TRANS);
	break;
      }

      i++;
    }

    /* there was a name, but it was invalid */
    if (transition == 0xdeadbeef) {
      fprintf(stderr, "Line %d: Invalid state name in transition '%s'",
	      lineno, cur);

      exit(1);
    }

    cur = strsep(&line, " ");
    
    /* had better have an 'else' clause.. */
    if (strcmp(cur, "else")) {
	fprintf(stderr, "Line %d: Invalid onz transition statement, missing else",
		lineno);

	exit(1);
    }

    i = 0;

    /* stick that other state in the lower half of transition */
    while (states[i] != NULL) {
      if (!strcmp(line, states[i])) {
	transition += (i << D_TRANS) + (i << I_TRANS);
	return transition;
      }

      i++;
    }
    
    fprintf(stderr, "Line %d: Invalid state name in transition '%s'",
	    lineno, line);

    exit(1);
  }

/* is there an 'ONINT' statement here? */
  if (!strncmp(line, "onint", 3)) {
    strsep(&line, " ");
    cur = strsep(&line, " ");

    /* yes, but there is no next state specified */
    if (*cur == '\0') {
      fprintf(stderr, "Line %d: Invalid onint transition statement", lineno);

      exit(1);
    }
    
    /* dig through the states; burn that state into transition */
    while (states[i] != NULL) {
      if (!strcmp(cur, states[i])) {
	transition = (i << I_TRANS) + (i << ZI_TRANS);
	break;
      }

      i++;
    }
	
    /* there was a name, but it was invalid */
    if (transition == 0xdeadbeef) {
      fprintf(stderr, "Line %d: Invalid state name in transition '%s'",
	      lineno, cur);

      exit(1);
    }

    cur = strsep(&line, " ");

    /* had better have an 'else' clause.. */
    if (strcmp(cur, "else")) {
	fprintf(stderr, "Line %d: Invalid onint transition statement, missing else",
		lineno);

	exit(1);
    }

    i = 0;

    /* stick that other state in the lower half of transition */
    while (states[i] != NULL) {
      if (!strcmp(line, states[i])) {
	transition += (i << D_TRANS) + (i << Z_TRANS);
	return transition;
      }

      i++;
    } 
    
    fprintf(stderr, "Line %d: Invalid state name in transition '%s'",
	    lineno, line);

    exit(1);
  }



  if (!strcmp(line, "dispatch")) {
    /* A special reserved state number is used for the dispatch
       transition, this state number is replaced by the corrent
       state number for the opcode specified at rom creation time */
    transition = DISPATCH_STATE;
    return transition;
  }
  
  fprintf(stderr, "Line %d: Expected state transition\n", lineno);
  
  exit(1);
}

/* Handles the parsing and rom update for one complete state
   for all values of opcode, z, and cc inputs */
unsigned int handlestate(FILE *fp) {
        unsigned int i, state, signals;
	uint64_t transition;
  char t;
  char buf[256];

  lineno++;

  t = fgetc(fp);
  if((t == '\n') || (t == '\r'))
    while ((t == '\n') || (t == '\r'))
      t = fgetc(fp);

  fseek(fp, -1, SEEK_CUR);

  for (i = 0; i < 255; i++) {
    t = fgetc(fp);

    if ((t == '\n') || (t == '\r') || (t == EOF)) {
      if (i == 0) return 0;

      buf[i] = '\0';
      break;
    } else buf[i] = t;
  }

  if (i == 255) {
    fprintf(stderr, "Line %d: Line exceeded maximum length", lineno);
    
    exit(1);
  }

  state = parsestate(buf);

  lineno++;

  while ((t == '\n') || (t == '\r'))
    t = fgetc(fp);

  for (i = 0; i < 255; i++) {
    t = fgetc(fp);

    if ((t == '\n') || (t == '\r') || (t == EOF)) {
      buf[i] = '\0';
      break;
    } else buf[i] = t;
  }

  if (i == 255) {
    fprintf(stderr, "Line %d: Line exceeded maximum length", lineno);
    
    exit(1);
  }

  signals = parsesignals(buf);

  lineno++;

  while ((t == '\n') || (t == '\r'))
    t = fgetc(fp);

  for (i = 0; i < 255; i++) {
    t = fgetc(fp);

    if ((t == '\n') || (t == '\r') || (t == EOF)) {
      buf[i] = '\0';
      break;
    } else buf[i] = t;
  }

  if (i == 255) {
    fprintf(stderr, "Line %d: Line exceeded maximum length", lineno);
    
    exit(1);
  }

  transition = parsetransition(buf);

  if (transition != DISPATCH_STATE) {
    for (i = 0; i < 13; i++) {	
                    rom[state + (i << OP_OFFSET)] =
                            (uint32_t)(transition & ((uint64_t)0xffff<<D_TRANS)) + signals;
	
                    rom[state + (i << OP_OFFSET) + Z_SET] =                       
              			(uint32_t)((transition & ((uint64_t)0xffff<<Z_TRANS)) >> Z_TRANS) + signals;

		    rom[state + (i << OP_OFFSET) + CCH_SET] =                       
              			(uint32_t)((transition & ((uint64_t)0xffff<<I_TRANS)) >> I_TRANS) + signals;
		    
		    rom[state + (i << OP_OFFSET) + CCH_SET + Z_SET] =                       
              			(uint32_t)((transition & ((uint64_t)0xffff<<ZI_TRANS)) >> ZI_TRANS) + signals;
		    	
    }
  } else {
    for (i = 0; i < 13; i++) {
	rom[state + (i << OP_OFFSET)] = dispatchstate[i] + signals;
		
                    rom[state + (i << OP_OFFSET) + Z_SET] = dispatchstate[i] + signals;
			
		rom[state + (i << OP_OFFSET) + CCH_SET] = dispatchstate[i] + signals;	

		rom[state + (i << OP_OFFSET) + Z_SET + CCH_SET] = dispatchstate[i] + signals;	
            
   
 }}

  return 1;
}

/* The main program reads in command line parameters, opens the
   source file for reading, invokes the main loop calling subroutines
   as needed and then writes out the completed file */
int main(int argc, char **argv) {
  unsigned int i;
  FILE *fp;

  /* Zero out the ROM initially such that if a state is not specified
     it is easy to notice (even halt states have a nonzero value) */
  for (i = 0; i < (1 << 12); i++)
    rom[i] = 0;

  if (argc != 2) {
    fprintf(stderr,
	    "Usage: fsmrom file.fsm\n\tOutputs a compiled ROM to file.hex\n");

    exit(1);
  }

  fp = fopen(argv[1], "r");

  if (fp == NULL) {
    fprintf(stderr, "File %s not found.\n", argv[1]);

    exit(1);
  }
  
  while (handlestate(fp));

  for (i = 0; i < (1 << 11); i++) {
    /* Hardwire in the zero DUMMY state which is the initial state of the
       state machine to assert no control signals and transition to
       state 1 regardless of other (z and cc) inputs */
    if ((i & 0x003f) == 0x00) {
      	rom[i] = 0x00000001;
	rom[i+Z_SET] = 0x00000001;
	rom[i+CCH_SET] = 0x00000001;
	rom[i+Z_SET+CCH_SET] = 0x00000001;   
	}
    /* deprecated: HALT is no longer hard-coded. */
    /* Hardwire in the HALT state which is the halting state of the state
       machine to assert no control signals and transition back to itself
       regardless of other (z and cc) inputs */
    /* if ((i & 0x003f) == 0x3f)
       rom[i] = 0x0000003f; */
  }

#ifdef DEBUG_VERBOSE
  for (i = 0; i < (1 << 13); i++)
    printf("0x%04X: %08X\n", i, rom[i]);
#endif

  fclose(fp);

  fp = fopen(strcat(strsep(&argv[1], "."), ".hex"), "w");

  for (i = 0; i < (1 << 13); i++) {
          /* LogicWorks wants a less "raw" format than above */
          printf("%08X ", rom[i]);
  }

  fclose(fp);

  return 0;
}

