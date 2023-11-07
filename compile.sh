#! /bin/bash
if [ ! -d "bin" ]; then
    mkdir bin
fi
g++ -g -o bin/main main.cpp Restaurant.cpp -I . -std=c++17 $1
