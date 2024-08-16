#include"s_types.h"
#include"spoor.h"
#include"s_font.c"
#include"s_render.c"
#include"s_status_bar.c"
#include"s_command_buffer.c"
#include"s_xlib.c"
#include"s_win32.c"

void status_bar_render(Graphic *graphic)
{
    printf("CONFIG_STATUS_BAR_HEIGHT: %d\n", CONFIG_STATUS_BAR_HEIGHT);
    u16 height = CONFIG_STATUS_BAR_HEIGHT;
    u16 y = graphic->height - height;

    render_rectangle_fill(graphic, 0, y, graphic->width, height, CONFIG_COLOR_STATUS_BAR_BACKGROUND);
    render_text(graphic,
                10,
                graphic->height + (graphic->font.face->size->metrics.descender >> 6) - CONFIG_STATUS_BAR_PADDING,
                (const wchar_t *)graphic->command_buffer.buffer,
                CONFIG_COLOR_STATUS_BAR_FOREGROUND);
    // render_label(graphic, 10, graphic->height, height, 5, L"STATUS BAR TESTg", CONFIG_COLOR_STATUS_BAR_FOREGROUND);
}

void render_func(Graphic *graphic)
{
    status_bar_render(graphic);
}

void input_func(Graphic *graphic, u8 key)
{
    printf("char %c\n", key);
    if (graphic->mode == GRAPHIC_MODE_NORMAL)
    {
        switch (key)
        {
            case ':':
            {
                graphic->mode = GRAPHIC_MODE_COMMAND_BUFFER;
                graphic->command_buffer.buffer[0] = ':';
                graphic->command_buffer.buffer[1] = 0;
                graphic->command_buffer.count = 1;
            } break;
        }
    }
    else if (graphic->mode == GRAPHIC_MODE_COMMAND_BUFFER)
    {
        switch (key)
        {
            case SPOOR_INPUT_ASCII_KEY_RETURN: 
            {
                graphic->mode = GRAPHIC_MODE_NORMAL;
                command_buffer_execute(&graphic->command_buffer);
            } break;

            case SPOOR_INPUT_ASCII_KEY_BACKSPACE:
            {
                graphic->command_buffer.buffer[--graphic->command_buffer.count] = 0;
            } break;

            default:
            {
                graphic->command_buffer.buffer[graphic->command_buffer.count++] = key;
                graphic->command_buffer.buffer[graphic->command_buffer.count] = 0;
            } break;
        }
    }

    printf("Key Pressed: %d\n", key);

    graphic_update(graphic);
}

int main(void)
{
    Spoor spoor = { 0 };
    spoor.graphic.running = true;
    spoor.graphic.width = 800;
    spoor.graphic.height = 600;
    spoor.graphic.pixels = NULL;
    spoor.graphic.render_func = render_func;
    spoor.graphic.input_func = input_func;

    //CONFIG_COLOR_BACKGROUND_SET(0xff883388);
    //CONFIG_GRAPHIC_SCALE_SET(2.1);

    //font_load(&spoor.graphic.font, "data/FreeMono.ttf", 25);
    font_load(&spoor.graphic.font, "data/Essays1743.ttf", 40);
    status_bar_init(&spoor.graphic);
    graphic_init(&spoor.graphic);
    graphic_main_loop(&spoor.graphic);
    
    return 0;
}
