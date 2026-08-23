# LVC - Literally Control Version

A new version control system, build from the ground up for native support of local, distributed and centralized usage.

Work in progress; documentation not accessible as of current.

## Compilation

All external dependencies are handled in cmake with FetchContent. All lvc configuration is accessible in lvc.hpp within LVCAPI folder.

### APP COMPILATION 

```bash
cmake -S app -B app/build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++
```
```bash
cmake --build app/build --parallel
```

### API COMPILATION

```bash 
cmake -S api -B api/build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=clang++
```
```bash
cmake --build api/build --parallel
```

### debugging 

```bash
gdb --args ./app/bin/lvc <arguments>
```