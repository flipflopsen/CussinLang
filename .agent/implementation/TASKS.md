# CussinLang Implementation Tasks

## Priority P0 - Publication / Reproducibility Blockers

### [BUILD-01] OPEN — Untracked source files required by `CMakeLists.txt`
- **Status**: Not Started
- **Description**: `CMakeLists.txt`'s `SOURCES` list references `src/utils/memethods.h`, `src/utils/memethods.cpp`, `src/lang/ast/StringExpressionAST.cpp`, `src/lang/ast/headers/StringExpressionAST.h`, and `src/utils/DebugInfoGenerator.h`. These are not currently tracked by git.
- **Acceptance Criteria**: Add the files in their expected paths (moving `memethods.*` into `src/utils/`) and re-verify a clean `cmake -B <dir> -G Ninja ...` configure from a fresh clone.

### [BUILD-02] OPEN — Hardcoded, machine-specific toolchain paths
- **Status**: Not Started
- **Description**: `CMakeLists.txt` hardcodes `include(B:/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake)`, and `NinjaBuild.ps1`/`NinjaBuild.bat` hardcode local Visual Studio and vcpkg paths.
- **Acceptance Criteria**: Accept the vcpkg toolchain path via a CMake cache variable/environment variable instead of hardcoded `include()`, and parameterize the Ninja/VS paths in the build scripts.

### [BUILD-03] OPEN — Compiler-detection failure not resolved
- **Status**: Not Started
- **Description**: A fresh CMake configure in the current environment fails during compiler detection due to Clang/MSVC toolset mismatch.
- **Acceptance Criteria**: Document or script explicit MSVC toolset selection and confirm a successful `cmake --build` from a clean directory.

## Priority P1 - High-impact Build / Maintenance

### [TEST-01] OPEN — Automated tests are not wired into the build
- **Status**: Not Started
- **Description**: `enable_testing()` is commented out in `CMakeLists.txt`, and `src/test/CussinLangImplTest.cpp` is not part of the `SOURCES`/target list.
- **Acceptance Criteria**: Add the test file to a test target, enable `enable_testing()`, and document the test-run command in `README.md`.

### [FEAT-01] OPEN — JIT execution path is unexercised
- **Status**: Not Started
- **Description**: `src/jit/CussinJIT.h` provides LLVM ORC-based JIT support, but its invocation is commented out in `src/CussingLangImpl.cpp`.
- **Acceptance Criteria**: Either re-enable and verify JIT execution in the sample driver, or explicitly document it as experimental/disabled in `README.md`.

## Priority P2 - Material Engineering Improvements

### [ORG-01] OPEN — Obsidian vault tracked at repository root
- **Status**: Not Started
- **Description**: `CussinLang/.obsidian/` and `CussinLang/Cussin Lang/` (design notes) are tracked in git at the repository root.
- **Acceptance Criteria**: Decide whether to keep as project documentation, ignore editor-state folder, or relocate.

## Priority P3 - Optional Cleanup

### [LICENSE] NEEDS DECISION
- **Status**: Not Started
- **Description**: No license file is present in the repository.
- **Acceptance Criteria**: Select an appropriate license for publication.

## Human Decisions Required

### [LICENSE] NEEDS DECISION
- **Status**: Not Started
- **Description**: No license file is present. `README.md` states this neutrally.
- **Acceptance Criteria**: Selecting a license is a human decision.

### [ORG-01] Human Decision Required
- **Status**: Not Started
- **Description**: Whether the tracked Obsidian notes vault should remain published as-is, have its editor-state folder ignored, or be relocated.
- **Acceptance Criteria**: Human decision on the Obsidian vault inclusion.