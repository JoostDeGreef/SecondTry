#!/bin/bash

set -xe

mkdir -p build
CC=gcc-10 CXX=g++-10 cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles"
make -C build

