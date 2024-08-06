pushd bin
cl /Zi /I../inc ../src/spoor_main.c /link /LIBPATH:../lib freetype.lib User32.lib Gdi32.lib
popd
