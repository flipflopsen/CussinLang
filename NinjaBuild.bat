@echo off

rem Set the default build directory (can be overridden with -o)
set BUILD_DIR=NinjaBuild

rem ===== Argument Parsing =====
:begin
shift

rem Check for flags and process accordingly
if "%~0" == "-g" (
    goto :generate
) else if "%~0" == "-b" (
    goto :build
) else if "%~0" == "-r" (
    goto :run
) else if "%~0" == "-o" (
    if "%~1" == "" (
        echo ERROR: Output directory not specified with -o flag.
        goto :show_help
    )
    set BUILD_DIR=%~2
    shift  
    goto :begin
) else if "%~0" == "-h" (
    goto :show_help
) else if "%~0" == "-a" (
    goto :all
) else (
    goto :all
)
goto :begin 

rem ===== ACTIONS =====
:generate
echo Generating build files...
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=B:\Programs\vcpkg\scripts\buildsystems\vcpkg.cmake ..
goto :eof

:build
echo Building the project...
cmake --build . --config Debug --parallel 24
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