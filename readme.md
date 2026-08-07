# LVC - Literally Control Version
Work in progress; documentation not accessible until finished

## Compilation

All lvc api settings are at the top of lvc.hpp file; these are both for the api and applications using it as a dependency.

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