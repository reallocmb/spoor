#include"s_types.h"
#include"spoor.h"
#include"s_render.c"
#include"s_xlib.c"
#include"s_win32.c"

void render_func(Graphic *graphic)
{
}

int main(void)
{
    Spoor spoor = { 0 };
    spoor.graphic.running = true;
    spoor.graphic.width = 600;
    spoor.graphic.height = 400;
    spoor.graphic.pixels = NULL;

    CONFIG_COLOR_BACKGROUND_SET(0xff883388);

    graphic_init(&spoor.graphic);
    graphic_main_loop(&spoor.graphic);

    return 0;
}
