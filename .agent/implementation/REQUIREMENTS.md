# CussinLang Compiler Requirements

## Project Scope
CussinLang is a statically/strongly typed toy programming language implemented in C++17 with a hand-written lexer and recursive-descent parser, an AST-based intermediate representation, and an LLVM-backed code generator/JIT.

## Core Features
- Function declarations with typed parameters and return types
- `let` variable bindings with explicit types
- `if` and `for` control-flow expressions
- Lexical scopes (`ScopeExpressionAST`)
- Struct declarations (`StructExpressionAST`)
- Unary/binary expressions with operator precedence, function calls, string and numeric literals

## Implementation Stages
1. **Lexer** (`src/lang/lexer.cpp`) - Tokenizes source text into a stream of `Token`/`TokenType` values
2. **Parser** (`src/lang/parser.cpp`) - Recursive-descent parser that builds an Abstract Syntax Tree (AST) from tokens
3. **AST** (`src/lang/ast/`) - Expression/statement node types with corresponding headers
4. **Codegen** (`src/lang/ast/CodegenVisitor.cpp`, `src/llvmstuff/codegen.cpp`) - Visitor that walks the AST and emits LLVM IR
5. **JIT** (`src/jit/CussinJIT.h`) - LLVM ORC-based JIT execution support

## Technical Requirements
- C++17 compiler (MSVC recommended)
- LLVM development libraries discoverable via `find_package(LLVM CONFIG REQUIRED)`
- vcpkg for dependency management
- CMake build system with Ninja generator

## Build Constraints
- Hardcoded vcpkg path in CMakeLists.txt needs to be parameterized
- Missing source files in CMakeLists.txt that need to be tracked
- Compiler-detection failure due to Clang/MSVC toolset mismatch
- Testing not wired into build system
- JIT execution path commented out in sample entry point

## Example Syntax
```text
fn test(x: i32, y: i32) -> i32 { let z: i32 = (x + y) * 2; x = z + 3; return x; }
```

## Known Limitations
1. Hardcoded vcpkg toolchain path in CMakeLists.txt
2. Missing source files in CMakeLists.txt that are not tracked by git
3. Compiler detection failure due to toolset mismatch
4. Testing not enabled in build system
5. JIT execution path commented out
6. No LICENSE file present