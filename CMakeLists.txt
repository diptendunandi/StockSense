cmake_minimum_required(VERSION 3.14)
project(StockSense VERSION 1.0.0 LANGUAGES CXX)

# C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/CSVParser.cpp
    src/DataCleaner.cpp
    src/FeatureEngine.cpp
    src/Matrix.cpp
    src/LinearRegression.cpp
    src/BaselineModel.cpp
    src/Evaluator.cpp
    src/Predictor.cpp
    src/Display.cpp
)

# Main executable
add_executable(stocksense ${SOURCES})

# Output directory
set_target_properties(stocksense PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

# Enable testing
enable_testing()

# Test executable
add_executable(stocksense_tests
    tests/test_main.cpp
    src/CSVParser.cpp
    src/DataCleaner.cpp
    src/FeatureEngine.cpp
    src/Matrix.cpp
    src/LinearRegression.cpp
    src/BaselineModel.cpp
    src/Evaluator.cpp
    src/Predictor.cpp
    src/Display.cpp
)

add_test(NAME StockSenseTests COMMAND stocksense_tests)

# Install target
install(TARGETS stocksense DESTINATION bin)

# Print configuration
message(STATUS "")
message(STATUS "=== StockSense Configuration ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "================================")
message(STATUS "")
