cmake_minimum_required(VERSION 3.10)

# set the project name
project(TestProject VERSION 0.01)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(./test_src/)
add_library(TestLibrary ./test_src/test_lib.cxx)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./test_bin")

# add the executable
add_executable(TestProject_x64 ./test_src/TestProject.cxx)

target_link_libraries(TestProject_x64 PUBLIC TestLibrary)
target_include_directories(TestProject_x64 PUBLIC
                          "$./test_bin/"
                          "./test_src/"
                          )

