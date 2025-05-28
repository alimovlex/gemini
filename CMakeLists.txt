cmake_minimum_required(VERSION 3.13)  # Automatically generated syntax

# project(gemini) --> When CLion project is created, it is automatically created if it is created as c project,
# Because of this, the C++ gtest is not built.
# Therefore, remove c as below.
project(gemini)

set(CMAKE_C_STANDARD 17) # Automatic generation syntax when creating a C project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# Build an executable program using the plus library (C code)
# It is not used when performing only tests, so there is no need to build.
# C++ includes
include_directories(${PROJECT_SOURCE_DIR}/inc)
file(GLOB SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp" "main.cpp")
add_executable(gemini ${SRCS})
target_link_libraries(gemini curl jsoncpp pthread)

