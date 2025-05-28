#include"spoor.h" /* compile_commands */

void settings_render_func(View *view)
{
    SettingsData *settings_data = (SettingsData *)view->data;

    u32 font_size_backup = GlobalGraphic.font.size;
    font_size_set(&GlobalGraphic.font, 30);

    u32 x, y;
    x = 40;
    y = GlobalGraphic.font.height * 2;

    /* title */
    view_render_text(view, x, y, "SETTING", CONFIG(COLOR_FOREGROUND));
    view_render_text(view, x + 800, y, "VALUE", CONFIG(COLOR_FOREGROUND));

    render_line_horizontal(view->x, view->y + y, view->width, 0xff000000);

    char value_buffer[100];

    y += GlobalGraphic.font.height;
    view_render_text(view, x, y, "COLOR_FOREGROUND", CONFIG(COLOR_FOREGROUND));
    sprintf(value_buffer, "%x", CONFIG(COLOR_FOREGROUND));
    view_render_text(view, x + 800, y, value_buffer, CONFIG(COLOR_FOREGROUND));

    y += GlobalGraphic.font.height;
    view_render_text(view, x, y, "COLOR_BACKGROUND", CONFIG(COLOR_FOREGROUND));
    sprintf(value_buffer, "%x", CONFIG(COLOR_BACKGROUND));
    view_render_text(view, x + 800, y, value_buffer, CONFIG(COLOR_FOREGROUND));

    y += GlobalGraphic.font.height;
    view_render_text(view, x, y, "FONT_DEFAULT_SIZE", CONFIG(COLOR_FOREGROUND));
    sprintf(value_buffer, "%d", CONFIG(FONT_DEFAULT_SIZE));
    view_render_text(view, x + 800, y, value_buffer, CONFIG(COLOR_FOREGROUND));

    font_size_set(&GlobalGraphic.font, font_size_backup);
}

void settings_input_func(View *view, u8 key)
{
    graphic_update();
}
