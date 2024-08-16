#ifndef SPOOR_H
#define SPOOR_H

#include"s_config.h"

#ifdef __unix__
#include<X11/Xlib.h>
#elif _WIN32
#include<windows.h>
#endif

#include<ft2build.h>
#include FT_FREETYPE_H

typedef struct SpoorFont {
    FT_Library library;
    FT_Face face;
    u16 size;
    u16 dpi_x;
    u16 dpi_y;
    u16 height;
} SpoorFont;

#define COMMAND_BUFFER_ALLOC 250

typedef struct CommandBuffer {
    wchar_t *buffer;
    u32 buffer_count;
} CommandBuffer;

enum {
    GRAPHIC_MODE_COMMAND_BUFFER,
    GRAPHIC_MODE_NORMAL,
};

typedef struct Graphic {
    bool running;
    u16 width;
    u16 height;
    u32 *pixels;
    u32 pixels_count;
    void (*render_func)(struct Graphic *graphic);
    void (*input_func)(struct Graphic *graphic, u8 key);
    CommandBuffer command_buffer;
#ifdef __unix__
    XImage *image;
    Window window;
    Display *display;
    XIC input_context;
#else _WIN32
    HDC device_context;
#endif
    SpoorFont font;
    u8 mode;
} Graphic;

typedef struct Spoor {
    Graphic graphic;
} Spoor;

#endif
