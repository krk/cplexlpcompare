#! /bin/bash

cmake -DCMAKE_USER_MAKE_RULES_OVERRIDE=./ClangOverrides.txt -DCMAKE_TOOLCHAIN_PREFIX=llvm- .
