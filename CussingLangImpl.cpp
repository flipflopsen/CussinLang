// CussingLangImpl.cpp : Defines the entry point for the application.
//

#include "CussingLangImpl.h"
#include "util.h"
#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <cstdio>


using namespace std;

int main()
{
	printf("Starting CussingLangImpl \n");

	bool running = true;

	// Modifiable Values
	int third_int = 9;
	char my_char = 'c';
	char my_str[] = "Test String";
	int another_int = 7;
	int my_int = 5;

	char input[256];
	GetInput(input);

	TokenDictionaryItem token_dict[] =
		{
			{VarToString(my_int), &my_int, const_cast<char*>("int")},
			{VarToString(another_int), &another_int, const_cast<char*>("int")},
			{VarToString(third_int), &third_int, const_cast<char*>("int")},
			{VarToString(*my_char), &my_char, const_cast<char*>("char")},
			{VarToString(my_str), &my_str, const_cast<char*>("string")}};


	if (!input)
		printf("no input string recieved\n");

	TokenArray token_array = LexInput(input);	
	ParseExpression(token_array.tokens, token_array.count, token_dict, ArrayCount(token_dict));
	DebugPrintTokenArray(token_array);
	DeleteTokens(token_array);
	return 0;
}