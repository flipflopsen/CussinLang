# CMakeList.txt : CMake project for CussingLangImpl, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20.0)

# Enable Hot Reload for MSVC compilers if supported.
#if (POLICY CMP0141)
#  cmake_policy(SET CMP0141 NEW)
#  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
#endif()

project ("CussingLangImpl")

set(CMAKE_CXX_STANDARD 17)

# Include vcpkg
include(B:/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake)

# Find LLVM package
find_package(LLVM CONFIG REQUIRED)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

#enable_testing()

set(SOURCES
		# Main Entry Point for Testing and Debugging etc.
		"src/CussingLangImpl.cpp"
		"src/CussingLangImpl.h"

		# Lexer and Parser
		"src/lang/lexer.cpp"
		"src/lang/lexer.h"
		"src/lang/parser.cpp"
		"src/lang/parser.h"

		# AST Headers
		"src/lang/ast/headers/ExpressionAST.h" 
		"src/lang/ast/headers/CodegenVisitor.h"
		"src/lang/ast/headers/BinaryExpressionAST.h"
		"src/lang/ast/headers/CallExpressionAST.h"
		"src/lang/ast/headers/ForExpressionAST.h"
		"src/lang/ast/headers/FunctionExpressionAST.h"
		"src/lang/ast/headers/IfExpressionAST.h"
		"src/lang/ast/headers/LetExpressionAST.h"
		"src/lang/ast/headers/NumberExpressionAST.h"
		"src/lang/ast/headers/PrototypeExpressionAST.h"
		"src/lang/ast/headers/UnaryExpressionAST.h"
		"src/lang/ast/headers/VariableExpressionAST.h"
		"src/lang/ast/headers/StructExpressionAST.h"
		"src/lang/ast/headers/ReturnExpressionAST.h"	
		"src/lang/ast/headers/ScopeExpressionAST.h"  
		"src/lang/ast/headers/Visitor.h"

		# AST Impl
		"src/lang/ast/BinaryExpressionAST.cpp"
		"src/lang/ast/CallExpressionAST.cpp" 
		"src/lang/ast/ScopeExpressionAST.cpp" 
		"src/lang/ast/ReturnExpressionAST.cpp" 
		"src/lang/ast/LetExpressionAST.cpp" 
		"src/lang/ast/ForExpressionAST.cpp" 
		"src/lang/ast/IfExpressionAST.cpp" 
		"src/lang/ast/UnaryExpressionAST.cpp" 
		"src/lang/ast/FunctionExpressionAST.cpp" 
		"src/lang/ast/NumberExpressionAST.cpp" 
		"src/lang/ast/VariableExpressionAST.cpp" 
		"src/lang/ast/PrototypeExpressionAST.cpp" 
		"src/lang/ast/StructExpressionAST.cpp"
		"src/lang/ast/CodegenVisitor.cpp"

		# LLVM Codegen STuff
		"src/llvmstuff/codegen.h"
		"src/llvmstuff/codegen.cpp"
		"src/llvmstuff/SymbolTable.h" 
		"src/llvmstuff/ScopeManager.h"
		"src/llvmstuff/datastorage.h" 
		"src/jit/CussinJIT.h"

		# Utils
		"src/utils/DataTypes.h"
		"src/utils/util.h"
		"src/utils/logger.h" 
		"src/utils/BinopPrecedence.h" 
		"src/utils/logger.cpp")

# Add source to this project's executable.
add_executable (CussingLangImpl ${SOURCES})

# Link LLVM libraries and flags
llvm_map_components_to_libnames(LLVM_LIBS
  Analysis
  Core
  ExecutionEngine
  InstCombine
  Linker
  Object
  RuntimeDyld
  ScalarOpts
  Support
  irreader
  native
  mcjit
)

add_compile_options(/FR)

foreach(target ${LLVM_TARGETS_TO_BUILD})
  list(APPEND targets "LLVM${target}CodeGen")
endforeach()

target_link_libraries(CussingLangImpl ${LLVM_LIBS} ${targets})