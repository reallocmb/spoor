#include"spoor.h"
#include"s_font.c"
#include"s_render.c"
#include"s_status_bar.c"
#include"s_view.c"
#include"s_command_buffer.c"
#include"s_xlib.c"
#include"s_win32.c"

void task_list_render_func(View *view)
{
    u16 x = view->x;
    u16 y = view->y;
    u16 width = view->width;
    u16 height = view->height;

    u32 size = GlobalGraphic.font.size;
    font_size_set(&GlobalGraphic.font, 15);
    render_text(x + 50, y + GlobalGraphic.font.height, (u8 *)"Mein Name ist Matthias, was ist deiner?", 0xffaa33bb);
    font_size_set(&GlobalGraphic.font, size);
}

void render_func(void)
{
    views_render(GlobalGraphic.views);
    status_bar_render_func();
}

void input_func(u8 key)
{
    printf("char %c\n", key);
    if (GlobalGraphic.mode == GRAPHIC_MODE_NORMAL)
    {
        switch (key)
        {
            case ':':
            {
                GlobalGraphic.mode = GRAPHIC_MODE_COMMAND_BUFFER;
                GlobalGraphic.command_buffer.buffer[0] = ':';
                GlobalGraphic.command_buffer.buffer[1] = 0;
                GlobalGraphic.command_buffer.count = 1;
            } break;

            case SPOOR_INPUT_ASCII_KEY_CONTROL_S:
            {
                printf("view append vertical\n");
                view_append(&GlobalGraphic.views,
                            &GlobalGraphic.views_count,
                            &GlobalGraphic.views_index,
                            VIEW_FLAG_VERTICAL,
                            view_default_render_func);

            } break;

            case SPOOR_INPUT_ASCII_KEY_CONTROL_H:
            {
                printf("view append horizontal\n");
                view_append(&GlobalGraphic.views,
                            &GlobalGraphic.views_count,
                            &GlobalGraphic.views_index,
                            VIEW_FLAG_HORIZONTAL,
                            view_default_render_func);
            } break;

            default:
            {
                GlobalGraphic.command_buffer.buffer[GlobalGraphic.command_buffer.count++] = key;
                GlobalGraphic.command_buffer.buffer[GlobalGraphic.command_buffer.count] = 0;
            } break;
        }

        bool have_to_command_buffer_clear = true;

        if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, "lt", 2) == 0)
        {
            GlobalGraphic.views[GlobalGraphic.views_index].render_func = task_list_render_func;
        }
        else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, " s", 2) == 0)
        {
            printf("wind left...\n");
            view_focus_left(&GlobalGraphic.views_index);
        }
        else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, " n", 2) == 0)
        {
            printf("wind down...\n");
            view_focus_down(&GlobalGraphic.views_index);
        }
        else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, " r", 2) == 0)
        {
            printf("wind up...\n");
            view_focus_up(&GlobalGraphic.views_index);
        }
        else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, " t", 2) == 0)
        {
            printf("wind right...\n");
            view_focus_right(&GlobalGraphic.views_index);
        }
        else
            have_to_command_buffer_clear = false;


        if (have_to_command_buffer_clear)
            command_buffer_clear(&GlobalGraphic.command_buffer);
    }
    else if (GlobalGraphic.mode == GRAPHIC_MODE_COMMAND_BUFFER)
    {
        switch (key)
        {
            case SPOOR_INPUT_ASCII_KEY_RETURN: 
            {
                GlobalGraphic.mode = GRAPHIC_MODE_NORMAL;
                command_buffer_execute();
            } break;

            case SPOOR_INPUT_ASCII_KEY_BACKSPACE:
            {
                GlobalGraphic.command_buffer.buffer[--GlobalGraphic.command_buffer.count] = 0;
            } break;

            default:
            {
                GlobalGraphic.command_buffer.buffer[GlobalGraphic.command_buffer.count++] = key;
                GlobalGraphic.command_buffer.buffer[GlobalGraphic.command_buffer.count] = 0;
            } break;
        }
    }

    printf("Key Pressed: %d\n", key);

    graphic_update();
}

int main(void)
{
    GlobalGraphic.render_func = render_func;
    GlobalGraphic.input_func = input_func;

    //CONFIG_COLOR_BACKGROUND_SET(0xff883388);
    //CONFIG_GRAPHIC_SCALE_SET(2.1);
    
    CONFIG_STATUS_BAR_FONT_SIZE_SET(40);
    CONFIG_VIEW_FONT_SIZE_SET(12);
    CONFIG_VIEW_BORDER_SIZE_SET(1);
    CONFIG_VIEW_GAP_SIZE_SET(4);

#if 0
    font_load(&GlobalGraphic.font, "data/FreeMono.ttf", 25);
#elif 1
    font_load(&GlobalGraphic.font, "data/LiberationMono-Regular.ttf", CONFIG_STATUS_BAR_FONT_SIZE);
#else
    font_load(&GlobalGraphic.font, "data/Essays1743.ttf", 40);
#endif
    status_bar_init();
    views_init(&GlobalGraphic.views);

    graphic_init();
    graphic_main_loop();
    
    return 0;
}
