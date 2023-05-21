#include "ast.h"
#include <string>
#include <vector>
#include <memory>

class ExprAST {
public:
	virtual ~ExprAST() = default;
};

class NumberExprAST : public ExprAST {
	double Val;

public:
	NumberExprAST(double val) : Val(val) {}
};


/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
	std::string Name;
};


/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
    : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};


/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
    : Callee(Callee), Args(std::move(Args)) {}
};

/// PrototypeAST - represents a Prototype for a function with name, argument names etc.

class PrototypeAST {
	std::string Name;
	std::vector<std::string> Args;
	std::string ReturnType;

public:
  PrototypeAST(const std::string &Name, std::vector<std::string> Args, const std::string &ReturnType)
    : Name(Name), Args(std::move(Args)), ReturnType(ReturnType) {}

  const std::string &getName() const { return Name; }
  const std::string &getReturnType() const {return ReturnType; }
};


class FunctionAST {
	std::unique_ptr<PrototypeAST> Proto;
	std::unique_ptr<ExprAST> Body;

public:
	FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
    : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

class IfExprAST : public ExprAST {
  std::unique_ptr<ExprAST> Cond, Then, Else;

public:
  IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
            std::unique_ptr<ExprAST> Else)
    : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

  //Value *codegen() override;
};

class DoExprAST : public ExprAST {

};

class TryExprAST : public ExprAST {

};

class StructExprAST : public ExprAST {
	std::string Name;
	std::vector<std::unique_ptr<ExprAST>> Contents;

public:
	StructExprAST(std::string Name, std::vector<std::unique_ptr<ExprAST>> Contents)
	: Name(Name), Contents(std::move(Contents)) {}
};

class EnumExprAST : public ExprAST {
	std::string Name;
	std::vector<std::unique_ptr<ExprAST>> Contents;

public:
	EnumExprAST(std::string Name, std::vector<std::unique_ptr<ExprAST>> Contents)
	: Name(Name), Contents(std::move(Contents)) {}
};
