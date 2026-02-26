Currently a work in progress terrain generator using Perlin noise at the moment

Uses CMake as the build system, and requires C++17 (glfw is included but this can be replaced by a self-compiled version if needed).

to run (in top level directory):
if using Mac/Linux/Windows with MinGW-64: make

otherwise
cmake -S . -B build (-G "MinGW Makefiles" if using MinGW-64, not sure what generator is for MSVC)
cmake --build build
./build/terrain-gen
