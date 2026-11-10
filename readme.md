# LVC - Literally Control Version

This repository is a playground for testing ideas in an environment where technical debt and vulnerabilities are acceptable.

LVC is a *working* (I'll get there in a week) minimal viable product as well as the basis for my bachelor's thesis. Around march 2027 I will start over again in a new repository with a publicly available design document.

## Brief Overview of Features

### Partial Atomicity

Every file operation is postponed to after the end of the shared library function where the program checks for error before writing pending changes. If anything fails during the program, the state of the repostiory doesn't change. This doesn't protect the repository from failing while writing the pending changes, which is why I call it partial atomicity.

### Path Input Resolving

Files can be selected with wildcards, root-relative paths, exclusion rules and or without spelling out the full path. `lvc revert`, `lvc prepare` and `lvc goto` filters out paths based on `lvc.ignore`, which as of current must be in the repository root. 

### Versioning

Files are prepared using `lvc prepare ...`. Prepare saves paths locally, but not the state of the path at the time of prepare. After that one can create a new snapshot using `lvc version <desc>`. To revert the working directory or selected files to a state of the version, use `lvc revert`.

### Merging

Workspaces can be merged both within the same repository and between local repositories. Versions created in the workspace are differentiated from the versions added as a result of a merge to support a more customizable display of history.

For a comprehensive list of arguments, run `lvc help`

## Compilation

All external dependencies are handled in cmake with FetchContent. All lvc configuration is accessible in lvc.hpp within LVCAPI folder.

### API COMPILATION

```bash 
cmake -S api -B api/build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=clang++
```
```bash
cmake --build api/build --parallel
```

### APP COMPILATION 

```bash
cmake -S app -B app/build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++
```
```bash
cmake --build app/build --parallel
```