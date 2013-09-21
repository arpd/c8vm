#!/bin/bash

do_clean () {
    echo -e "[-] cleaning binaries"
    make clean 1>&2 2>/dev/null
    echo -e "[-] cleaning CMake cruft"
    rm -r CMakeFiles CMakeCache.txt cmake_install.cmake Makefile 2>/dev/null
}

do_build() {
    echo -e "[-] starting new CMake build"
    cmake . 1>/dev/null
    echo -e "[-] building"
    make
}

if [[ "$1" == "clean" ]]; then
    do_clean
else
    do_clean
    do_build
fi
