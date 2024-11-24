#ifndef S_CONFIG_H
#define S_CONFIG_H

enum {
    SPOOR_COLOR_FOREGROUND,
    SPOOR_COLOR_BACKGROUND,
};

#define CONFIG_COLOR_BACKGROUND config_colors[SPOOR_COLOR_BACKGROUND]
#define CONFIG_COLOR_BACKGROUND_SET(color) config_colors[SPOOR_COLOR_BACKGROUND] = color

#define CONFIG_COLOR_FOREGROUND config_colors[SPOOR_COLOR_FOREGROUND]
#define CONFIG_COLOR_FOREGROUND_SET(color) config_colors[SPOOR_COLOR_FOREGROUND] = color

u32 config_colors[] = {
    0xffffffff,
    0xff105020,
    /*
    0xff0c0c0c,
    0xffa08563,
    */
};

/* CONFIG STATUS BAR */
enum {
    SPOOR_STATUS_BAR_HEIGHT,
    SPOOR_STATUS_BAR_PADDING,
    SPOOR_STATUS_BAR_FONT_SIZE,
    SPOOR_COLOR_STATUS_BAR_FOREGROUND,
    SPOOR_COLOR_STATUS_BAR_BACKGROUND,
};

#define CONFIG_STATUS_BAR_HEIGHT config_status_bar[SPOOR_STATUS_BAR_HEIGHT]
#define CONFIG_STATUS_BAR_HEIGHT_SET(height) config_status_bar[SPOOR_STATUS_BAR_HEIGHT] = height

#define CONFIG_STATUS_BAR_PADDING config_status_bar[SPOOR_STATUS_BAR_PADDING]
#define CONFIG_STATUS_BAR_PADDING_SET(padding) config_status_bar[SPOOR_STATUS_BAR_PADDING] = padding

#define CONFIG_STATUS_BAR_FONT_SIZE config_status_bar[SPOOR_STATUS_BAR_FONT_SIZE]
#define CONFIG_STATUS_BAR_FONT_SIZE_SET(font_size) config_status_bar[SPOOR_STATUS_BAR_FONT_SIZE] = font_size

#define CONFIG_COLOR_STATUS_BAR_BACKGROUND config_status_bar[SPOOR_COLOR_STATUS_BAR_BACKGROUND]
#define CONFIG_COLOR_STATUS_BAR_BACKGROUND_SET(color) config_status_bar[COLOR_STATUS_BAR_BACKGROUND] = color

#define CONFIG_COLOR_STATUS_BAR_FOREGROUND config_status_bar[SPOOR_COLOR_STATUS_BAR_FOREGROUND]
#define CONFIG_COLOR_STATUS_BAR_FOREGROUND_SET(color) config_status_bar[SPOOR_COLOR_STATUS_BAR_FOREGROUND] = color

u32 config_status_bar[] = {
    30,
    2,
    40,
    0xff000000,
    0x22000000,
    /*
    0xff0c0c0c,
    0x440c0c0c,
    */
};

enum {
    SPOOR_GRAPHIC_SCALE,
};

#define CONFIG_GRAPHIC_SCALE config_graphic[SPOOR_GRAPHIC_SCALE]
#define CONFIG_GRAPHIC_SCALE_SET(scale) config_graphic[SPOOR_GRAPHIC_SCALE] = scale;

float config_graphic[] = {
    1,
};

/* CONFIG VIEW */
enum {
    SPOOR_VIEW_BORDER_SIZE,
    SPOOR_VIEW_GAP_SIZE,
    SPOOR_VIEW_BORDER_COLOR,
    SPOOR_VIEW_FONT_SIZE,
};

#define CONFIG_VIEW_BORDER_SIZE config_view[SPOOR_VIEW_BORDER_SIZE]
#define CONFIG_VIEW_BORDER_SIZE_SET(border_size) config_view[SPOOR_VIEW_BORDER_SIZE] = border_size

#define CONFIG_VIEW_GAP_SIZE config_view[SPOOR_VIEW_GAP_SIZE]
#define CONFIG_VIEW_GAP_SIZE_SET(gap_size) config_view[SPOOR_VIEW_GAP_SIZE] = gap_size

#define CONFIG_VIEW_BORDER_COLOR config_view[SPOOR_VIEW_BORDER_COLOR]
#define CONFIG_VIEW_BORDER_COLOR_SET(border_color) config_view[SPOOR_VIEW_BORDER_COLOR] = border_color

#define CONFIG_VIEW_FONT_SIZE config_view[SPOOR_VIEW_FONT_SIZE]
#define CONFIG_VIEW_FONT_SIZE_SET(font_size) config_view[SPOOR_VIEW_FONT_SIZE] = font_size

u32 config_view[] = {
    2,
    10,
    0xffffffff,
    15,
};

/* status colors */
const u32 CONFIG_COLOR_STATUS[3] = {
    0xff551100,
    0xff887722,
    0xff228811,
};

#endif
