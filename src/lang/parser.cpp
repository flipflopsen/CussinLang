#include <map>

#include "parser.h"
#include "../utils/util.h"
#include "../utils/logger.h"

std::map<std::string, DataType> KnownVars;


void Parser::Parse(bool jit)
{
	if (jit)
	{
		while (Position < Count) {
			getNextToken();
			switch (CurTok) {
			case TokenType_EOF:
				return;
			case TokenType_SEMICOLON:
				printf("Got semicolon !\n");
				getNextToken();
				break;
			case TokenType_FN:
				HandleDefinitionJIT();
				break;
			case TokenType_EXTERN:
				HandleExternJIT();
				break;
			default:
				HandleTopLevelExpressionJIT();
				break;
			}
		}
	}
	else
	{
		while (Position < Count && CurTok != TokenType_EOF) {
			getNextToken();
			fprintf(stderr, "[PARSER-ENTRY] Current token is %d\n", CurTok);
			switch (CurTok) {
			case TokenType_SEMICOLON:
				printf("Got semicolon !\n");
				getNextToken();
			case TokenType_FN:
				HandleDefinition();
				break;
			case TokenType_EXTERN:
				HandleExtern();
				break;
			case TokenType_PERSISTENT:
			case TokenType_SCOPE:
				HandleScopeExpression();
				break;
			default:
				HandleTopLevelExpression();
				break;
			}
		}
	}
}


std::unique_ptr<ExprAST> Parser::ParseExpression() {
	auto LHS = ParseUnary();
	if (!LHS)
		return nullptr;

	return ParseBinOpRHS(0, std::move(LHS));

}


std::unique_ptr<ExprAST> Parser::ParsePrimary()
{
	if (CurTok == TokenType_IDENTIFIER && PeekNextToken().type == TokenType_LPAREN)
		return ParseCallExpr();
	
	switch (CurTok) {
	case TokenType_SEMICOLON:
		getNextToken(); // Consume the semicolon
		printf("[PARSER] Parsing of Expression is done!\n");
		return ParseExpression();
	case TokenType_STRUCT:
		printf("[PARSER] Parsing Struct Expr!\n");
		return ParseStructExpr();
	case TokenType_PERSISTENT:
	case TokenType_SCOPE:
		printf("[PARSER] Parsing Scope Expression\n");
		return ParseScopeExpr();
	case TokenType_IDENTIFIER:
		printf("[PARSER] Parsing Identifier Expression\n");
		return ParseIdentifierExpr();
	case TokenType_DIGIT:
		printf("[PARSER] Parsing number Expression\n");
		return ParseNumberExpr();
	case TokenType_RBRACE:
		printf("[PARSER] Parsing of Body is done!\n");
		return nullptr;
	case TokenType_LPAREN:
		printf("[PARSER] Parsing Paren Expression\n");
		return ParseParenExpr();
	case TokenType_IF:
		printf("[PARSER] Parsing IF Expression\n");
		return ParseIfExpr();
	case TokenType_FOR:
		printf("[PARSER] Parsing FOR Expression\n");
		return ParseForExpr();
	case TokenType_LET:
		printf("[PARSER] Parsing LET Expression\n");
		return ParseLetExpr();
	case TokenType_RETURN:
		printf("[PARSER] Parsing return Expression\n");
		return ParseReturnExpr();
	case TokenType_EOF:
		printf("[PARSER] Parsing of File is done!\n");
		return nullptr;
	case TokenType_COMMA:
		printf("[PARSER] Got comma in ParsePrimary!\n");
		return nullptr;
	default:
		printf("[PARSER] Token type of current token: %d\n", CurTok);
		return LogError("Unknown token when expecting an expression");
	};
}

// Parsers

std::unique_ptr<ExprAST> Parser::ParseNumberExpr()
{
	auto const token = PeekCurrentToken().contents;
	fprintf(stderr, "[PARSER-NR] Parsing number expression for token: %s\n", token);
	//Todo: figure out datatype of number, maybe with casting or sth, rn it defaults to i32
	auto result = std::make_unique<NumberExprAST>(strtoint(token));
	return std::move(result);
}

std::unique_ptr<ExprAST> Parser::ParseIdentifierExpr()
{
	std::string id_name = PeekCurrentToken().contents;

	auto const lookahead = PeekNextToken();

	if (lookahead.type != TokenType_LPAREN)
	{
		if (KnownVars.count(id_name) > 0)
			return std::make_unique<VariableExprAST>(id_name, KnownVars[id_name]);
		if (lookahead.type == TokenType_COLON)
		{
			//LogError("Expected ':' after identifier!");
			fprintf(stderr, "[PIDENT] Lookahead type is: %d\n", lookahead.type);
			auto dt = EvaluateDataTypeOfToken(2);
			KnownVars[id_name] = dt;

			return std::make_unique<VariableExprAST>(id_name, dt);
		}
		//return std::make_unique<VariableExprAST>(id_name, EvaluateDataTypeOfToken(2));
	}

	getNextToken();

	// This is vor single vars
	if ((CurTok != TokenType_SEMICOLON && 
		(IsOperator(lookahead.type) || lookahead.type == TokenType_SEMICOLON) &&
		lookahead.type != TokenType_LPAREN) || lookahead.type == TokenType_RPAREN)
	{
		fprintf(stderr, "[PIDENT] Lookahead type is: %d\n", lookahead.type);
		return std::make_unique<VariableExprAST>(id_name, EvaluateDataTypeOfToken(1));
	}
	
	// If we're here, it's a function call
	fprintf(stderr, "[IMPORTANT] Calling ParseCallExpr from ParseIdentExpr!!\n");
	return ParseCallArgsExpr();
}

std::unique_ptr<ExprAST> Parser::ParseCallExpr()
{
	std::string IdName = PeekCurrentToken().contents;

	getNextToken();

	std::vector<std::unique_ptr<ExprAST>> Args;

	if (CurTok != TokenType_RPAREN)
	{
		while (true)
		{
			if (CurTok != TokenType_COMMA)
			{
				if (auto Arg = ParseCallArgsExpr())
				{
					printf("[PARSER-IDENT] Parsed token with val: %d\n", CurTok);
					Args.push_back(std::move(Arg));
				}
				else
				{
					if (CurTok == TokenType_RPAREN)
					{
						printf("[PARSER-IDENT] Encountered ')' \n");
						break;
					}

					if (CurTok != TokenType_LPAREN)
					{
						printf("[PARSER-IDENT-ERROR] Failed to parse call for tok %d\n", CurTok);
						return nullptr;
					}
					printf("[PARSER-IDENT-ERROR] Encountered ( in CallParsing\n");

				}
			}
			getNextToken();
		}
	}

	return std::make_unique<CallExprAST>(IdName, std::move(Args));
}

std::unique_ptr<ExprAST> Parser::ParseCallArgsExpr()
{
	if (CurTok == TokenType_LPAREN)
		getNextToken();

	std::string id_name = PeekCurrentToken().contents;

	auto const lookahead = PeekNextToken();

	if (lookahead.type == TokenType_COMMA || lookahead.type == TokenType_RPAREN)
	{
		if (CurTok == TokenType_IDENTIFIER)
		{
			if (KnownVars.count(id_name) > 0)
				return std::make_unique<VariableExprAST>(id_name, KnownVars[id_name]);
			getNextToken(); // eat colon
			getNextToken();
			fprintf(stderr, "[ParseCallArgsExpr] Lookahead type is: %d\n", lookahead.type);
			//TODO: remeber initialized vars and their datatype
			return std::make_unique<VariableExprAST>(id_name, EvaluateDataTypeOfToken(0));
		}
		return ParseNumberExpr();
	}

	if (IsOperator(lookahead.type))
	{
		return ParseExpression();
	}

	LogErrorP("Returning nullptr in CallArgsExpr, lul?\n");

	return nullptr;
}

std::unique_ptr<ExprAST> Parser::ParseParenExpr()
{
	getNextToken();  // Consume '('

	auto expr = ParseExpression();
	if (!expr)
		return nullptr;

	getNextToken();

	if (CurTok != TokenType_RPAREN)
		return LogError("Expected ')'");


	//getNextToken();  // Consume ')'
	return expr;
}

std::unique_ptr<ExprAST> Parser::ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS)
{
	if (CurTok == TokenType_RBRACE || CurTok == TokenType_SEMICOLON || PeekNextToken().type == TokenType_FN)
		return LHS;

	if (ExprPrec < 1)
		getNextToken();

	while (true) {
		int TokPrec = GetTokenPrecedence();

		if (TokPrec < ExprPrec)
			return LHS;

		int BinOp = static_cast<int>(*PeekCurrentToken().contents);
		getNextToken(); 

		auto RHS = ParseUnary();
		if (!RHS)
			return nullptr;

		if (IsOperator(PeekNextToken().type))
		{

			getNextToken();
			int NextPrec = GetTokenPrecedence();
			if (TokPrec <= NextPrec) {
				RHS = ParseBinOpRHS(TokPrec, std::move(RHS));
				if (!RHS)
					return nullptr;
			}
			else
			{
				RHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
				RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
			}
		}

		// Merge LHS/RHS.
		LHS =
			std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));

		return LHS;
	}
}


std::unique_ptr<PrototypeAST> Parser::ParsePrototype(bool is_extern)
{
	printf("[PARSER] Parsing prototype\n");

	std::string FnName;
	unsigned Kind = 0; // 0 = identifier, 1 = unary, 2 = binary
	unsigned BinaryPrecedence = 30;

	switch (CurTok) {
	default:
		return LogErrorP("Expected function name in prototype");
	case TokenType_IDENTIFIER:
		FnName = PeekCurrentToken().contents;
		Kind = 0;
		getNextToken();
		break;
	case TokenType_UNARY:
		getNextToken();
		if (!isascii(CurTok))
			return LogErrorP("Expected unary operator");
		FnName = "unary";
		FnName += (char)CurTok;
		Kind = 1;
		getNextToken();
		break;
	case TokenType_BINARY:
		getNextToken();
		if (!isascii(CurTok))
			return LogErrorP("Expected binary operator");
		FnName = "binary";
		FnName += (char)CurTok;
		Kind = 2;
		getNextToken();

		// Read the precedence if present.
		if (CurTok == TokenType_DIGIT) {
			auto NumVal = strtoint(PeekCurrentToken().contents);
			if (NumVal < 1 || NumVal > 100)
				return LogErrorP("Invalid precedence: must be 1..100");
			BinaryPrecedence = (unsigned)NumVal;
			getNextToken();
		}
		break;
	case TokenType_EOF:
		return nullptr;
	}


	printf("[PARSER] FnName for prototype: %s\n", FnName.c_str());
	DataType ReturnType = DT_VOID;

	//getNextToken();

	if (CurTok != TokenType_LPAREN)
		return LogErrorP("Expected '(' in prototype");

	std::vector< std::pair<std::string, DataType>> ArgNames;
	Token nextToken = getNextToken();
	auto lookahead = PeekNextToken();
	while (nextToken.type != TokenType_RPAREN)
	{
		if (nextToken.type != TokenType_COMMA && nextToken.type != TokenType_DT) 
		{
			DataType dt;
			if (lookahead.type == TokenType_COLON)
			{
				getNextToken(); // eat up colon
				fprintf(stderr, "[ParsePrototype] Lookahead type is: %d, nextToken: %d\n", lookahead.type, nextToken.type);
				dt = EvaluateDataTypeOfToken(1);
				ArgNames.push_back(std::make_pair(nextToken.contents, dt));
				KnownVars[nextToken.contents] = dt;
			}
		}
		nextToken = getNextToken();
	}

	if (CurTok != TokenType_RPAREN)
		return LogErrorP("Expected ')' in prototype");

	// success.
	getNextToken(); // eat ')'.

	if (!is_extern && Kind == 0)
	{
		if (CurTok != TokenType_RETSTMT)
		{
			//return LogErrorP("Expected '->' in prototype");
			ReturnType = DT_VOID;
		}
		else
		{
			Token tok = getNextToken(); // eat '->'

			if (CurTok != TokenType_DT)
				return LogErrorP("Expected valid return type (i8/i32/i64/double) in prototype");
			ReturnType = EvaluateDataTypeOfToken(0);

			getNextToken(); // eat up the return type;
		}
	}

	// Verify right number of names for operator.
	if (Kind && ArgNames.size() != Kind)
		return LogErrorP("Invalid number of operands for operator");

	return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames), ReturnType, Kind != 0, BinaryPrecedence);
}

std::unique_ptr<FunctionAST> Parser::ParseFnDef()
{
	getNextToken(); // eat next token
	auto Proto = ParsePrototype(false);
	if (!Proto)
		return nullptr;

	if (CurTok != TokenType_LBRACE)
		LogError("Expected '{' for function def");
	fprintf(stderr, "[PARSER] Starting to parse body for %s\n", Tokens.tokens[1].contents);

	//getNextToken(); // eat {

	auto Body = ParseBlock();
	if (Body.empty())
	{
		printf("[PARSER-ERR] Body is empty!\n");
		return nullptr;
	}

	return std::make_unique<FunctionAST>(std::move(Proto), std::move(Body));
}

std::unique_ptr<FunctionAST> Parser::ParseTopLevelExpr()
{
	printf("[PARSER-TLE] Starting to parse TLE!\n");
	std::vector<std::unique_ptr<ExprAST>> body;
	if (auto E = ParseExpression()) {
		// Make an anonymous proto.
		auto Proto = std::make_unique<PrototypeAST>("__anon_expr",
			std::vector<std::pair<std::string, DataType>>(), DT_VOID);
		body.push_back(std::move(E));
		return std::make_unique<FunctionAST>(std::move(Proto), std::move(body));
	}
	return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::ParseExtern()
{
	getNextToken(); // eat extern.
	return ParsePrototype(true);
}

std::unique_ptr<ExprAST> Parser::ParseIfExpr()
{
	printf("[PARSER-TLE] Starting to parse IF-Expr AST!\n");
	getNextToken();

	auto Condition = ParseExpression();
	if (!Condition)
		return nullptr;

	if (CurTok != TokenType_THEN)
		return LogError("Expected 'then' after 'if'");
	getNextToken();

	auto Then = ParseExpression();
	
	/*
	if (CurTok == TokenType_SEMICOLON && PeekNextToken().type != TokenType_RBRACE)
	{
		getNextToken(); // eat up ; after return in then
	}
	*/

	if (CurTok != TokenType_ELSE)
		return LogError("Expected 'else'");

	getNextToken();

	auto Else = ParseExpression();
	if (!Else)
		return nullptr;

	return std::make_unique<IfExprAST>(std::move(Condition), std::move(Then),
		std::move(Else));
}

/// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in' expression
std::unique_ptr<ExprAST> Parser::ParseForExpr()
{
	getNextToken();  // eat the for.

	if (CurTok != TokenType_IDENTIFIER)
		return LogError("expected identifier after for");

	std::string IdName = PeekCurrentToken().contents;
	getNextToken();  // eat identifier.
	if (CurTok != TokenType_COLON)
		return LogError("expected ':' after loop var");
	getNextToken(); // eat colon
	auto LoopVarDT = EvaluateDataTypeOfToken(0);
	getNextToken(); // eat datatype

	if (CurTok != TokenType_EQL)
		return LogError("expected '=' after for");
	getNextToken();  // eat '='.


	auto Start = ParseExpression();
	if (!Start)
		return nullptr;
	if (CurTok != TokenType_COMMA)
		return LogError("expected ',' after for start value");
	getNextToken();

	auto End = ParseExpression();
	if (!End)
		return nullptr;

	// The step value is optional.
	std::unique_ptr<ExprAST> Step;
	getNextToken();
	if (CurTok == TokenType_COMMA) {
		getNextToken();
		Step = ParseExpression();
		if (!Step)
			return nullptr;
	}

	if (CurTok != TokenType_FIN)
		return LogError("expected 'in' after for");
	getNextToken();  // eat 'fin'.

	auto Body = ParseExpression();
	if (!Body)
		return nullptr;

	return std::make_unique<ForExprAST>(IdName, LoopVarDT, std::move(Start),
		std::move(End), std::move(Step),
		std::move(Body));
}

std::unique_ptr<ExprAST> Parser::ParseUnary()
{
	// If the current token is not an operator, it must be a primary expr.
	if (!IsOperator(CurTok) || CurTok == '(' || CurTok == ',')
		return ParsePrimary();

	// If this is a unary operator, read it.
	int Opc = CurTok;
	getNextToken();
	if (auto Operand = ParseUnary())
		return std::make_unique<UnaryExprAST>(Opc, std::move(Operand));
	return nullptr;
}

std::unique_ptr<ExprAST> Parser::ParseLetExpr()
{
	getNextToken(); // eat let

	std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> VarNames;

	if (CurTok != TokenType_IDENTIFIER)
		return LogError("Expected identifier after 'let'");

	DataType dt;

	while (true)
	{
		std::string Name = PeekCurrentToken().contents;
		getNextToken(); // eat up identifier

		std::unique_ptr<ExprAST> Init;

		if (CurTok == TokenType_COLON)
		{
			getNextToken(); // eat up colon
			dt = EvaluateDataTypeOfToken(0);
			getNextToken(); // eat up dt
			KnownVars[Name] = dt;
		}

		if (CurTok == TokenType_EQL)
		{
			getNextToken(); // eat up '='

			Init = ParseExpression();
			if (!Init)
				return nullptr;
		}

		VarNames.push_back(std::make_pair(Name, std::move(Init)));

		if (CurTok != TokenType_COMMA)
			break;
		getNextToken(); // eat the ','

		if (CurTok != TokenType_IDENTIFIER)
			return LogError("expected identifier list after 'let'");
	}

	if (CurTok != TokenType_IN)
	{
		printf("[PARSER-LET] Encountered 'let' without 'in'!\n");
		return std::make_unique<LetExprAST>(std::move(VarNames), nullptr, dt);

		//return LogError("expected 'in' keyword after 'let'");
	}
	getNextToken(); // eat 'in'

	auto Body = ParseExpression();

	if (!Body)
		return nullptr;

	return std::make_unique<LetExprAST>(std::move(VarNames), std::move(Body), dt);
}

std::unique_ptr<ExprAST> Parser::ParseStructExpr()
{
	if (CurTok != TokenType_STRUCT)
		LogError("Expected struct keyword");

	getNextToken();
	auto name = PeekCurrentToken().contents;

	getNextToken();
	if (CurTok != TokenType_LBRACE)
		LogError("Expected lbrace after struct ident");

	std::vector<std::pair<std::string, DataType>> Fields;

	while (CurTok != TokenType_RBRACE)
	{
		getNextToken();
		auto fieldName = PeekCurrentToken().contents;
		getNextToken();
		getNextToken();
		auto fieldType = EvaluateDataTypeOfToken(0);
		Fields.push_back(std::pair<std::string, DataType>(fieldName, fieldType));
		if (PeekNextToken().type == TokenType_COMMA)
			getNextToken();
		if (PeekNextToken().type == TokenType_RBRACE)
			break;
	}
	printf("[PARSER-STRUCT] returning struct with %d fields\n", Fields.size());
	getNextToken(); // eat }
	getNextToken(); // prepare next token
	return std::make_unique<StructExprAST>(std::move(name), std::move(Fields));
}

std::unique_ptr<ExprAST> Parser::ParseReturnExpr()
{
	getNextToken(); // Eat the "return" token
	auto Expr = ParseExpression();
	auto lookahead = PeekNextToken();
	return std::make_unique<ReturnExprAST>(std::move(Expr));
}

std::vector<std::unique_ptr<ExprAST>> Parser::ParseBlock()
{
	std::vector<std::unique_ptr<ExprAST>> statements;

	if (CurTok != TokenType_LBRACE)
	{
		LogError("Expected '{' at beginning of block");
		return statements;
	}
	getNextToken(); // eat {

	while (CurTok != TokenType_RBRACE && CurTok != TokenType_EOF)
	{
		auto statement = ParseExpression();
		if (!statement)
			break;

		fprintf(stderr, "Parsing block, current token: %d\n", CurTok);

		statements.push_back(std::move(statement));
	}

	if (CurTok != TokenType_RBRACE)
	{
		LogError("Expected '}' at the end of block");
		return statements;
	}

	//getNextToken(); // eat }

	return statements;
}


std::unique_ptr<ScopeExprAST> Parser::ParseScopeExpr()
{
	bool persistent = false;
	Token lookahead;
	if (CurTok == TokenType_PERSISTENT)
	{
		persistent = true;
		getNextToken();
	}
	if (CurTok != TokenType_SCOPE)
	{
		LogError("Expected 'scope' keyword, falling back to global scope");
		return std::make_unique<ScopeExprAST>("GLOBAL", true, ParseBlock());
	}

	getNextToken(); // eat 'scope'
	std::string scopeIdentifier = PeekCurrentToken().contents;
	getNextToken(); // eat identifier
	auto body = ParseBlock();
	if (CurTok != TokenType_RBRACE)
		getNextToken();
	if (CurTok != TokenType_RBRACE)
		LogError("Expected '}' at the end of a scope!");
	return std::make_unique<ScopeExprAST>(scopeIdentifier, persistent, std::move(body));
}



// Handlers

void Parser::HandleDefinition()
{

	CodegenVisitor visitor;

	printf("[PARSER-Init] Parsing Definition\n");

	std::unique_ptr<FunctionAST> function = ParseFnDef();

	if (function) {
		fprintf(stderr, "[PARSER-DONE] Got FnAST\n");
		if (auto* FnIR = function->accept(&visitor)) {
			fprintf(stderr, "[PARSER-CG-DONE] Read FN expression: \n");
			FnIR->print(errs());
			fprintf(stderr, "\n");
		}
	}
	else {
		// Skip token for error recovery.
		getNextToken();
	}
}

void Parser::HandleTopLevelExpression()
{

	CodegenVisitor visitor;

	printf("[PARSER-Init] Parsing TLE.\n");
	std::unique_ptr<FunctionAST> function = ParseTopLevelExpr();
	std::unique_ptr<PrototypeAST> proto = nullptr;

	if (function)
	{
		fprintf(stderr, "[PARSER-DONE] Got TLE-FN-AST\n");
		if (auto* FnIR = function->accept(&visitor)) {
			fprintf(stderr, "[PARSER-CG-DONE] Read TLE FN expression: \n");
			FnIR->print(errs());
			fprintf(stderr, "\n");
			FnIR->eraseFromParent();
		}
	}
	else {
		// Skip token for error recovery.
		getNextToken();
	}
}

void Parser::HandleExtern()
{
	CodegenVisitor visitor;

	if (auto ProtoAST = ParseExtern()) {
		if (auto* FnIR = ProtoAST->accept(&visitor)) {
			fprintf(stderr, "Read extern: ");
			FnIR->print(errs());
			fprintf(stderr, "\n");
		}
	}
	else {
		// Skip token for error recovery.
		getNextToken();
	}
}

void Parser::HandleScopeExpression()
{
	CodegenVisitor visitor;
	std::unique_ptr<ScopeExprAST> ScopeAST = ParseScopeExpr();
	if (ScopeAST) {
		auto* FnIR = ScopeAST->accept(&visitor);
		fprintf(stderr, "Parsed Scope Expression\n");
		
	}

}

void Parser::HandleExternJIT()
{
	
}
void Parser::HandleDefinitionJIT()
{
	
}
void Parser::HandleTopLevelExpressionJIT()
{
	
}


void DebugAST()
{
	
}


// Getter for Tokens buf

Token Parser::getNextToken()
{
	Token tok = Tokens.tokens[Position];
	CurTok = tok.type;
	if (Position < Count)
	{
		fprintf(stderr, "[PARSER] Getting token nr. %d val: %s, type: %d \n", Position, tok.contents, tok.type);
		Position++;
	}
	else
	{
		//Todo: delete token array and signal done.
	}
	return tok;
}

int Parser::GetTokenPrecedence()
{
	char* tokenVal = Tokens.tokens[Position - 1].contents;
	if (!isascii(*tokenVal))
		return -1;

	int TokPrec = BinopPrecedence[*tokenVal];
	if (TokPrec <= 0)
		return -1;
	return TokPrec;
}


// Debug
void Parser::outputVals()
{
	int i = 0;
	while (Position < Count - 1)
	{
		Token tok = getNextToken();
		fprintf(stderr, "[PARSER] Token %d val: %s, type: %d \n", i, tok.contents, tok.type);
		i++;
	}
}

Token Parser::PeekNextNextToken()
{
	return Tokens.tokens[Position + 1];
}

Token Parser::PeekNextToken()
{
	return Tokens.tokens[Position];
}

Token Parser::PeekCurrentToken()
{
	return Tokens.tokens[Position - 1];
}

// 0 = Current, 1 = next, 2 = next next
DataType Parser::EvaluateDataTypeOfToken(int tokenPos)
{
	Token token;
	switch (tokenPos)
	{
	case 0: { token = PeekCurrentToken(); break; }
	case 1: { token = PeekNextToken(); break; }
	case 2: { token = PeekNextNextToken(); break; }
	default: { LogError("Specify 0, 1 or 2 as tokenPos at EvaluateDataTypeOfToken! "); break; }
	}

	fprintf(stderr, "Got token %s\n", token.contents);

	if (strcompare(token.contents, "struct"))
	{
		printf("Returning i32\n");
		return DT_STRUCT;
	}

	if (strcompare(token.contents, "i8"))
	{
		printf("Returning i8\n");
		return DT_I8;
	}
	if (strcompare(token.contents, "i32"))
	{
		printf("Returning i32\n");
		return DT_I32;
	}
	if (strcompare(token.contents, "i64"))
	{
		printf("Returning i64\n");
		return DT_I64;
	}
	if (strcompare(token.contents, "double"))
		return DT_DOUBLE;
	if (strcompare(token.contents, "bool"))
		return DT_BOOL;
	if (strcompare(token.contents, "float"))
		return DT_FLOAT;
	if (strcompare(token.contents, "void"))
		return DT_VOID;

	return DT_UNKNOWN;
}


bool Parser::IsOperator(int type)
{
	switch (type)
	{
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
	case TokenType_MOD:
	case TokenType_EXCL:
		return true;
	default:
		return false;
	}
}