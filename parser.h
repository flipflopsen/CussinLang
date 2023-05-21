#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

enum DataType
{
	DT_BOOL,
	DT_CHAR,
	DT_SHORT_INT,
	DT_UNSIGNED_SHORT_INT,
	DT_INT,
	DT_UNSIGNED_INT,
	DT_DOUBLE,
	DT_FLOAT,

	/* Pointers */
	DT_POINTER_TO_INT,
	DT_POINTER_TO_DOUBLE,
	DT_POINTER_TO_CHAR,
	DT_POINTER_TO_STRUCT,
	DT_POINTER_TO_FLOAT,
	DT_OTHER
};

//Note: 2 approaches, crate a seperate dictionary for each type, or use a void*
// pointer and cast based on the input. First approach seems safer, second is more flexible,
// but if a user inputs char for a value that is supposed to be int, then problems will occur.

struct TokenDictionaryItem
{
	char const * key;
	void* value;
	char* datatype;
};


void ParseChangeExpression(Token *tokens, int token_count, TokenDictionaryItem *dict, int dict_count);
void ParseListExpression(Token *tokens, int token_count, TokenDictionaryItem *dict, int dict_count);
void ParseExpression(Token *tokens, int token_count, TokenDictionaryItem *dict, int dict_count);

#endif