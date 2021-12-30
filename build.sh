#!/bin/bash

set -xe

mkdir -p build
CC=gcc CXX=g++ cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles"
make -C build

