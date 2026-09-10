# CussinLang

CussinLang is a small, statically/strongly typed toy programming language implemented in C++17, with a hand-written lexer and recursive-descent parser, an AST-based intermediate representation, and an LLVM-backed code generator/JIT. It was built as a learning project for exploring compiler and programming-language-implementation techniques.

## Status

This repository is an experimental/learning project. It is published primarily for technical documentation and portfolio purposes and should not be considered production-ready. The build depends on a hardcoded, machine-specific vcpkg path (see "Known limitations"), and build/test success has not been verified as part of this publication pass.

## About the Naming
The reason I've initially picked "CussinLang" as the name for the project is, that I initially created this project to learn C++ and educate myself further in compiler theory.
I think a lot of experienced C++ developers will understand this decision.

Why didn't I change the name?
Before publishing it to GitHub for Portfolio reasons, I wanted to update packages (including LLVM) and make the code easier to compile and execute.
As obviously visible, this is still WIP, thus the name will stay as it is as a humorous name for a C++ & LLVM project.

## Overview

CussinLang compiles source text through the following stages:

- **Lexer** (`src/lang/lexer.cpp`) — tokenizes source text into a stream of `Token`/`TokenType` values.
- **Parser** (`src/lang/parser.cpp`) — a recursive-descent parser that builds an Abstract Syntax Tree (AST) from tokens, using an operator-precedence table (`src/utils/BinopPrecedence.h`) for binary expressions.
- **AST** (`src/lang/ast/`) — expression/statement node types (binary, unary, call, for, if, let, function, prototype, return, scope, struct, string, number, variable), each with a corresponding header in `src/lang/ast/headers/`.
- **Codegen** (`src/lang/ast/CodegenVisitor.cpp`, `src/llvmstuff/codegen.cpp`) — a visitor (`Visitor.h`/`CodegenVisitor.h`) that walks the AST and emits LLVM IR, using a symbol table and scope manager (`src/llvmstuff/SymbolTable.h`, `src/llvmstuff/ScopeManager.h`).
- **JIT** (`src/jit/CussinJIT.h`) — LLVM ORC-based JIT execution support (invocation currently commented out in `src/CussingLangImpl.cpp`).

## Features

Based on the AST node types and language example present in the codebase:

- Function declarations with typed parameters and return types (e.g. `i64`)
- `let` variable bindings with explicit types
- `if` and `for` control-flow expressions
- Lexical scopes (`ScopeExpressionAST`)
- Struct declarations (`StructExpressionAST`) — present in the AST but shown only as a commented-out example in `src/CussingLangImpl.cpp`
- Unary/binary expressions with operator precedence, function calls, string and numeric literals

## Example syntax

The following snippet reflects sample program strings found (commented out) in `src/CussingLangImpl.cpp`; it illustrates the language's function/`let`/`return` syntax but has not been independently re-verified to compile in this pass:

```text
fn test(x: i32, y: i32) -> i32 { let z: i32 = (x + y) * 2; x = z + 3; return x; }
```

## Architecture

```text
src/
  CussingLangImpl.cpp/.h   # Entry point, sample driver code
  jit/                     # LLVM ORC JIT integration
  lang/
    lexer.cpp/.h           # Tokenizer
    parser.cpp/.h          # Recursive-descent parser
    ast/                   # AST node implementations
      headers/             # AST node type declarations, Visitor interface
  llvmstuff/               # LLVM IR codegen, symbol table, scope manager, data storage
  utils/                  # Data types, operator precedence table, logger, memory helpers, debug-info generator
  test/                   # CussinLangImplTest.cpp (not currently wired into the CMake build)
```

## Building and running

The project builds with CMake (Ninja generator) and MSVC, and depends on LLVM and vcpkg.

Prerequisites:
- A C++17 compiler (MSVC, per `CMakePresets.json` and `NinjaBuild.ps1`)
- LLVM development libraries discoverable via `find_package(LLVM CONFIG REQUIRED)`
- vcpkg

Note: `CMakeLists.txt` currently hardcodes the vcpkg toolchain path (`include(B:/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake)`), and `NinjaBuild.ps1`/`NinjaBuild.bat` hardcode local Visual Studio and vcpkg paths. These will need to be adjusted to match your local environment before configuring.

Configure/build commands:

```powershell
cmake -B out -G Ninja -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build out
```

**Verification status:** A fresh `cmake -B out_validate -G Ninja -DCMAKE_TOOLCHAIN_FILE=...` configure was re-attempted in the current environment (using the hardcoded vcpkg path present in `CMakeLists.txt`, which does exist on this machine) and failed immediately during compiler detection: the default toolset resolved to a Clang-based compiler, and CMake reported `MSVC_DEBUG_INFORMATION_FORMAT value 'ProgramDatabase' not known for this C compiler`. This is an environment/toolset selection issue (the project expects `cl.exe`, per `CMakePresets.json`) rather than a defect confirmed in the C++ sources, and it occurred before the missing sources listed below or LLVM discovery could be evaluated. `cmake --build` was not attempted. Build success remains unverified overall.

## Known limitations

- The vcpkg toolchain path in `CMakeLists.txt` is hardcoded to a specific machine and must be changed to build elsewhere.
- Testing is not enabled in the build (`enable_testing()` is commented out in `CMakeLists.txt`); `src/test/CussinLangImplTest.cpp` exists but its integration is unverified.
- JIT execution is present in the codebase but currently commented out in the sample entry point.
- `CMakeLists.txt` lists several source files that are not currently tracked by git and only exist (or are missing entirely) in the local working tree: `src/utils/memethods.h`/`src/utils/memethods.cpp` (present only at the repository root, not under `src/utils/`), `src/lang/ast/StringExpressionAST.cpp`, `src/lang/ast/headers/StringExpressionAST.h`, and `src/utils/DebugInfoGenerator.h`. A fresh checkout will fail to configure/build until these are added to version control in their expected locations.
- A from-scratch CMake configure was attempted in a separate build directory and failed at the compiler-detection stage due to a Clang/MSVC toolset mismatch in the current environment (see "Building and running"); overall build success remains unverified.
- No LICENSE file is currently present in this repository.
