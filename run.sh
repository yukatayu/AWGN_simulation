#!/bin/bash
set -e

clear

echo "---- compiling ----"
clang++ -fsanitize=undefined -g -std=c++17 -O2 ./QPSK.cpp

rm -rf ./outputs
mkdir -p outputs
pushd outputs

echo "---- running ----"
../a.out

echo "---- plotting ----"
gnuplot plotAll.plot

popd

