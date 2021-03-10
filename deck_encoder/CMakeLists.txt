cmake_minimum_required(VERSION 3.15)


set(LIBRARY_SOURCES
        ${DECK_CODES_SRC_DIR}/base32.cpp
        ${DECK_CODES_SRC_DIR}/codec.cpp
        ${DECK_CODES_SRC_DIR}/string_utils.cpp
        ${DECK_CODES_SRC_DIR}/varint.cpp
        )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

target_sources(deck_encoder PRIVATE ${LIBRARY_SOURCES})

target_include_directories(deck_encoder
        PRIVATE
        ${DECK_CODES_INCLUDE_DIR}
        )
target_include_directories(deck_encoder
        SYSTEM INTERFACE
        ${DECK_CODES_INCLUDE_DIR}
        )

set_target_properties(deck_encoder PROPERTIES
        CXX_STANDARD 17
        )

#target_compile_features(aze PRIVATE cxx_std_17)

# stdc++fs is needed for static linking against the std::filesystem library (not needed for GCC >=9)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9")
    target_link_libraries(deck_encoder PUBLIC stdc++fs)
endif()

target_link_libraries(deck_encoder PUBLIC project_options)