#ifndef SPOOR_H
#define SPOOR_H

#include"s_config.h"

#ifdef __unix__
#include<X11/Xlib.h>
#endif

#include<ft2build.h>
#include FT_FREETYPE_H

typedef struct Font {
    FT_Library library;
    FT_Face face;
    u16 size;
} Font;

typedef struct Graphic {
    bool running;
    u16 width;
    u16 height;
    u32 *pixels;
    u32 pixels_count;
    void (*render_func)(struct Graphic *graphic);
    void (*input_func)(struct Graphic *graphic);
#ifdef __unix__
    XImage *image;
    Window window;
    Display *display;
#endif
} Graphic;

typedef struct Spoor {
    Graphic graphic;
    Font font;
} Spoor;

#endif
