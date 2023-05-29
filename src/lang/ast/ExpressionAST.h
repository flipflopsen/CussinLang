#ifndef EXPRESSIONAST_H
#define EXPRESSIONAST_H

#include <algorithm>
#include <cctype>
#include <memory>
#include <string>
#include <vector>

#include "../utils/Datatypes.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

#include "Visitor.h"

using namespace llvm;

class ExprAST
{
public:
	virtual ~ExprAST() = default;
	virtual Value* accept(Visitor* visitor) = 0;

};

#endif