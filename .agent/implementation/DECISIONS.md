# CussinLang Architecture Decisions

## Decision 1: Language Design
- **Date**: 2026-09-03
- **Status**: Active
- **Description**: CussinLang is a statically/strongly typed toy programming language with function declarations, typed variable bindings, control-flow expressions, lexical scopes, struct declarations, and unary/binary expressions with operator precedence.

## Decision 2: Implementation Approach
- **Date**: 2026-09-03
- **Status**: Active
- **Description**: Hand-written lexer and recursive-descent parser with AST-based intermediate representation and LLVM-backed code generation/JIT.

## Decision 3: Build System
- **Date**: 2026-09-03
- **Status**: Active
- **Description**: CMake build system with Ninja generator, using vcpkg for dependency management.

## Decision 4: C++ Standard
- **Date**: 2026-09-03
- **Status**: Active
- **Description**: C++17 standard chosen for compatibility with modern compilers and features.

## Decision 5: LLVM Integration
- **Date**: 2026-09-03
- **Status**: Active
- **Description**: LLVM used for code generation and JIT execution capabilities.

## Decision 6: Testing Strategy
- **Date**: 2026-09-03
- **Status**: Active
- **Description**: GoogleTest (GTest) framework for comprehensive test coverage.

## Decision 7: Documentation
- **Date**: 2026-09-03
- **Status**: Active
- **Description**: Markdown-based documentation for requirements, implementation plan, and architecture decisions.

## Decision 8: Source Code Organization
- **Date**: 2026-09-03
- **Status**: Active
- **Description**: Modular organization with separate directories for source, headers, tests, and utilities.