#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
	

/* 	Parses the given file for tokens, returning 0 on success or non-zero on failure.  
	file_name = file to be parsed.
	tokens = out parameter of tokens parsed.
	token_count = the number of tokens parsed.
*/
uint8_t parse_file (const char* file_name, size_t* token_count);

/*	Function which will parse out a token, returning 0 on success or non-zero on failure.
	buffer = string to be parsed.
	count = number of characters in token.
	tokens = the list of tokens already parsed
*/
typedef uint8_t (*parse_token) (char* buffer);

/*	Registers a set of token parsing functions which fullfill the contract
	specified by parse_token.
	functions = set of functions to be registered.
	func_count = number of functions to be registered.
*/
void register_token_parsers (parse_token* functions, size_t count);

/*
	Finds the longest matching string from strs.
*/
size_t best_match(char* buffer, const char** strs, size_t count);

#endif
