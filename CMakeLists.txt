# CMakeList.txt : CMake project for CussingLangImpl, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
#if (POLICY CMP0141)
#  cmake_policy(SET CMP0141 NEW)
#  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
#endif()


project ("CussingLangImpl")

set(CMAKE_CXX_STANDARD 17)

set (LLVM_DIR "G:\\Programme\\LLVM\\lib\\cmake\\llvm\\")

# Find LLVM package
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(SOURCES
		"src/lang/lexer.cpp"
		"src/lang/lexer.h"
		"src/lang/parser.cpp"
		"src/lang/parser.h"
		"src/utils/util.h"
		"src/CussingLangImpl.cpp"
		"src/CussingLangImpl.h"
		"src/llvmstuff/codegen.h"
		"src/llvmstuff/codegen.cpp"
		"src/jit/CussinJIT.h"
		"src/utils/DataTypes.h" 
		"src/lang/ast/ast.h"
		"src/lang/ast/BinaryExpressionAST.h"
		"src/lang/ast/CallExpressionAST.h"
		"src/lang/ast/ForExpressionAST.h"
		"src/lang/ast/FunctionExpressionAST.h"
		"src/lang/ast/IfExpressionAST.h"
		"src/lang/ast/LetExpressionAST.h"
		"src/lang/ast/NumberExpressionAST.h"
		"src/lang/ast/PrototypeExpressionAST.h"
		"src/lang/ast/UnaryExpressionAST.h"
		"src/lang/ast/VariableExpressionAST.h"
		"src/lang/ast/StructExpressionAST.h"
		"src/lang/ast/ReturnExpressionAST.h"
		"src/lang/ast/ExpressionAST.h" 
		"src/lang/ast/Visitor.h"
		"src/llvmstuff/visitors.h"
)

# Add source to this project's executable.
add_executable (CussingLangImpl ${SOURCES})

# Link LLVM libraries and flags
llvm_map_components_to_libnames(LLVM_LIBS
  #all
  Analysis
  Core
  ExecutionEngine
  InstCombine
  Object
  RuntimeDyld
  ScalarOpts
  Support
  irreader
  native
  mcjit
)

foreach(target ${LLVM_TARGETS_TO_BUILD})
  list(APPEND targets "LLVM${target}CodeGen")
endforeach()

target_link_libraries(CussingLangImpl ${LLVM_LIBS} ${targets})
