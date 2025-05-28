@echo off
pushd bin
rm spoor_main.exe
cl /TC /Zi /I../inc ../src/spoor_main.c /link /LIBPATH:../lib freetype.lib User32.lib Gdi32.lib Dwmapi.lib /SUBSYSTEM:WINDOWS
popd
