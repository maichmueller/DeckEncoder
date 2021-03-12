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

If you want to reference the encoder in CMake, its target name is `deck_encoder`.


## Usage

The deck encoder is namespaced and accessible through `DeckCodec::`. One can pass any `std::vector` like container into `encode`, as long as it provides the following interface elements:
```
begin()
end()
size()
value_type trait of its contained type
```
The contained value must have a public function `code()` returning a type convertible to std::string, and `count()` returning a `size_t` count of the card. The library provides a class `CardToken`, which can be used instead.

To encode and decode (with the aforementioned `CardToken` class used - non deducible!) one calls
```c++
std::vector<CardToken> deck_from_code = DeckCodec::decode<CardToken>(code);
std::string deck_code = DeckCodec::encode(deck_container);
```
