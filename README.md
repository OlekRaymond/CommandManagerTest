# Command Manager (Test)

## CMake
CMake is included with all visual studio installations.

To check you have CMake open up a Visual Studio command line:
    - You can find it by pressing the windows key and entering command line in the search box
    - It will look something like: `Visual Studio 2022 Developer Command Prompt`

Then type `cmake --version` and observe a result similar to:
```
cmake version 3.31.6-msvc6

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

## Buidling 
In the same command line:
- navigate to the folder containing this file
- run `cmake -B build -S .` to configure the cmake build
- run `cmake --build build` to compile and link the code
- run `ctest --test-dir build` to run the tests

## Portability
The same code runs on clang and gcc with few warnings so is likely to run on all systems.

