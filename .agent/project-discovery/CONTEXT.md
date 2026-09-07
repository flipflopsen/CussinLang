# Project Context

## Repository Information

- **Repository Root**: L:\GitRepos\CussinLang
- **Git Revision**: e264748
- **Branch**: main
- **Working Tree**: Clean (only .gitignore modified, CussingLangImpl.sln untracked)
- **Last Commit**: "Merge pull request #1 from flipflopsen/dev" (e264748)

## Analysis Constraints

- Do not modify any project source code, tests, configuration, or documentation
- Only create Markdown files in `.agent/project-discovery/`
- Do not install dependencies or access external services
- Use read-only inspection only
- Run existing tests/builds only if they don't require external dependencies

## Tools Available

- File system operations (read, write, glob)
- Git operations
- Basic shell commands
- Web fetching (for documentation)

## Agent Instructions

This agent is an autonomous project discovery agent examining the CussinLang repository to:
1. Determine what the project does and who uses it
2. Understand how it is built, configured, tested, deployed, and operated
3. Analyze its static and runtime architecture
4. Identify principal components, dependencies, entry points, and data flows
5. Assess code style and engineering practices
6. Identify risks, technical debt, and contradictions

## Key Files and Directories

- `src/` - Source code directory
- `include/` - Header files directory
- `tests/` - Test files directory
- `README.md` - Project documentation
- `Problems.md` - Documentation of issues
- `TODO.md` - Development tasks
- `CussingLangImpl.sln` - Visual Studio solution file
- `.gitignore` - Git ignore rules

## Current Working Directory
L:\GitRepos\CussinLang