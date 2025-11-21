@echo off
if not exist build (
	mkdir build
)

pushd build

clang++ ../game.cpp -IC:\Tools\SDL3-3.2.26\include -LC:\Tools\SDL3-3.2.26\lib\x64 -lSDL3 -g -gcodeview -o game.exe 



popd