$LLVMSource = "B:/Sources/llvm-project/llvm"
$LLVMBuildRoot = "B:/Build/llvm"
$LLVMInstallRoot = "B:/Libraries"
$Jobs = 8

function Build-LLVM {
    param(
        [string]$Variant,
        [string]$Assertions
    )

    $BuildDir = "$LLVMBuildRoot/$Variant"
    $InstallDir = "$LLVMInstallRoot/$Variant"

    $ConfigureArgs = @(
        "-S", $LLVMSource,
        "-B", $BuildDir,
        "-G", "Ninja",

        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_INSTALL_PREFIX=$InstallDir",

        "-DCMAKE_C_COMPILER=clang-cl",
        "-DCMAKE_CXX_COMPILER=clang-cl",
        "-DCMAKE_C_COMPILER_LAUNCHER=sccache",
        "-DCMAKE_CXX_COMPILER_LAUNCHER=sccache",

        "-DLLVM_USE_LINKER=lld",
        "-DLLVM_USE_CRT_RELEASE=MD",

        "-DLLVM_ENABLE_PROJECTS=",
        "-DLLVM_ENABLE_RUNTIMES=",
        "-DLLVM_TARGETS_TO_BUILD=X86",

        "-DLLVM_ENABLE_ASSERTIONS=$Assertions",
        "-DLLVM_ENABLE_LTO=OFF",

        "-DLLVM_ENABLE_RTTI=ON",
        "-DLLVM_ENABLE_EH=ON",

        "-DLLVM_BUILD_TOOLS=OFF",
        "-DLLVM_INCLUDE_TESTS=OFF",
        "-DLLVM_INCLUDE_EXAMPLES=OFF",
        "-DLLVM_INCLUDE_BENCHMARKS=OFF",

        "-DLLVM_PARALLEL_LINK_JOBS=1"
    )

    & cmake @ConfigureArgs
    if ($LASTEXITCODE -ne 0) {
        throw "LLVM configuration failed: $Variant"
    }

    & cmake --build $BuildDir --target install --parallel $Jobs
    if ($LASTEXITCODE -ne 0) {
        throw "LLVM build/install failed: $Variant"
    }
}

Build-LLVM -Variant "llvm-noassert" -Assertions "OFF"
Build-LLVM -Variant "llvm-assert" -Assertions "ON"