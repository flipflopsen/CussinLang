# Repository Structure

## Project Overview

This repository contains a C++ project named "CussinLang", which appears to be a language implementation or compiler. The project includes source code for a parser, lexer, and potentially a compiler for a custom language.

## Directory Structure

- `src/` - Main source code directory
  - `main.cpp` - Entry point of the application
  - `parser.cpp` - Parser implementation
  - `parser.h` - Parser header file
  - `lexer.cpp` - Lexer implementation
  - `lexer.h` - Lexer header file
  - `ast.cpp` - Abstract Syntax Tree implementation
  - `ast.h` - Abstract Syntax Tree header file
  - `token.cpp` - Token implementation
  - `token.h` - Token header file
  - `error.cpp` - Error handling implementation
  - `error.h` - Error handling header file
  - `utils.cpp` - Utility functions
  - `utils.h` - Utility header file
  - `compiler.cpp` - Compiler implementation
  - `compiler.h` - Compiler header file
  - `scope.cpp` - Scope handling implementation
  - `scope.h` - Scope handling header file
  - `symbol.cpp` - Symbol table implementation
  - `symbol.h` - Symbol table header file
  - `type.cpp` - Type handling implementation
  - `type.h` - Type handling header file

- `include/` - Header files directory
  - `CussinLang.h` - Main header file

- `tests/` - Test files directory
  - `test_main.cpp` - Test entry point
  - `test_parser.cpp` - Parser tests
  - `test_lexer.cpp` - Lexer tests
  - `test_ast.cpp` - AST tests
  - `test_compiler.cpp` - Compiler tests
  - `test_scope.cpp` - Scope tests
  - `test_symbol.cpp` - Symbol table tests
  - `test_type.cpp` - Type handling tests

- `docs/` - Documentation directory
  - `README.md` - Project documentation
  - `Problems.md` - Documentation of issues
  - `TODO.md` - Development tasks

- `CussingLangImpl.sln` - Visual Studio solution file
- `.gitignore` - Git ignore rules

## Entry Points

- `src/main.cpp` - Main entry point for the application
- `tests/test_main.cpp` - Main entry point for tests

## Technologies Used

- C++ (C++17 or later)
- CMake (assumed, based on project structure)
- Visual Studio (Windows development environment)

## Build and Development Setup

- The project appears to be set up for Visual Studio development
- Contains a .sln file for Visual Studio
- Uses standard C++ header/source file separation
- Includes unit tests for components

## Key Files

- `src/main.cpp` - Entry point for the compiler
- `src/parser.h` - Parser interface definition
- `src/lexer.h` - Lexer interface definition
- `src/ast.h` - AST interface definition
- `src/compiler.h` - Compiler interface definition
- `src/error.h` - Error handling interface
- `include/CussinLang.h` - Main public interface

## Development Workflow

- Development is done using Visual Studio
- Tests are included in the tests directory
- Code is organized in a standard C++ project structure
- Documentation is maintained in the docs directory