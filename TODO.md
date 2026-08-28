# TODO

Evidence-based backlog for CussinLang. Items reflect the state of the
repository as of this preparation pass.

## P0 — Publication / reproducibility blockers

- **[BUILD-01] OPEN — Untracked source files required by `CMakeLists.txt`.**
  `CMakeLists.txt`'s `SOURCES` list references `src/utils/memethods.h`,
  `src/utils/memethods.cpp`, `src/lang/ast/StringExpressionAST.cpp`,
  `src/lang/ast/headers/StringExpressionAST.h`, and
  `src/utils/DebugInfoGenerator.h`. None of these are currently tracked by git
  (`memethods.*` exist only at the repository root, not `src/utils/`; the
  other two are untracked working-tree files). A fresh clone cannot
  configure/build. Acceptance criteria: `git add` the files in their expected
  paths (moving `memethods.*` into `src/utils/`) and re-verify a clean
  `cmake -B <dir> -G Ninja ...` configure from a fresh clone.

## P1 — High-impact build / maintenance

- **[BUILD-02] OPEN — Hardcoded, machine-specific toolchain paths.**
  `CMakeLists.txt` hardcodes `include(B:/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake)`,
  and `NinjaBuild.ps1`/`NinjaBuild.bat` hardcode local Visual Studio and
  vcpkg paths (e.g. `C:\Program Files\Microsoft Visual Studio\2022\Enterprise\...`).
  These prevent configuring/building on another machine without manual edits.
  Acceptance criteria: accept the vcpkg toolchain path via a CMake cache
  variable/environment variable (e.g. `CMAKE_TOOLCHAIN_FILE` supplied on the
  command line) instead of a hardcoded `include()`, and parameterize the
  Ninja/VS paths in the build scripts.

- **[BUILD-03] OPEN — Compiler-detection failure not resolved.**
  A fresh CMake configure in the current environment (using the existing
  hardcoded vcpkg path, which does resolve on this machine) fails during
  compiler detection: the default toolset resolves to Clang, and CMake
  reports `MSVC_DEBUG_INFORMATION_FORMAT value 'ProgramDatabase' not known
  for this C compiler`, because `CMakePresets.json` expects `cl.exe` but no
  preset/generator selection forces it outside of Visual Studio's IDE
  integration. Build success (configure and compile) is unverified.
  Acceptance criteria: document or script explicit MSVC toolset selection
  (e.g. via a Developer Command Prompt or explicit `CMAKE_C_COMPILER`/
  `CMAKE_CXX_COMPILER=cl.exe` with matching environment) and confirm a
  successful `cmake --build` from a clean directory.

## P2 — Material engineering improvements

- **[TEST-01] OPEN — Automated tests are not wired into the build.**
  `enable_testing()` is commented out in `CMakeLists.txt`, and
  `src/test/CussinLangImplTest.cpp` is not part of the `SOURCES`/target list,
  so it is not compiled or run by any documented command. Acceptance
  criteria: add the test file to a test target (or a separate executable),
  enable `enable_testing()`, and document the test-run command in
  `README.md` with its verification status.

- **[FEAT-01] OPEN — JIT execution path is unexercised.**
  `src/jit/CussinJIT.h` provides LLVM ORC-based JIT support, but its
  invocation is commented out in `src/CussingLangImpl.cpp`, so the JIT path
  is not exercised by the current sample entry point. Acceptance criteria:
  either re-enable and verify JIT execution in the sample driver, or
  explicitly document it as experimental/disabled in `README.md` (already
  partially reflected there).

## P3 — Optional cleanup

- **[ORG-01] OPEN — Obsidian vault tracked at repository root.**
  `CussinLang/.obsidian/` and `CussinLang/Cussin Lang/` (design notes) are
  tracked in git at the repository root under a directory also named
  `CussinLang/`. The `.obsidian/` subfolder is editor/workspace state (not
  reviewed for accuracy here) rather than source or documentation; the
  design notes themselves are content, not code. Human decision: keep as
  project documentation (in which case consider excluding
  `CussinLang/.obsidian/` via `.gitignore`), or relocate/exclude if not
  intended for publication.

## Human decisions required

- **[LICENSE] NEEDS DECISION.** No license file is present. `README.md`
  states this neutrally. Selecting a license is a human decision and is out
  of scope for this preparation pass.
- **[ORG-01]** Whether the tracked Obsidian notes vault (`CussinLang/`) should
  remain published as-is, have its editor-state folder ignored, or be
  relocated.
