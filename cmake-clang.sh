#! /bin/sh

cmake -DCMAKE_USER_MAKE_RULES_OVERRIDE=./ClangOverrides.txt -D_CMAKE_TOOLCHAIN_PREFIX=llvm- .
