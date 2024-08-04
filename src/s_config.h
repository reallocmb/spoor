#ifndef S_CONFIG_H
#define S_CONFIG_H

enum {
    COLOR_BACKGROUND,
};

#define CONFIG_COLOR_BACKGROUND config_colors[COLOR_BACKGROUND]
#define CONFIG_COLOR_BACKGROUND_SET(color) config_colors[COLOR_BACKGROUND] = color

u32 config_colors[] = {
    0xff0c0c0c,
};

#endif
