
set(TEST_SOURCES
        main_test.cpp
        test_codec.cpp
        test_base32.cpp
        )

add_executable(tests ${TEST_SOURCES})

target_include_directories(tests
        PRIVATE
        ${DECK_CODES_INCLUDE_DIR}
        ${GTEST_INCLUDE_DIRS}
        )

set_target_properties(tests PROPERTIES
        CXX_STANDARD 17
        )
target_link_libraries(tests PRIVATE CONAN_PKG::gtest deck_encoder)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_cases.txt
        ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

message(${CMAKE_CURRENT_BINARY_DIR})
