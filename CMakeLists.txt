# CMakeLists.txt
# MetalMario971
# 2/17/2020
#
#  To run CMAKE run 
#     ./cmake.sh 
#   bash script. (in Windows run it in Git Bash or the like)
# 
# Run without .sh file: 
#  CMAKE puts everything in the directory that it was run from.  To run CMAKE correctly use the following syntax:
#       CD to /build folder
#       run cmake ..
# This puts everything in the correct folder.
#   Note: The CMAKE_CURRENT_SOURCE_DIR is where CMakeLists.TXT is, not where CMAKE is run from (so it's in the /BR2/ root directory)
#     https://stackoverflow.com/questions/27247123/how-do-i-clean-up-the-project-files-generated-by-cmake/27254365
#     https://stackoverflow.com/questions/9680420/looking-for-a-cmake-clean-command-to-clear-up-cmake-output
#     https://stackoverflow.com/questions/6921695/how-can-i-build-a-c-project-with-multiple-interdependent-subdirectories
#
# Note: Currently we have some issues
# - [ ] This is currently a test CMakeLists to build the test project.  Switch this to /br2/.
# - [ ] The output folder isn't /bin/ for some reason.  Fix this.
#

#Force Unix makefile
#We want to build with GCC on windows, using CYGWIN
#Install CYGWIN if on Windows.
#set (CMAKE_GENERATOR "Unix Makefiles" CACHE INTERNAL "" FORCE)
message("Generator is set to ${CMAKE_GENERATOR}")

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Remove ZERO_CHECK
#set(CMAKE_SUPPRESS_REGENERATION true)
#Put predefined targets in their own folder.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(BR2_PROJECT_NAME BR2) #Solution name.
set(BR2_BUILD_ARCH x64) 
set(BR2_PROJECT_VERSION 0.01)
set(BR2_GLOBAL_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
#Specifying the same debug/release directories since /data/ exists at the top level.
set(BR2_GLOBAL_BIN_DEBUG_DIR ${BR2_GLOBAL_BIN_DIR})
set(BR2_GLOBAL_BIN_RELEASE_DIR ${BR2_GLOBAL_BIN_DIR})
set(BR2_GLOBAL_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/)
set(BR2_GLOBAL_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/)

#debug vs release
#https://stackoverflow.com/questions/8335499/cmake-visual-studio-debug-release-config
#https://stackoverflow.com/questions/24460486/cmake-build-type-is-not-being-used-in-cmakelists-txt
#We must have separate d/r builds, for profiling, and in the same folder due to /data/.
#set(BR2_CONFIG_SHORT d) #$<$<CONFIG:Debug>:d, $<CONFIG:Release>:r>)
#On VS systems this doesn't appear to work (intuitively)
#SET(CMAKE_BUILD_TYPE Debug CACHE) 
if(${GENERATOR_IS_MULTI_CONFIG)
	#Per CMAKE doc.
	# "Multi-config generators use CMAKE_CONFIGURATION_TYPES as the set of configurations and ignore CMAKE_BUILD_TYPE."

else()
	message("BUILD TYPE " ${CMAKE_BUILD_TYPE})
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(BR2_CONFIG_SHORT "d")
	else ()
		set(BR2_CONFIG_SHORT "r")
	endif()
	message("BUILD TYPE " ${BR2_CONFIG_SHORT})
endif()
project(${BR2_PROJECT_NAME} VERSION ${BR2_PROJECT_VERSION})

set(CMAKE_CACHEFILE_DIR ${BR2_BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BR2_BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BR2_BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BR2_BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BR2_BIN_DIR}) #DLL, not .lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BR2_LIB_DIR}) 

###############################################################################################
#BR2 Target
set(BR2_TARGET_NAME_BR2 ${BR2_PROJECT_NAME})
set(BR2_SRC_DIR_BR2 ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake_test_src/)

file(GLOB_RECURSE BR2_SOURCES_BR2 ${BR2_SRC_DIR_BR2}*.cpp)
file(GLOB_RECURSE BR2_HEADERS_BR2 ${BR2_SRC_DIR_BR2}*.h)
set (BR2_INCLUDE_DIRS "")
foreach (_headerFile ${BR2_HEADERS_BR2})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND BR2_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES BR2_INCLUDE_DIRS)

#TEST -- Header directory for temp source file
list (APPEND BR2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake_test_src/)
#TEST

message("Info ")
message("  + Found Sources: " ${BR2_SOURCES_BR2})
message("  + Found Headers: " ${BR2_HEADERS_BR2})
message("  + Include Directories: " ${BR2_INCLUDE_DIRS})

set(BR2_TARGET_NAME_BR2 ${BR2_PROJECT_NAME})
set(BR2_EXE_NAME_BR2 ${BR2_TARGET_NAME_BR2}_${BR2_CONFIG_SHORT}${BR2_BUILD_ARCH})

add_executable(${BR2_TARGET_NAME_BR2} ${BR2_SOURCES_BR2})
#https://stackoverflow.com/questions/24395517/cmake-executable-name
set_target_properties(${BR2_TARGET_NAME_BR2} PROPERTIES OUTPUT_NAME ${BR2_EXE_NAME_BR2})
target_include_directories(${BR2_TARGET_NAME_BR2} PUBLIC ${BR2_HEADERS_BR2})
set_target_properties(${BR2_TARGET_NAME_BR2} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BR2_GLOBAL_BIN_DEBUG_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BR2_GLOBAL_BIN_RELEASE_DIR}
    #RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BR2_GLOBAL_BIN_DEBUG_DIR}
    #RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BR2_GLOBAL_BIN_DEBUG_DIR}
)

#target_link_libraries(${BR2_TARGET_NAME_BR2} PUBLIC TestLibrary)
###############################################################################################
#Compile Options

function(add_compile_options_config CONFIG)
	#This function adds compile options for a TARGET to debug/release.
	#https://stackoverflow.com/questions/48895687/cmake-add-compile-options-error-with-release-debug-config
    foreach(opt ${ARGN})
        add_compile_options("$<$<CONFIG:${CONFIG}>:${opt}>")
    endforeach()
endfunction()

if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	message("Setting GCC/CLANG options.")
	
	#Static c linkage
	set(BR2_C_GCC_STATIC_C -static-libgcc)
	set(BR2_C_GCC_STATIC_CPP -static-libstdc++)
	
	#These will add just to the target.
	#add_compile_options_config(RELEASE "-W" "-Wall" "-O3" "-pedantic" ${BR2_C_GCC_STATIC_C} ${BR2_C_GCC_STATIC_CPP})
	#add_compile_options_config(DEBUG "-W" "-Wall" "-O0" "-g" "-pedantic" ${BR2_C_GCC_STATIC_C} ${BR2_C_GCC_STATIC_CPP})

	#This adds to ALL targets 
	string(APPEND CMAKE_CXX_FLAGS_RELEASE " -W -Wall -03 -pedantic" ${BR2_C_GCC_STATIC_C} ${BR2_C_GCC_STATIC_CPP})
	
endif()

###############################################################################################
#Set Default Target for Visual Studio
#https://stackoverflow.com/questions/7304625/how-do-i-change-the-startup-project-of-a-visual-studio-solution-via-cmake
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${BR2_TARGET_NAME_BR2})




















