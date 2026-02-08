SOURCE_DIR = src
BUILD_DIR  = build

all:
	cmake -S . -B build
	cmake --build build
