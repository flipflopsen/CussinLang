# CMakeList.txt : CMake project for CussingLangImpl, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("CussingLangImpl")


set(SOURCES
		lexer.cpp
		lexer.h
		parser.cpp
		parser.h
		util.h
		CussingLangImpl.cpp
		CussingLangImpl.h
		codegen.h
		codegen.cpp
	)

# Add source to this project's executable.
add_executable (CussingLangImpl ${SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CussingLangImpl PROPERTY CXX_STANDARD 17)
  set(BUILD_ARCH "-m64")
  set(TARGET_64 ON)
endif()

set (LLVM_DIR "G:\\Programme\\LLVM\\lib\\cmake\\llvm\\")
# import LLVM
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs#
	Analysis
	Core
	ExecutionEngine
	InstCombine
	Object
	#PrcJIT
	RuntimeDyld
	ScalarOpts
	Support
	native
)

target_link_libraries(CussingLangImpl ${llvm_libs})

# TODO: Add tests and install targets if needed.
