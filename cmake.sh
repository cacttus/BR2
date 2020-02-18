rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_CONFIGURATION_TYPES="Debug;Release"
make