```c++
enum TokenType
{
	TokenType_IDENTIFIER,

	//Datastructs

	TokenType_STRUCT,
	TokenType_UN,
	TokenType_ENUM,
	TokenTyper_UNENUM,

	//Modifiers
	TokenType_CONST,
	TokenType_SHORT,
	TokenType_UNSIGNED,

	//Literals
	TokenType_INT,
	TokenType_DOUBLE,
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


	//Keywords
	TokenType_IF,
	TokenType_ELIF,
	TokenType_ELSE,
	TokenType_DO,
	TokenType_WHILE,
	TokenType_FOR,
	TokenType_FORE,
	TokenType_FOREB,
	TokenType_TRY,
	TokenType_TRYAROUND,
	TokenType_GRASP,
	TokenType_FINALLY,
	TokenType_GOTO,
	TokenType_RETURN,

	//Other Keywords
	TokenType_NEW,
	TokenType_LET,
	TokenType_DELETE,
	TokenType_NULL,

	//Unique
	TokenType_UNKNOWN,
	TokenType_EOF
};
```