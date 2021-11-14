#!/bin/bash

mkdir -p build
CC=gcc-10 CXX=g++-10 cmake -S . -B build
make -C build
 