LC2200-32 Tools Documentation

=> NOTE: This set of tools is intended to help you complete phase 2 of
project 1.

=> Introduction

The LC2200-32 is defined in the Project 2 description along with all its
specific implementation details in the provided datapath.  The tools in
this package are provided to assist in development and testing of your
LC2200-32 implementation.  The tools can be compiled using the included
makefile and should work under any UNIX-like system.  There are also
Windows binaries which were compiled using Cygwin.

=> Example Files

A sample FSM input file is included which shows you how to code the first
two states of the FETCH operation.  You don't need to use this file as the
basis of your FSM code, but you may do so if you desire.

=> Loading Data into LogicWorks PROMs and RAMs

Once you have used the included tools to generate the code for your
ROM and/or test programs in the .hex format, you will need to load them into the correct
places in your GT Logisim datapath. To load the file into either part,
first, open up one of the generated HEX file (example.hex for the standard microcontroller), 
hit "ctrl+a" to highlight all the text and "ctrl+v" to copy
it to the clipboard. Then, right click on the correct part in GT Logisim,
select "Edit Contents..." from the menu, select ONLY the first byte in GT
Logisim's HEX editor, and paste the copied HEX code into the ROM/RAM.
Click "Save" to save your changes. The data is now loaded into the PROM/RAM module.

=> FSM ROM Translator

The FSM ROM translator accepts as input a file specifying the various states
of the LC2200-32 state machine control logic along with the transitions between
states and the various control signals which should be asserted in each state.
As output, it provides a translated file containing a ROM/RAM image of the
state machine specified in the input file, suitable for loading into
GT Logisim as a hex input file to a PROM or RAM module.  Note that everything
in the FSM ROM Translator is cAsE sEnSiTiVe.  If you're not familiar with
what a grammar is or how to read one, simply skip down below the formal
grammar definition for a more extensive discussion of how to write compatible
files.  The grammar is present only so that if you know how to read one you
can quickly use it as a summary and avoid reading extra text.  Here is a
description of the grammar to use for your FSM input and some examples:

   FSM        -> STATE FSM | STATE
   STATE      -> STATE_NAME:
                 STATE_DEF
                 goto STATE_NAME |
		 onz STATE_NAME else STATE_NAME |
		 dispatch
   STATE_NAME -> (an element from the states array below)
   STATE_DEF  -> SIGNAL STATE_DEF | SIGNAL
   SIGNAL     -> (an element from the control array below)

   Basically if you don't know what a grammar is or how to read one,
   here is a short description and examples on how to write your FSM.

   Each state has 3 lines, a line specifying the name of the state,
   a line containing the control signals to be asserted in that state,
   and a line specifying what the next state to transition to should be.

   The list of valid state names (you don't need to use them all, just use
   as many as you need) follows:

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
  /* 55 */ "FUTURE", "FUTURE", "FUTURE", "FUTURE", "FUTURE",
           "FUTURE", "FUTURE",
  /* 62 */ "RESERVED",
  /* 63 */ "HALT"

   The control signals which you may assert are listed below:

   DrREG, DrMEM, DrALU, DrPC, DrOFF, LdPC, LdIR, LdMAR, LdA, LdB, LdZ, 
   WrREG, WrMEM, SelPR, RegSelLo, RegSelHi, ALULo, ALUHi, LdIE, IntAck

   The ROM outputs these signals as follows:

	Main ROM Contents
	Bit
	0 lower bit State
	1
	2
	3
	4
	5  higher bit State
	6 DrREG
	7 DrMEM
	8 DrALU
	9 DrPC
	10 DrOFF
	11 LdPC
	12 LdIR
	13 LdMAR
	14 LdA
	15 LdB
	16 LdZ
	17 WrREG
	18 WrMEM
	19 IntEn
	20 RegSelLo
	21 RegSelHi
	22 ALULo
	23 ALUHi
	24 LdIE
	25 IntAck

   The ROM accepts input signals in this order:

	0 Begin next state
	1
	2
	3
	4
	5 End next state
	6 Begin opcode
	7
	8
	9 End opcode
	10 Interrupt detected 
	11 CheckZ
   
   Each state is specified in three lines (may have leading whitespace)...

   	FETCH0:
	   DrPC LdMAR
       	   goto FETCH1

   When you want to go to a next state based upon some other signal input
   besides the current state such as the opcode or the state of the Z
   register use the alternative transitions shown below.

   Here's an example on how to go to BEQ3 if Z is set, BEQ2 otherwise:

   	onz BEQ3 else BEQ2

   Here's an example on how to go to FETCH4 if an interrupt is detected, FETCH5 otherwise:

	onint FETCH4 else FETCH5

   An example of how to choose the next state based upon the opcode input:

        dispatch

   This syntax uses the predefined base states for the various instructions
   to determine the next state.

   Each state follows directly after the previous one with no space between.

To use the translator, enter the following at the console:
fsmrom <input>.fsm

This will produce an output file called <input>.hex which may then be loaded
as a hex input file into LogicWorks.  Because of the way LogicWorks wants its
input, this file will actually be a human-readable transcription of the hex
data in ASCII characters according to the normal written hex format.

As an interesting note, the current optimal solution makes use of 28 states.
This is not to say that there does not exist a more optimal solution than
has yet been proven, but that's a pretty good lower bound.

=> Dealing with HALT

HALT is a special case. It's actually a pseudo-instruction translated into 
an SPOP (special operation), but you can ignore that.  All you need to ensure 
is that when a HALT operation is encountered no further instructions are 
fetched or run.  The single state HALT will be sufficient for this.  Be 
creative!

---
25 Jan. 2005: Initial revision, Kyle Goodwin
30 Jan. 2005: Revised for release, Kyle Goodwin
28 May  2005: Minor Touch-up, Matt Balaun
12 Sep. 2006: Revised for 16 bit, Kane Bonnette
19 May  2008: Cleaned up SPOP/HALT confusion, James Robinson
28 Aug. 2008: Added Windows binaries information and did some cleanup, Matt Bigelow
15 Sep. 2010: Re-written for the MICOCompiler three-ROM state machine. Assembler left untouched, Charlie Shilling
20 Jan. 2011: Split into several files: README-assembler.txt, README-standard.txt, and README-bonus.txt, Charlie Shilling
----
29 Jan. 2012: Trashing fsmromv4, reimpleminting it due to difficulty maintaining it.  Note that fsmromv4.c is not under development.
