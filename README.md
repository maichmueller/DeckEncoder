# DeckEncoder

[![Build and Test](https://github.com/maichmueller/DeckEncoder/actions/workflows/gtest.yml/badge.svg)](https://github.com/maichmueller/DeckEncoder/actions/workflows/gtest.yml)

Port of [LoRDeckCodes](https://github.com/RiotGames/LoRDeckCodes) from C# to C++17. Not many (if any) optimizations have been made to the code and thus is kept true to the source mainly.

## Dependencies

Only requires [GoogleTest](https://github.com/google/googletest) to run the tests, which is installed via the [Conan package manager](https://conan.io/). If one doesn't want to test the compilation, they need to call cmake with the command

```
-DENABLE_TESTING=OFF
```
which will also forego requiring Conan.

## Build

DeckEncoder is configured as a static library to be included in your project. To build independently

```
cd MyProjectFolder
cmake -E make_directory ./build
cd build
cmake --build . --config $BUILD_TYPE
```
