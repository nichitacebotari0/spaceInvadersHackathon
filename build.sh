#!/bin/bash

if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Detect Homebrew location
if [ -d "/opt/homebrew" ]; then
    BREW_PREFIX="/opt/homebrew"  # Apple Silicon
else
    BREW_PREFIX="/usr/local"      # Intel
fi

clang++ ../game.cpp \
    -std=c++20 \
    -I${BREW_PREFIX}/include \
    -L${BREW_PREFIX}/lib \
    -lSDL3 \
    -o game

if [ $? -eq 0 ]; then
    echo "Build successful!"
    ./game
else
    echo "Build failed!"
fi

cd ..