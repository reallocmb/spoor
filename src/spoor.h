#ifndef SPOOR_H
#define SPOOR_H

#include"s_types.h"
#include"s_config.h"
// #include"s_commands.h" /* todo(mb) */
#include"s_input.h"

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

#define COMMAND_BUFFER_ALLOC_SIZE 250

typedef struct CommandBuffer {
    u8 *buffer;
    u32 count;
} CommandBuffer;

enum {
    GRAPHIC_MODE_NORMAL,
    GRAPHIC_MODE_COMMAND_BUFFER,
};

#define SPOOR_VIEWS_ALLOC_SIZE 50

#define VIEW_FLAG_PARENT 1
#define VIEW_FLAG_VERTICAL (1 << 1)
#define VIEW_FLAG_HORIZONTAL (1 << 2)

#define VIEW_NO_PARENT 0xffff

typedef struct View {
    // char name[50];
#if 1
    u16 flags;
    u16 id;
    u16 x;
    u16 y;
    u16 width;
    u16 height;
    u16 parent_id;
    u16 childs_count;
    void (*render_func)(struct View *view); /* void *graphic => Graphic *graphic */
#endif /* todo(mb) */
} View;

struct Graphic {
    bool running;
    u16 width;
    u16 height;
    u32 *pixels;
    u32 pixels_count;
    CommandBuffer command_buffer;
    SpoorFont font;
    View *views;
    u32 views_count;
    u32 views_index;
    u8 mode;
    void (*render_func)(void);
    void (*input_func)(u8 key);
#ifdef __unix__
    XImage *image;
    Window window;
    Display *display;
    XIC input_context;
#elif _WIN32
    HDC device_context;
#endif
} GlobalGraphic = {
    .running = true,
    .width = 800,
    .height = 600,
    .pixels = NULL,
    .pixels_count = 0,
    .command_buffer = {
        .buffer = NULL,
        .count = 0,
    },
    .font = { 0 },
    .views = NULL,
    .views_count = 0,
    .mode = GRAPHIC_MODE_NORMAL,
    .render_func = NULL,
    .input_func = NULL,
#ifdef __unix__
    .image = NULL,
    .window = 0,
    .display = NULL,
    .input_context = 0,
#elif _WIN32
    .device_context = 0,
#endif
};

#endif
