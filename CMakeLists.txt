cmake_minimum_required(VERSION 3.15)
project(LoRDeckEcoder)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CONFIG_FOLDER "cmake")

# Set this path if the conan install is not in the standard directory
# This doesn't work due to a conan internal error when conan is not found in the standard directory (/usr/bin on linux)
# It doesn't work in general when CONAN_COMMAND is set it seems.
#execute_process(COMMAND which conan OUTPUT_VARIABLE CONAN_PATH)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)
# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# enable cache system
include(${CMAKE_CONFIG_FOLDER}/Cache.cmake)

# standard compiler warnings
include(${CMAKE_CONFIG_FOLDER}/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(${CMAKE_CONFIG_FOLDER}/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(${CMAKE_CONFIG_FOLDER}/Doxygen.cmake)
option(ENABLE_DOXYGEN "Enable doxygen doc builds of source")
enable_doxygen(ENABLE_DOXYGEN)

# allow for static analysis options
include(${CMAKE_CONFIG_FOLDER}/StaticAnalyzers.cmake)

# Very basic PCH example
option(ENABLE_PCH "Enable Precompiled Headers" ON)
if (ENABLE_PCH)
    # This sets a global PCH parameter, each project will build its own PCH, which
    # is a good idea if any #define's change
    #
    # consider breaking this out per project as necessary
    target_precompile_headers(project_options
            INTERFACE <vector> <string> <map> <utility> <memory> <array> <list> <stack>)
endif()


set(DECK_CODES_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deck_encoder/src")
set(DECK_CODES_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/deck_encoder/include)
add_library(deck_encoder STATIC)
add_subdirectory(deck_encoder)


option(ENABLE_TESTING "Enable Test Builds" ON)
if(ENABLE_TESTING)
include(cmake/Conan.cmake)
run_conan()
include(${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)

enable_testing()
add_subdirectory(test)
endif()

