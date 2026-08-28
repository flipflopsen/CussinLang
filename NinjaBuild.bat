@echo off

rem Set the default build directory (can be overridden with -o)
set BUILD_DIR=NinjaBuild

rem ===== Check for CMake =====
cmake --version >nul 2>&1 
IF %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake is not found in your PATH environment variable.
    echo Please specify the path to your CMake executable below:
    set /p cmakePath="Enter CMake path (e.g., C:\Program Files\CMake\bin\cmake.exe): "
) ELSE (
    set cmakePath=cmake
)

rem ===== Argument Parsing =====
:enter_cmake_path
shift
if "%~1" == "" (
    goto :all  
) 

rem Check for flags and process accordingly
if "%~1" == "-g" (
    goto :generate
) else if "%~1" == "-b" (
    goto :build
) else if "%~1" == "-r" (
    goto :run
) else if "%~1" == "-o" (
    if "%~2" == "" (
        echo ERROR: Output directory not specified with -o flag.
        goto :show_help
    )
    set BUILD_DIR=%~2
    shift  
    goto :enter_cmake_path
) else if "%~1" == "-h" (
    goto :show_help
) else if "%~1" == "-a" (
    goto :all
) else (
    echo ERROR: Invalid argument "%~1"
    goto :show_help
)
goto :enter_cmake_path 

rem ===== ACTIONS =====
:generate
echo Generating build files...
%cmakePath% -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=B:\Programs\vcpkg\scripts\buildsystems\vcpkg.cmake
goto :eof

:build
echo Building the project...
%cmakePath% --build . --config Debug --parallel 24
goto :eof

:run
echo Running the application...
cmd /c ".\%BUILD_DIR%\CussingLangImpl.exe"
goto :eof

:all
echo Generating, building, and running...
call :generate
call :build
call :run
goto :eof

:show_help
echo Usage: build_run.bat [options]
echo Options:
echo   -g   Generate build files only
echo   -b   Build the generated files
echo   -r   Run the application
echo   -o   Specify output directory (overrides default)
echo   -h   Show this help
echo   -a   Generate build files, build, and run (default)

:eof 
