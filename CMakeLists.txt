cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BR2_PROJECT_NAME "BR2")
set(BR2_PROJECT_VERSION 0.01)
set(BR2_BIN_DIR "./cmake_test_bin/")
set(BR2_SRC_DIR "./src/cmake_test_src/")
set(BR2_BUILD_DIR "./build/")

message("Building" ${BR2_PROJECT_NAME})
project(${BR2_PROJECT_NAME} VERSION ${BR2_PROJECT_VERSION})

#Clean build.
file(REMOVE_RECURSE ${BR2_BUILD_DIR})

#Create /bin if not present.
set(CMAKE_CACHEFILE_DIR ${BR2_BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BR2_BIN_DIR})
file(MAKE_DIRECTORY ${BR2_BIN_DIR})

#add_subdirectory(${BR2_SRC_DIR})
#add_library(TestLibrary ./test_src/test_lib.cxx)

#https://stackoverflow.com/questions/6921695/how-can-i-build-a-c-project-with-multiple-interdependent-subdirectories
file(GLOB_RECURSE BR2_SOURCES ${BR2_SRC_DIR}"/*.cpp")
file(GLOB_RECURSE BR2_HEADERS ${BR2_SRC_DIR}"/*.h")
set (BR2_INCLUDE_DIRS "")
foreach (_headerFile ${BR2_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND BR2_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES BR2_INCLUDE_DIRS)

# add the executable
add_executable(TestProject_x64 ${BR2_SOURCES})

#target_link_libraries(TestProject_x64 PUBLIC TestLibrary)
target_include_directories(TestProject_x64 PUBLIC ${BR2_HEADERS})

