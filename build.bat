pushd bin
cl /Zi /I../inc ../src/spoor_main.c User32.lib Gdi32.lib
popd
