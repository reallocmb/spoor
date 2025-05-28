#!/bin/bash
#set -x
cxx="gcc"
c_flags="-g -Wall -Wextra -pedantic -pedantic-errors"
c_flags_release="-O3 -DRELEASE"
extern_lib="-lX11 `pkg-config --libs --cflags freetype2`"
output="./bin/spoor"
output_test="./tests/bin/test"

src_compile() {
    $cxx $c_flags -Iinc -Llib src/spoor_main.c -o $output $extern_lib
}

case $1 in
    "run")
        src_compile
        $output
        ;;
    "ddd")
        ddd $output
        ;;
    "test")
        $cxx $c_flags tests/test.c tests/eenheid/e_test.c -o $output_test $extern_lib 
        $output_test
        ;;
    "install")
        c_flags=$c_flags_release
        src_compile
        sudo cp $output /usr/bin
        ;;
    *)
        src_compile
        ;;
esac
