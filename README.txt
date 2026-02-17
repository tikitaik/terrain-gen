Currently a work in progress terrain generator using Perlin noise at the moment

Uses CMake as the build system, and requires C++17 and glfw library (may add to project soon though).

to run (in top level directory):
make (if using Mac/Linux)

or:
cmake -S . -B build (-G "MinGW Makefiles" if using MinGW-64, not sure what generator is for MSVC)
cmake --build build
./build/terrain-gen
