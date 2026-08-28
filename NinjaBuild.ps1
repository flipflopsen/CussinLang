param(
    [Parameter(ParameterSetName='Default')]
    [switch]$Generate,
    [switch]$Build,
    [switch]$Run,
    [switch]$Test,
    [Parameter(ParameterSetName='FullChainRun')]
    [switch]$FullChainRun,
    [Parameter(ParameterSetName='OutputDir')]
    [string]$OutputDir,
    [switch]$Help
)

# Set the Paths for Ninja and and VS Build tools
[string]$ninjaPath = "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
[string]$compilerBasePath = "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.39.33519\bin\Hostx64\x64"

$env:PATH += ";" + $ninjaPath
$env:PATH += ";" + $compilerBasePath

$env:CMAKE_GENERATOR = "Ninja"  # Set the generator
$env:CMAKE_MAKE_PROGRAM = $ninjaPath
$env:CMAKE_C_COMPILER = $compilerBasePath + "\cl.exe" 
$env:CMAKE_CXX_COMPILER = $compilerBasePath + "\cl.exe" 

# Set the default build directory (can be overridden with -OutputDir)
$BuildDir = "NinjaBuild"

# Function to check if CMake is installed
function Test-CMake {
    try {
        cmake --version > $null 2>&1
        return $true  # CMake found
    } catch {
        return $false # CMake not found
    }
}

# Check for CMake and prompt for path if not found
if (-not (Test-CMake)) {
    $cmakePath = Read-Host -Prompt "Enter CMake path (e.g., C:\Program Files\CMake\bin\cmake.exe): "
} else {
    $cmakePath = "cmake"
}

if ($Help) {
    Write-Host "Usage: build_run.ps1 [options]"
    Write-Host "Options:"
    Write-Host "  -Generate     Generate build files only"
    Write-Host "  -Build        Build the generated files"
    Write-Host "  -Run          Run the application"
    Write-Host "  -OutputDir    Specify output directory (overrides default)"
    Write-Host "  -Test         Run GTests"
    Write-Host "  -Help         Show this help"
    Write-Host "  (Default)     Generate build files, build, and run"
    exit
}

Write-Host "Parameters: Generate=$Generate Build=$Build Run=$Run Test=$Test OutputDir=$OutputDir Help=$Help"  

if ($FullChainRun -Or $Generate -Or !$Generate -And !$Build -And !$Run -And !$Test) {
    Write-Host "Generating build files..."
    & $cmakePath -G "Ninja" -B $BuildDir -DCMAKE_TOOLCHAIN_FILE=B:\Programs\vcpkg\scripts\buildsystems\vcpkg.cmake -S .
}

if ($FullChainRun -Or $Build -Or !$Generate -And !$Build -And !$Run -And !$Test) {
    Write-Host "Building the project..."
    & $cmakePath --build $BuildDir --config Debug --parallel 24
}

if ($Test) {
    Write-Host "Building and running tests..."
    & $cmakePath --build $BuildDir --target UnitTests --config Debug
    & "$BuildDir\UnitTests.exe"
}

if ($FullChainRun -Or $Run -And !$Test) {
    Write-Host "Running the application..."
    Start-Process -FilePath ".\$BuildDir\CussingLangImpl.exe"
}