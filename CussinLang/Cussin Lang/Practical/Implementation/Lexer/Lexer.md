This is the Lexer, containing:
- [[TokenTypes]]
- Token struct
- TokenArray struct
- Tokenizer struct

Methods:
```c++
bool IsWhiteSpace(char c);

bool IsEndOfLine(char c);

bool IsLetter(char c);

void IgnoreCommentsAndWhiteSpace(Tokenizer& tokenizer);

Token GetToken(Tokenizer& Tokenizer);

void DeleteTokenContents(TokenArray token_array);

void DeleteTokens(TokenArray token_array);

void InitializeTokenArray(TokenArray& token_array, unsigned int size);

void ResizeTokenArray(TokenArray& token_array, unsigned int size);

TokenArray LexInput(char* input);

void DebugPrintTokenArray(TokenArray token_array);

```