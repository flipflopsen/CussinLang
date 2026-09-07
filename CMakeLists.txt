cmake_minimum_required(VERSION 3.29)

# The vcpkg toolchain must be supplied before project(), through
# CMakeSettings.json or -DCMAKE_TOOLCHAIN_FILE=...
project(
    CussingLangImpl
    VERSION 0.1.0
    LANGUAGES C CXX
)

# -----------------------------------------------------------------------------
# Project configuration
# -----------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Deliberately keep LTO disabled for the supplied build profiles.
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)

option(BUILD_TESTING "Build the unit tests" OFF)
option(CUSS_DEBUG_INFO "Generate application debug information" ON)
option(CUSS_ENABLE_PCH "Use a private precompiled header for core_lib" OFF)
option(CUSS_ENABLE_UNITY "Use unity compilation for core_lib" OFF)

set(
    CUSS_EXPECT_LLVM_ASSERTIONS "AUTO"
    CACHE STRING
    "Required assertions setting of the installed LLVM: AUTO, ON, or OFF"
)

set_property(
    CACHE CUSS_EXPECT_LLVM_ASSERTIONS
    PROPERTY STRINGS AUTO ON OFF
)

if(NOT CUSS_EXPECT_LLVM_ASSERTIONS MATCHES "^(AUTO|ON|OFF)$")
    message(FATAL_ERROR
        "CUSS_EXPECT_LLVM_ASSERTIONS must be AUTO, ON, or OFF."
    )
endif()

# CMake >= 3.25, with CMP0141 NEW:
# Embedded means /Z7 rather than compiler-generated shared PDBs.
if(MSVC)
    if(CUSS_DEBUG_INFO)
        set(
            CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
            "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>"
        )
    else()
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "")
    endif()
endif()

# Avoid simultaneous large executable links.
# This limits links, not the number of compile jobs.
if(CMAKE_GENERATOR MATCHES "Ninja")
    set_property(GLOBAL APPEND PROPERTY JOB_POOLS cuss_link_pool=1)
endif()

include(CTest)

# -----------------------------------------------------------------------------
# LLVM dependency
# -----------------------------------------------------------------------------

find_package(LLVM CONFIG REQUIRED)

message(STATUS "LLVM version: ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM package directory: ${LLVM_DIR}")

if(DEFINED LLVM_ENABLE_ASSERTIONS)
    message(STATUS "Installed LLVM assertions: ${LLVM_ENABLE_ASSERTIONS}")
else()
    message(STATUS "Installed LLVM does not export its assertions setting")
endif()

if(NOT CUSS_EXPECT_LLVM_ASSERTIONS STREQUAL "AUTO")
    if(NOT DEFINED LLVM_ENABLE_ASSERTIONS)
        message(FATAL_ERROR
            "Cannot verify the selected LLVM assertions setting. "
            "Choose a package that exports LLVM_ENABLE_ASSERTIONS, or use "
            "CUSS_EXPECT_LLVM_ASSERTIONS=AUTO after checking it manually."
        )
    endif()

    if(CUSS_EXPECT_LLVM_ASSERTIONS STREQUAL "ON"
       AND NOT LLVM_ENABLE_ASSERTIONS)
        message(FATAL_ERROR
            "This configuration requires LLVM built with assertions ON. "
            "Select the corresponding installation using LLVM_DIR."
        )
    endif()

    if(CUSS_EXPECT_LLVM_ASSERTIONS STREQUAL "OFF"
       AND LLVM_ENABLE_ASSERTIONS)
        message(FATAL_ERROR
            "This configuration requires LLVM built with assertions OFF. "
            "Select the corresponding installation using LLVM_DIR."
        )
    endif()
endif()

# Preserve the components requested by the original project.
# Add OrcJIT here if CussinJIT.h actually uses LLVM's ORC implementation.
llvm_map_components_to_libnames(
    CUSS_LLVM_LIBRARIES
    Analysis
    Core
    ExecutionEngine
    InstCombine
    Linker
    Object
    RuntimeDyld
    ScalarOpts
    Support
    IRReader
    native
    MCJIT
    BinaryFormat
)

# Normalize LLVM_DEFINITIONS:
# LLVM packages may expose a space-separated string or a CMake list.
set(CUSS_LLVM_DEFINITIONS)

foreach(_entry IN LISTS LLVM_DEFINITIONS)
    separate_arguments(_tokens NATIVE_COMMAND "${_entry}")

    foreach(_token IN LISTS _tokens)
        string(REGEX REPLACE "^(-D|/D)" "" _definition "${_token}")

        if(NOT _definition STREQUAL "")
            list(APPEND CUSS_LLVM_DEFINITIONS "${_definition}")
        endif()
    endforeach()
endforeach()

list(REMOVE_DUPLICATES CUSS_LLVM_DEFINITIONS)

add_library(cuss_llvm INTERFACE)

target_include_directories(
    cuss_llvm
    SYSTEM INTERFACE
        ${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(
    cuss_llvm
    INTERFACE
        ${CUSS_LLVM_DEFINITIONS}
)

target_link_libraries(
    cuss_llvm
    INTERFACE
        ${CUSS_LLVM_LIBRARIES}
)

# Do not include HandleLLVMOptions:
# it configures LLVM's own build, not just LLVM consumer requirements.

# -----------------------------------------------------------------------------
# Sources
# -----------------------------------------------------------------------------

set(APP_SOURCES
    src/CussingLangImpl.cpp
)

set(CORE_SOURCES
    src/CussingLangImpl.h

    # Lexer and parser
    src/lang/lexer.h
    src/lang/lexer.cpp
    src/lang/parser.h
    src/lang/parser.cpp

    # AST
    src/lang/ast/headers/ExpressionAST.h
    src/lang/ast/headers/CodegenVisitor.h

    src/lang/ast/headers/BinaryExpressionAST.h
    src/lang/ast/BinaryExpressionAST.cpp

    src/lang/ast/headers/CallExpressionAST.h
    src/lang/ast/CallExpressionAST.cpp

    src/lang/ast/headers/ForExpressionAST.h
    src/lang/ast/ForExpressionAST.cpp

    src/lang/ast/headers/FunctionExpressionAST.h
    src/lang/ast/FunctionExpressionAST.cpp

    src/lang/ast/headers/IfExpressionAST.h
    src/lang/ast/IfExpressionAST.cpp

    src/lang/ast/headers/LetExpressionAST.h
    src/lang/ast/LetExpressionAST.cpp

    src/lang/ast/headers/NumberExpressionAST.h
    src/lang/ast/NumberExpressionAST.cpp

    src/lang/ast/headers/PrototypeExpressionAST.h
    src/lang/ast/PrototypeExpressionAST.cpp

    src/lang/ast/headers/UnaryExpressionAST.h
    src/lang/ast/UnaryExpressionAST.cpp

    src/lang/ast/headers/VariableExpressionAST.h
    src/lang/ast/VariableExpressionAST.cpp

    src/lang/ast/headers/StructExpressionAST.h
    src/lang/ast/StructExpressionAST.cpp

    src/lang/ast/headers/ReturnExpressionAST.h
    src/lang/ast/ReturnExpressionAST.cpp

    src/lang/ast/headers/ScopeExpressionAST.h
    src/lang/ast/ScopeExpressionAST.cpp

    src/lang/ast/headers/StringExpressionAST.h
    src/lang/ast/StringExpressionAST.cpp

    src/lang/ast/headers/Visitor.h
    src/lang/ast/CodegenVisitor.cpp

    # LLVM code generation
    src/llvmstuff/codegen.h
    src/llvmstuff/codegen.cpp
    src/llvmstuff/SymbolTable.h
    src/llvmstuff/datastorage.h
    src/llvmstuff/ContextManager.h
    src/llvmstuff/ModuleManager.h
    src/llvmstuff/ScopeManager.h
    src/llvmstuff/ScopeManager.cpp
    src/jit/CussinJIT.h

    # Utilities
    src/utils/DataTypes.h
    src/utils/util.h
    src/utils/logger.h
    src/utils/logger.cpp
    src/utils/BinopPrecedence.h
    src/utils/DebugInfoGenerator.h
    src/utils/memethods.h
    src/utils/memethods.cpp
)

# -----------------------------------------------------------------------------
# Core library
# -----------------------------------------------------------------------------

add_library(core_lib STATIC ${CORE_SOURCES})
add_library(CussingLang::Core ALIAS core_lib)

target_compile_features(core_lib PUBLIC cxx_std_17)

target_include_directories(
    core_lib
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# PUBLIC is intentional: the current public headers appear to expose LLVM.
# After isolating LLVM implementation details, some requirements may become
# PRIVATE.
target_link_libraries(
    core_lib
    PUBLIC
        cuss_llvm
)

if(CUSS_ENABLE_PCH)
    target_precompile_headers(
        core_lib
        PRIVATE
            <string>
            <vector>
            <memory>
            <unordered_map>
            <llvm/ADT/StringRef.h>
            <llvm/IR/LLVMContext.h>
            <llvm/IR/Module.h>
            <llvm/IR/IRBuilder.h>
    )
endif()

set_target_properties(
    core_lib
    PROPERTIES
        UNITY_BUILD "${CUSS_ENABLE_UNITY}"
        UNITY_BUILD_BATCH_SIZE 4
)

# -----------------------------------------------------------------------------
# Application
# -----------------------------------------------------------------------------

add_executable(CussingLangImpl ${APP_SOURCES})

target_link_libraries(
    CussingLangImpl
    PRIVATE
        CussingLang::Core
)

if(CMAKE_GENERATOR MATCHES "Ninja")
    set_property(
        TARGET CussingLangImpl
        PROPERTY JOB_POOL_LINK cuss_link_pool
    )
endif()

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(
        UnitTests
        src/test/CussinLangImplTest.cpp
    )

    target_link_libraries(
        UnitTests
        PRIVATE
            CussingLang::Core
            GTest::gtest_main
            GTest::gmock
    )

    if(CMAKE_GENERATOR MATCHES "Ninja")
        set_property( TARGET UnitTests
            PROPERTY JOB_POOL_LINK cuss_link_pool
        )
    endif()

    include(GoogleTest)

    # Discover individual GoogleTest cases when CTest runs, rather than
    # executing the test binary immediately after every relink.
    gtest_discover_tests(
        UnitTests
        DISCOVERY_MODE PRE_TEST
        DISCOVERY_TIMEOUT 30
    )
endif()

# -----------------------------------------------------------------------------
# Visual Studio source organization
# -----------------------------------------------------------------------------

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PREFIX "Source Files"
    FILES ${APP_SOURCES} ${CORE_SOURCES}
)

if(BUILD_TESTING)
    source_group(
        TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
        PREFIX "Source Files"
        FILES src/test/CussinLangImplTest.cpp
    )
endif()