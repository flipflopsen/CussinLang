@echo off

rem Set the build directory path
set BUILD_DIR=build

rem Create the build directory if it doesn't exist
if not exist %BUILD_DIR% (
  mkdir %BUILD_DIR%
) else (
  rem Delete the contents of the build directory
  rd /s /q %BUILD_DIR%
  mkdir %BUILD_DIR%
)

rem Move into the build directory
cd %BUILD_DIR%

rem Generate the build files using CMake
cmake ..

rem Build the project with Debug configuration
cmake --build . --config Debug

rem Move into the Debug directory
cd Debug

rem Run the CussingLangImpl executable
CussingLangImpl.exe