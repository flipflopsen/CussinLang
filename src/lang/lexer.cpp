#include "lexer.h"

#include "../utils/util.h"
#include "stdio.h"

bool IsWhiteSpace(char c)
{
	bool result = (c == ' ') || (c == '\t') || (c == '\f') || (c == '\v');
	return result;
}

bool IsEndOfLine(char c)
{
	bool result = (c == '\n') || (c == '\r');
	return result;
}

bool IsLetter(char c)
{
	bool result = false;

	if ((c >= 'A') && (c <= 'Z')) result = true;
	if ((c >= 'a') && (c <= 'z')) result = true;

	return result;
}

bool IsNumeric(char c)
{
	bool result = false;

	if ((c >= '0') && (c <= '9')) result = true;

	return result;
}

void IgnoreCommentsAndWhiteSpace(Tokenizer& tokenizer)
{
	if (IsWhiteSpace(tokenizer.location[0])) tokenizer.location++;

	if ((tokenizer.location[0] == '/') && (tokenizer.location[1] == '/'))
	{
		while (!IsEndOfLine(tokenizer.location[0]))
		{
			tokenizer.location++;
		}
		tokenizer.location++;
	}
	else if ((tokenizer.location[0] == '/') && (tokenizer.location[1] == '*'))
	{
		while (((tokenizer.location[0] == '*') && (tokenizer.location[1] == '/')))
		{
			tokenizer.location++;
		}
		tokenizer.location += 2;
	}
}

Token GetToken(Tokenizer& tokenizer)
{
	Token token = {};
	token.length = 0;

	IgnoreCommentsAndWhiteSpace(tokenizer);

	switch (tokenizer.location[0])
	{
	case '\0':
	{
		token.type = TokenType_EOF;
		char* str = new char[10];
		strcopy(str, "EndSymbol");
		token.contents = str;
	} break;
	case ':':
	{
		token.type = TokenType_COLON;
		char* str = new char[4];
		strcopy(str, ":");
		token.contents = str;
		tokenizer.location++;
	} break;
	case '(':
	{
		token.type = TokenType_LPAREN;
		char* str = new char[4];
		strcopy(str, "(");
		token.contents = str;
		tokenizer.location++;
	} break;
	case ')':
	{
		token.type = TokenType_RPAREN;
		char* str = new char[4];
		strcopy(str, ")");
		token.contents = str;
		tokenizer.location++;
	} break;
	case '{':
	{
		token.type = TokenType_LBRACE;
		char* str = new char[4];
		strcopy(str, "{");
		token.contents = str;
		tokenizer.location++;
	} break;
	case '}':
	{
		token.type = TokenType_RBRACE;
		char* str = new char[4];
		strcopy(str, "}");
		token.contents = str;
		tokenizer.location++;
	} break;
	case '[':
	{
		token.type = TokenType_LBRACK;
		char* str = new char[4];
		strcopy(str, "[");
		token.contents = str;
		tokenizer.location++;
	} break;
	case ']':
	{
		token.type = TokenType_RBRACK;
		char* str = new char[4];
		strcopy(str, "]");
		token.contents = str;
		tokenizer.location++;
	} break;

	case '+':
	{
		token.type = TokenType_PLUS;
		char* str = new char[4];
		strcopy(str, "+");
		token.contents = str;
		tokenizer.location++;
	} break;
	case '-':
	{
		tokenizer.location++;
		if (tokenizer.location[0] == '>')
		{
			token.type = TokenType_RETSTMT;
			char* str = new char[4];
			strcopy(str, "->");
			token.contents = str;
			tokenizer.location++;
			break;
		}
		token.type = TokenType_MINUS;
		char* str = new char[4];
		strcopy(str, "-");
		token.contents = str;
		tokenizer.location++;
	} break;
	case '*':
	{
		token.type = TokenType_TIMES;
		char* str = new char[4];
		strcopy(str, "*");
		token.contents = str;
		tokenizer.location++;
	} break;
	case '/':
	{
		token.type = TokenType_SLASH;
		char* str = new char[4];
		strcopy(str, "/");
		token.contents = str;
		tokenizer.location++;
	} break;

	case ';':
	{
		token.type = TokenType_SEMICOLON;
		char* str = new char[4];
		strcopy(str, ";");
		token.contents = str;
		tokenizer.location++;
	} break;
	case ',':
	{
		token.type = TokenType_COMMA;
		char* str = new char[4];
		strcopy(str, ",");
		token.contents = str;
		tokenizer.location++;
	} break;
	case '.':
	{
		token.type = TokenType_PERIOD;
		char* str = new char[4];
		strcopy(str, ".");
		token.contents = str;
		tokenizer.location++;
	} break;

	case '=':
	{
		token.type = TokenType_EQL;
		char* str = new char[4];
		strcopy(str, "=");
		token.contents = str;
		tokenizer.location++;
	} break;
	case '>':
	{
		token.type = TokenType_GRT;
		char* str = new char[4];
		strcopy(str, ">");
		token.contents = str;
		tokenizer.location++;
	} break;
	case '<':
	{
		token.type = TokenType_LSS;
		char* str = new char[4];
		strcopy(str, "<");
		token.contents = str;
		tokenizer.location++;
	} break;
	case '!':
	{
		token.type = TokenType_EXCL;
		char* str = new char[4];
		strcopy(str, "!");
		token.contents = str;
		tokenizer.location++;
	} break;

		//TODO: single quotes are skipped for now
		//NOTE: should everything in the quotes be a single token, 
		// or each quote symbol is a token,
		// and every token inbetween can be interpreted as a quote
	case '"':
	{
		//We need to skip the " and start copying from the next char
		tokenizer.location++;

		token.type = TokenType_STRING;
		char* start_loc = tokenizer.location;

		while (tokenizer.location[0] != '"')
		{
			tokenizer.location++;
			token.length++;

			//Note: how do we properly handle unclosed quotation?
			// if we reached the end of the input then we know for sure something went wrong. 
			if (tokenizer.location[0] == '\0')
			{
				//For now we can just print an error and skip copying our values
				printf("you forgot to close the quotation mark\n");
				token.type = TokenType_UNKNOWN;
				break;
			}
		}

		token.contents = new char[token.length + 1];
		int iterator = 0;
		while (start_loc != tokenizer.location)
		{
			token.contents[iterator] = *start_loc;
			start_loc++;
			iterator++;
		}
		token.contents[token.length] = '\0';

		// We also skip the end " 
		tokenizer.location++;
	}break;

	default:
	{
		if (IsLetter(tokenizer.location[0]))
		{
			// We put a pointer at the beginning, then iterate to the end
			// of the identifier, and then copy everything inbetween into 
			// our token - much more efficient then copying 1 lettr at a time
			char* start_loc = tokenizer.location;
			token.type = TokenType_IDENTIFIER;

			while (IsLetter(tokenizer.location[0]) || IsNumeric(tokenizer.location[0]) || tokenizer.location[0] == '_')
			{
				tokenizer.location++;
				token.length++;
				//TODO: should we limit ourselves?
				if (token.length == 8193) break;
			}
			token.contents = new char[token.length + 1];
			int iterator = 0;
			while (start_loc != tokenizer.location)
			{
				token.contents[iterator] = *start_loc;
				start_loc++;
				iterator++;
			}
			token.contents[token.length] = '\0';

			// Once we have an identifier, we can check if it matches a keyword
			if (strcompare(token.contents, "persistent"))
			{
				token.type = TokenType_PERSISTENT;
				break;
			}
			if (strcompare(token.contents, "scope"))
			{
				token.type = TokenType_SCOPE;
				break;
			}
			if (strcompare(token.contents, "scoped"))
			{
				token.type = TokenType_SCOPED;
				break;
			}
			if (strcompare(token.contents, "binary"))
			{
				token.type = TokenType_BINARY;
				break;
			}
			if (strcompare(token.contents, "unary"))
			{
				token.type = TokenType_UNARY;
				break;
			}
			if (strcompare(token.contents, "if"))
			{
				token.type = TokenType_IF;
				break;
			}
			if (strcompare(token.contents, "then"))
			{
				token.type = TokenType_THEN;
				break;
			}
			if (strcompare(token.contents, "elif"))
			{
				token.type = TokenType_ELIF;
				break;
			}
			if (strcompare(token.contents, "else"))
			{
				token.type = TokenType_ELSE;
				break;
			}
			if (strcompare(token.contents, "extern"))
			{
				token.type = TokenType_EXTERN;
				break;
			}
			if (strcompare(token.contents, "i64"))
			{
				token.type = TokenType_DT;
				break;
			}
			if (strcompare(token.contents, "i32"))
			{
				token.type = TokenType_DT;
				break;
			}
			if (strcompare(token.contents, "i8"))
			{
				token.type = TokenType_DT;
				break;
			}
			if (strcompare(token.contents, "double"))
			{
				token.type = TokenType_DT;
				break;
			}
			if (strcompare(token.contents, "in"))
			{
				token.type = TokenType_IN;
				break;
			}
			if (strcompare(token.contents, "is"))
			{
				token.type = TokenType_IS;
				break;
			}
			if (strcompare(token.contents, "and"))
			{
				token.type = TokenType_AND;
				break;
			}
			if (strcompare(token.contents, "not"))
			{
				token.type = TokenType_NOT;
				break;
			}
			if (strcompare(token.contents, "or"))
			{
				token.type = TokenType_OR;
				break;
			}
			if (strcompare(token.contents, "xor"))
			{
				token.type = TokenType_XOR;
				break;
			}
			if (strcompare(token.contents, "struct"))
			{
				token.type = TokenType_STRUCT;
				break;
			}
			if (strcompare(token.contents, "enum"))
			{
				token.type = TokenType_ENUM;
				break;
			}
			if (strcompare(token.contents, "for"))
			{
				token.type = TokenType_FOR;
				break;
			}
			if (strcompare(token.contents, "fin"))
			{
				token.type = TokenType_FIN;
				break;
			}
			if (strcompare(token.contents, "fore"))
			{
				token.type = TokenType_FORE;
				break;
			}
			if (strcompare(token.contents, "foreb"))
			{
				token.type = TokenType_FOREB;
				break;
			}
			if (strcompare(token.contents, "try"))
			{
				token.type = TokenType_TRY;
				break;
			}
			if (strcompare(token.contents, "tryaround"))
			{
				token.type = TokenType_TRYAROUND;
				break;
			}
			if (strcompare(token.contents, "grasp"))
			{
				token.type = TokenType_GRASP;
				break;
			}
			if (strcompare(token.contents, "finally"))
			{
				token.type = TokenType_FINALLY;
				break;
			}
			if (strcompare(token.contents, "goto"))
			{
				token.type = TokenType_GOTO;
				break;
			}
			if (strcompare(token.contents, "while"))
			{
				token.type = TokenType_WHILE;
				break;
			}
			if (strcompare(token.contents, "fn"))
			{
				token.type = TokenType_FN;
				break;
			}
			if (strcompare(token.contents, "return"))
			{
				token.type = TokenType_RETURN;
				break;
			}
			if (strcompare(token.contents, "do"))
			{
				token.type = TokenType_DO;
				break;
			}
			if (strcompare(token.contents, "new"))
			{
				token.type = TokenType_NEW;
				break;
			}
			if (strcompare(token.contents, "let"))
			{
				token.type = TokenType_LET;
				break;
			}
			if (strcompare(token.contents, "delete"))
			{
				token.type = TokenType_DELETE;
				break;
			}
			if (strcompare(token.contents, "null"))
			{
				token.type = TokenType_NULL;
				break;
			}
			//TODO: before we avoided copies, now we copy early
			// We save the copy to the end to avoid copying for keywords
			//strcopy(token.contents, buffer);

			// For boolean values we can change our token type
			if (strcompare(token.contents, "true") ||
				strcompare(token.contents, "false"))
			{
				token.type = TokenType_BOOL;
			}
		}
		else if (IsNumeric(tokenizer.location[0]))
		{
			//TODO: should we eat all leading 0's in a numerical value 
			// other than the first 0 for a float?
			char* start_loc = tokenizer.location;
			token.type = TokenType_DIGIT;

			while (IsNumeric(tokenizer.location[0]))
			{
				tokenizer.location++;
				token.length++;

				//Look forward 1 charachter for the . operator for floats
				if (tokenizer.location[0] == '.')
				{
					token.type = TokenType_FLOAT;
					tokenizer.location++;
					token.length++;
				}
				//TODO: we should probably check for multiple '.', because that's not a real float
			}
			token.contents = new char[token.length + 1];
			int iterator = 0;
			while (start_loc != tokenizer.location)
			{
				token.contents[iterator] = *start_loc;
				start_loc++;
				iterator++;
			}
			token.contents[token.length] = '\0';
		}
	} break;
	}

	return token;

}

void DeleteTokenContents(TokenArray token_array)
{
	for (int i = 0; i < token_array.count; i++)
	{
		delete[] token_array.tokens[i].contents;
	}
}

void DeleteTokens(TokenArray token_array)
{
	DeleteTokenContents(token_array);
	delete[] token_array.tokens;
	token_array = {};
}

void InitializeTokenArray(TokenArray& token_array, unsigned int size)
{
	token_array.tokens = new Token[size];
	token_array.capacity = size;
}
void ResizeTokenArray(TokenArray& token_array, unsigned int size)
{
	DeleteTokens(token_array);
	token_array.tokens = new Token[size];
	token_array.capacity = size;
}

TokenArray LexInput(char* input)
{
	Tokenizer tokenizer = {};
	tokenizer.location = input;
	bool lexing = true;

	TokenArray token_array = {};
	InitializeTokenArray(token_array, 128);

	while (lexing)
	{
		token_array.tokens[tokenizer.count] = GetToken(tokenizer);
		tokenizer.count++;

		if (tokenizer.count == token_array.capacity)
		{
			ResizeTokenArray(token_array, token_array.capacity * 2);
		}

		if (token_array.tokens[tokenizer.count - 1].type == TokenType_EOF)
		{
			lexing = false;
		}
	}

	token_array.count = tokenizer.count;
	return token_array;
}

char const* TokenTypeToString(TokenType type)
{
	switch (type)
	{
		case TokenType_IDENTIFIER:
			return "Identifier";
		case TokenType_ENUM:
			return "Datastruct";
		case TokenType_CONST:
		case TokenType_SHORT:
		case TokenType_UNSIGNED:
			return "Modifier";
		case TokenType_DIGIT:
		case TokenType_DT:
		case TokenType_FLOAT:
		case TokenType_STRING:
		case TokenType_BOOL:
			return "Literal";
		case TokenType_PLUS:
		case TokenType_MINUS:
		case TokenType_TIMES:
		case TokenType_SLASH:
		case TokenType_PERIOD:
		case TokenType_EQL:
		case TokenType_NOT:
		case TokenType_LSS:
		case TokenType_GRT:
		case TokenType_LEQ:
		case TokenType_GEQ:
		case TokenType_IN:
		case TokenType_AND:
		case TokenType_OR:
		case TokenType_XOR:
		case TokenType_IS:
			return "Operator";
		case TokenType_LPAREN:
		case TokenType_RPAREN:
		case TokenType_LBRACK:
		case TokenType_RBRACK:
		case TokenType_LBRACE:
		case TokenType_RBRACE:
		case TokenType_SEMICOLON:
		case TokenType_COMMA:
		case TokenType_BECOMES:
		case TokenType_COLON:
			return "Separator";
		case TokenType_FN:
		case TokenType_IF:
		case TokenType_THEN:
		case TokenType_ELIF:
		case TokenType_ELSE:
		case TokenType_DO:
		case TokenType_WHILE:
		case TokenType_FOR:
		case TokenType_FIN:
		case TokenType_FORE:
		case TokenType_FOREB:
		case TokenType_TRY:
		case TokenType_TRYAROUND:
		case TokenType_GRASP:
		case TokenType_FINALLY:
		case TokenType_GOTO:
		case TokenType_RETURN:
		case TokenType_NEW:
		case TokenType_LET:
		case TokenType_DELETE:
		case TokenType_NULL:
		case TokenType_EXTERN:
		case TokenType_SCOPE:
		case TokenType_SCOPED:
		case TokenType_PERSISTENT:
		case TokenType_STRUCT:
			return "Keyword";
		case TokenType_BINARY:
		case TokenType_UNARY:
			return "Keyword for UserDefinedOperators";
		case TokenType_RETSTMT:
			return "Return specification strongly";
		case TokenType_EOF:
			return "EndOfStream";
		case TokenType_UNKNOWN:
		default:
			return "unknown";
	}
}

void DebugPrintTokenArray(TokenArray token_array)
{
	for (int i = 0; i < token_array.count; i++)
	{
		printf("[LEXER] Token %i: %s - %s \n", i, TokenTypeToString(token_array.tokens[i].type),
			token_array.tokens[i].contents);
	}
}