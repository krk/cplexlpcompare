#! /bin/bash

mkdir -p build
cd build

cmake -DCMAKE_USER_MAKE_RULES_OVERRIDE=./build-clang/ClangOverrides.txt ../..
