#ifndef LEXER_H
#define LEXER_H

enum TokenType
{
	TokenType_IDENTIFIER,

	//Datastructs
	TokenType_STRUCT,
	TokenType_ENUM,

	//Modifiers
	TokenType_CONST,
	TokenType_SHORT,
	TokenType_UNSIGNED,

	//Literals
	TokenType_DIGIT,
	TokenType_DT,
	TokenType_FLOAT,
	TokenType_STRING,
	TokenType_BOOL,

	//Operators
	TokenType_PLUS,
	TokenType_MINUS,
	TokenType_TIMES,
	TokenType_SLASH,
	TokenType_PERIOD,
	TokenType_EQL,
	TokenType_NOT,
	TokenType_LSS,
	TokenType_GRT,
	TokenType_LEQ,
	TokenType_GEQ,
	TokenType_MOD,

	//Seperators
	TokenType_LPAREN,
	TokenType_RPAREN,
	TokenType_LBRACK,
	TokenType_RBRACK,
	TokenType_LBRACE,
	TokenType_RBRACE,
	TokenType_SEMICOLON,
	TokenType_COMMA,
	TokenType_BECOMES,
	TokenType_COLON,

	//Function Keywords
	TokenType_FN,
	TokenType_RETURN,
	TokenType_RETSTMT,
	TokenType_EXTERN,

	//Keywords
	TokenType_IF,
	TokenType_THEN,
	TokenType_ELIF,
	TokenType_ELSE,
	TokenType_DO,
	TokenType_WHILE,
	TokenType_FOR,
	TokenType_FIN,
	TokenType_FORE,
	TokenType_FOREB,
	TokenType_TRY,
	TokenType_TRYAROUND,
	TokenType_GRASP,
	TokenType_FINALLY,
	TokenType_GOTO,

	//User defined binary operators
	TokenType_BINARY,
	TokenType_UNARY,
	TokenType_EXCL,

	//Other Keywords
	TokenType_NEW,
	TokenType_LET,
	TokenType_DELETE,
	TokenType_NULL,
	TokenType_SCOPE,
	TokenType_SCOPED,
	TokenType_PERSISTENT,

	//Python like boolean
	TokenType_IN,
	TokenType_IS,
	TokenType_AND,
	TokenType_OR,
	TokenType_XOR,

	//Unique
	TokenType_UNKNOWN = -2,
	TokenType_EOF = -1,
};

struct Token
{
	TokenType type;
	char* contents;
	int length;
};

struct TokenArray
{
	Token* tokens;
	int count;
	int capacity;
};

struct Tokenizer
{
	char* location;
	int count;
};

bool IsWhiteSpace(char c);
bool IsEndOfLine(char c);
bool IsLetter(char c);
void IgnoreCommentsAndWhiteSpace(Tokenizer& tokenizer);
void DeleteTokenContents(TokenArray token_array);
void DeleteTokens(TokenArray token_array);
void InitializeTokenArray(TokenArray& token_array, unsigned int size);
void ResizeTokenArray(TokenArray& token_array, unsigned int size);
void DebugPrintTokenArray(TokenArray token_array);
char const* TokenTypeToString(TokenType type);
Token GetToken(Tokenizer& Tokenizer);
TokenArray LexInput(char* input);

#endif