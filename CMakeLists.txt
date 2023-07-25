cmake_minimum_required(VERSION 3.13)  # CMake version check
project(tee_benchmark)
set(CMAKE_CXX_STANDARD 17)

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES main.cpp algo.cpp algo.hpp)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(tee_benchmark ${SOURCE_FILES})

# Use OpenSSL for hashing, encryption, and random number generation
find_package(OpenSSL REQUIRED)
target_link_libraries(tee_benchmark OpenSSL::Crypto)