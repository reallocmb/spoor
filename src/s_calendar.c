#include"spoor.h"

void calendar_input_func(View *view, u8 key)
{
    u16 x = view->x;
    u16 y = view->y;

    /* header */
    render_rectangle_fill(x, y, view->width, view->height, 0x55666666);
    x += 100;
    y += 50;
    view_render_text(view, x, y, "Monday", CONFIG_COLOR_FOREGROUND);
}

void calendar_render_func(View *view)
{
    Data *data = (Data *)view->data;

    u32 spoor_objects_count = spoor_object_load(&data->spoor_objects);
}
