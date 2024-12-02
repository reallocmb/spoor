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
#define VIEW_FLAG_HORIZONTAL (1 << 1)
#define VIEW_FLAG_VERTICAL (1 << 2)

#define VIEW_FLAG_CHILD (VIEW_FLAG_HORIZONTAL | VIEW_FLAG_VERTICAL)

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
    void (*input_func)(struct View *view, u8 key);
    void *data;
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
    .width = 1200,
    .height = 800,
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

enum {
    SPOOR_STATUS_NOT_STARTED,
    SPOOR_STATUS_IN_PROGRESS,
    SPOOR_STATUS_COMPLETED,
};

enum {
    SPOOR_TYPE_TASK,
    SPOOR_TYPE_PROJECT,
    SPOOR_TYPE_EVENT,
    SPOOR_TYPE_APPOINTMENT,
    SPOOR_TYPE_GOAL,
    SPOOR_TYPE_HABIT,
    SPOOR_TYPE_IDEA,
};

const char SPOOR_STATUS[3][25] = {
    "NOT STARTED",
    "IN PROGRESS",
    "COMPLETED"
};

const char SPOOR_TYPES[7][23] = {
    "TASK",
    "PROJECT",
    "EVENT",
    "APPOINTMENT",
    "GOAL",
    "HABIT",
    "IDEA"
};

#define SPOOR_TIME_DEFAULT_VALUE 0xffffffff

#define SPOOR_OBJECT_ID_DELETED 0xffffffff

typedef struct SpoorTime {
    s32 sec;
    s32 min;
    s32 hour;
    s32 day;
    s32 mon;
    s32 year;
} SpoorTime;

typedef struct SpoorTimeSpan {
    SpoorTime start;
    SpoorTime end;
} SpoorTimeSpan; // 48 bytes

typedef struct SOLink {
    u32 id;
    u32 link_ids[120];
    u32 link_ids_count;
    u32 id_prev;
    u32 id_next;
} SOLink;

typedef struct SOTT {
    u32 id; // 4 bytes
    SpoorTimeSpan time_spans[10]; // 10 * 48 bytes = 480
    u32 time_spans_count; // 4 bytes
    u32 id_prev; // 4 bytes
    u32 id_next; // 4 bytes
} SOTT; // 496 bytes

#define SPOOR_FILTER_STATUS_NOT_STARTED 0x1
#define SPOOR_FILTER_STATUS_IN_PROGRESS (0x1 << 1)
#define SPOOR_FILTER_STATUS_COMPLETED (0x1 << 2)
#define SPOOR_FILTER_STATUS_ALL 0b111

const u3, SPOOR_FILTER_STATUS_BITS[3] = {
    SPOOR_FILTER_STATUS_NOT_STARTED,
    SPOOR_FILTER_STATUS_IN_PROGRESS,
    SPOOR_FILTER_STATUS_COMPLETED,
};

#define SPOOR_FILTER_TYPE_TASK 0x1
#define SPOOR_FILTER_TYPE_PROJECT (0x1 << 1)
#define SPOOR_FILTER_TYPE_EVENT (0x1 << 2)
#define SPOOR_FILTER_TYPE_APPOINTMENT (0x1 << 3)
#define SPOOR_FILTER_TYPE_GOAL (0x1 << 4)
#define SPOOR_FILTER_TYPE_HABIT (0x1 << 5)
#define SPOOR_FILTER_TYPE_IDEA (0x1 << 6)
#define SPOOR_FILTER_TYPE_ALL 0b1111111

const u32 SPOOR_FLITER_TYPES_BITS[7] = {
    SPOOR_FILTER_TYPE_TASK,
    SPOOR_FILTER_TYPE_PROJECT,
    SPOOR_FILTER_TYPE_EVENT,
    SPOOR_FILTER_TYPE_APPOINTMENT,
    SPOOR_FILTER_TYPE_GOAL,
    SPOOR_FILTER_TYPE_HABIT,
    SPOOR_FILTER_TYPE_IDEA,
};

typedef struct SpoorFilter {
    u32 type;
    u32 status;
} SpoorFilter;
        
#define SPOOR_OBJECT_TITLE_SIZE_MAX 240
#define SPOOR_OBJECT_NO_PARENT_ID 0xffffffff
#define SPOOR_OBJECT_NO_ID 0xffffffff
#define SPOOR_OBJECT_FREE_ID 0xfffffffe

typedef struct SpoorObject {
    u32 id; // 4 bytes
    u32 id_parent; // 4 bytes
    char title[SPOOR_OBJECT_TITLE_SIZE_MAX]; //  250
    SpoorTimeSpan deadline; // 48 bytes
    SpoorTimeSpan schedule; // 48 bytes
    u32 schedule_id_next; // 4 bytes
    SpoorTimeSpan tracked; // 48 bytes
    SpoorTimeSpan complete; // 48 bytes
    SpoorTimeSpan created; // 48 bytes
    u8 status; // 1 byte
    u8 type; // 1 byte
} SpoorObject; // 500 bytes

#define TASK_LIST_HAND_INDEX_DEFAULT 0xffffffff

typedef struct Data {
    SpoorObject *spoor_objects;
    u32 index;
} Data;

typedef struct TaskListData {
    SpoorObject *spoor_objects;
    u32 hand_index;
    SpoorFilter spoor_filter;
} TaskListData;

#define DAY_SECONDS 86400

typedef struct CalendarData {
    SpoorObject *spoor_objects;
    u32 index;
    u32 days_count;
    u32 time_offset;
    s32 date_offset;
} CalendarData;

#endif
