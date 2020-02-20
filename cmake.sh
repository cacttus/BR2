rm -rf build
mkdir "build"
cd build
# Force a unix makefile on windows.  This uses CYGWIN
# Install CYGWIN with GCC, Make and CMAKE if on Windows.
cmake .. -G"Unix Makefiles" -DCMAKE_CONFIGURATION_TYPES="Debug;Release"
make