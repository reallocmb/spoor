#ifndef SPOOR_H
#define SPOOR_H

#include"s_types.h"
#include"s_config2_0.h"
//#include"s_config.h"
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

typedef struct TTFCacheData {
    FT_Bitmap   bitmap;
    FT_Int      bitmap_left;
    FT_Int      bitmap_top;
    FT_Vector   advance;
    bool has_cache;
} TTFCacheData;

typedef struct TTFCacheSize {
    u32 font_size;
    TTFCacheData ttf_cache_data[256];
} TTFCacheSize;

TTFCacheSize global_ttf_cache_sizes[100];
extern u32 global_ttf_cache_sizes_count;

TTFCacheData *global_ttf_cache_data;

#define COMMAND_BUFFER_ALLOC_SIZE 200

typedef struct CommandBuffer {
    u32 count;
    u8 *buffer;
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

#define RENDER_FLAG_ALL 0x1
#define RENDER_FLAG_COMMAND_BUFFER (0x1 << 1)
#define RENDER_FLAG_INDEX_CHANGE (0x1 << 2)

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
    u8 render_flags;
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
    .render_flags = RENDER_FLAG_ALL,
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
    SPOOR_PRIORITY_HIGH,
    SPOOR_PRIORITY_MEDIUM,
    SPOOR_PRIORITY_LOW,
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
    "NS",
    "IP",
    "C"
};

const u32 SPOOR_STATUS_COLORS[3] = {
    0xff551100,
    0xff887722,
    0xff228811,
};

const char SPOOR_TYPES[7][5] = {
    "TASK",
    "PROJ",
    "EVEN",
    "APPO",
    "GOAL",
    "HABI",
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
#define SPOOR_FILTER_STATUS_ALL 0x7

const u32 SPOOR_FILTER_STATUS_BITS[3] = {
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
#define SPOOR_FILTER_TYPE_ALL 0x7f

const u32 SPOOR_FLITER_TYPES_BITS[7] = {
    SPOOR_FILTER_TYPE_TASK,
    SPOOR_FILTER_TYPE_PROJECT,
    SPOOR_FILTER_TYPE_EVENT,
    SPOOR_FILTER_TYPE_APPOINTMENT,
    SPOOR_FILTER_TYPE_GOAL,
    SPOOR_FILTER_TYPE_HABIT,
    SPOOR_FILTER_TYPE_IDEA,
};

#define SPOOR_FILTER_PRIORITY_HIGH 0x1
#define SPOOR_FILTER_PRIORITY_MEDIUM (1 << 1)
#define SPOOR_FILTER_PRIORITY_LOW (1 << 2)
#define SPOOR_FILTER_PRIORITY_ALL 0x3

const u32 SPOOR_FILTER_PRIORITYS_BITS[3] = {
    SPOOR_FILTER_PRIORITY_HIGH,
    SPOOR_FILTER_PRIORITY_MEDIUM,
    SPOOR_FILTER_PRIORITY_LOW,
};

#define SPOOR_FILTER_FLAG_SCHEDULE (1 << 0)
#define SPOOR_FILTER_FLAG_DEADLINE (1 << 1)

#define SPOOR_FILTER_TIME_SPAN_DEFAULT_START \
(SpoorTime){ \
    .sec = SPOOR_TIME_DEFAULT_VALUE, \
    .min = SPOOR_TIME_DEFAULT_VALUE, \
    .hour = SPOOR_TIME_DEFAULT_VALUE, \
    .day = SPOOR_TIME_DEFAULT_VALUE, \
    .mon = SPOOR_TIME_DEFAULT_VALUE, \
    .year = SPOOR_TIME_DEFAULT_VALUE, \
}

#define SPOOR_FILTER_TIME_SPAN_DEFAULT_END \
(SpoorTime){ \
    .sec = INT32_MAX, \
    .min = INT32_MAX, \
    .hour = INT32_MAX, \
    .day = INT32_MAX, \
    .mon = INT32_MAX, \
    .year = INT32_MAX, \
}

typedef struct SpoorFilter {
    u32 type;
    u32 status;
    u32 priority;
    u32 flags;
    SpoorTimeSpan schedule;
} SpoorFilter;

typedef struct SpoorSort {
    u32 nothing;
} SpoorSort;
        
#define SPOOR_OBJECT_TITLE_SIZE_MAX 240
#define SPOOR_OBJECT_NO_PARENT_ID 0xffffffff
#define SPOOR_OBJECT_NO_ID 0xffffffff
#define SPOOR_OBJECT_FREE_ID 0xfffffffe

typedef struct SpoorObject {
    u32 id; // 4 bytes
    u32 id_parent; // 4 bytes
    char title[SPOOR_OBJECT_TITLE_SIZE_MAX]; //  240
    SpoorTimeSpan deadline; // 48 bytes
    SpoorTimeSpan schedule; // 48 bytes
    u32 schedule_id_next; // 4 bytes
    SpoorTimeSpan tracked; // 48 bytes
    SpoorTimeSpan complete; // 48 bytes
    SpoorTimeSpan created; // 48 bytes
    u8 status; // 1 byte
    u8 type; // 1 byte
    u8 priority;
} SpoorObject; // 500 bytes

#define TASK_LIST_HAND_INDEX_DEFAULT 0xffffffff

#define SPOOR_OBJECTS_MAX 500

#define SPOOR_OBJECTS_INDEXES_SYNC_CREATE (1)
#define SPOOR_OBJECTS_INDEXES_SYNC_EDIT (1 << 1)
#define SPOOR_OBJECTS_INDEXES_SYNC_REMOVE (1 << 2)

typedef struct IndexesSync {
    u32 index;
    u32 mode;
} IndexesSync;

typedef struct GlobalData {
    SpoorObject spoor_objects[SPOOR_OBJECTS_MAX];
    u32 spoor_objects_count;
    IndexesSync indexes_sync; 
} GlobalData;

GlobalData global_data = {
    .spoor_objects_count = 0,
};

typedef struct Data {
    u32 spoor_objects_indexes[SPOOR_OBJECTS_MAX];
    u32 spoor_objects_indexes_count;
    u32 index;
    s32 (*sort_func)(const void *data0, const void *data1);
} Data;

typedef struct TaskListData {
    u32 spoor_objects_indexes[SPOOR_OBJECTS_MAX];
    u32 spoor_objects_indexes_count;
    u32 index;
    s32 (*sort_func)(const void *data0, const void *data1);
    SpoorFilter spoor_filter;
    u32 index_last;
    s32 offset_y;
} TaskListData;

#define DAY_SECONDS 86400

const u32 CALENDAR_OBJECT_COLORS[7] = {
    0x95c3a083, // SPOOR_TYPE_TASK,
    0x95d39083,// SPOOR_TYPE_PROJECT,
    0x95d3b063,// SPOOR_TYPE_EVENT,
    0x95d3a073,// SPOOR_TYPE_APPOINTMENT,
    0x95c3b073,// SPOOR_TYPE_GOAL,
    0x95b3b083,// SPOOR_TYPE_HABIT,
    0x9583a0c3,// SPOOR_TYPE_IDEA,
};

typedef struct CalendarData {
    u32 spoor_objects_indexes[SPOOR_OBJECTS_MAX];
    u32 spoor_objects_indexes_count;
    u32 index;
    s32 (*sort_func)(const void *data0, const void *data1);
    SpoorFilter spoor_filter;
    u32 days_count;
    u32 time_offset;
    s32 date_offset;
    bool is_grab;
} CalendarData;

typedef struct SettingsData {
    u32 index;
} SettingsData;

enum {
    SPOOR_OBJECTS_STORAGE_NO_LOAD,
    SPOOR_OBJECTS_STORAGE_LOAD,
};

#endif
