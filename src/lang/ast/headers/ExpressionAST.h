#ifndef EXPRESSIONAST_H
#define EXPRESSIONAST_H

#include <algorithm>
#include <cctype>
#include <memory>
#include <string>
#include <vector>

#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "CodegenVisitor.h"

class Visitor;

/// ExpressionAST superclass
class ExprAST
{
public:
	virtual ~ExprAST() = default;
	virtual llvm::Value* accept(Visitor* visitor) = 0;

};

#include "../../../utils/Datatypes.h"
#include "../../../llvmstuff/datastorage.h"

#endif