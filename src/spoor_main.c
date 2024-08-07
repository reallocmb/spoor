#include"s_types.h"
#include"spoor.h"
#include"s_font.c"
#include"s_render.c"
#include"s_xlib.c"
#include"s_win32.c"

#define STATUS_BAR_HEIGHT 30

void status_bar_render(Graphic *graphic)
{
    u16 height = STATUS_BAR_HEIGHT;
    u16 y = graphic->height - height;

    render_rectangle_fill(graphic, 0, y, graphic->width, height, CONFIG_COLOR_STATUS_BAR_BACKGROUND);
    render_text(graphic, 10, graphic->height - 50, L"statusCommandgolijen", CONFIG_COLOR_STATUS_BAR_FOREGROUND);
    // render_label(graphic, 10, graphic->height, height, 5, L"STATUS BAR TESTg", CONFIG_COLOR_STATUS_BAR_FOREGROUND);
}

void render_func(Graphic *graphic)
{
    status_bar_render(graphic);
}

int main(void)
{
    Spoor spoor = { 0 };
    spoor.graphic.running = true;
    spoor.graphic.width = 800;
    spoor.graphic.height = 600;
    spoor.graphic.pixels = NULL;
    spoor.graphic.render_func = render_func;
    
    CONFIG_COLOR_BACKGROUND_SET(0xff883388);
    
    font_load(&spoor.graphic.font, "../data/FreeMono.ttf", 30);
    
    graphic_init(&spoor.graphic);
    graphic_main_loop(&spoor.graphic);
    
    return 0;
}
