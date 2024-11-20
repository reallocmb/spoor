#include"spoor.h"
#include"s_redbas.c"
#include"s_time.c"
#include"s_object.c"
#include"s_font.c"
#include"s_render.c"
#include"s_status_bar.c"
#include"s_view.c"
#include"s_command_buffer.c"
#include"s_xlib.c"
#include"s_win32.c"
#include"s_task_list.c"

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
                GlobalGraphic.views[GlobalGraphic.views_index].data = malloc(sizeof(TaskListData));
                ((TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->hand_index = 0;
            } break;

            default:
            {
                GlobalGraphic.command_buffer.buffer[GlobalGraphic.command_buffer.count++] = key;
                GlobalGraphic.command_buffer.buffer[GlobalGraphic.command_buffer.count] = 0;
            } break;
        }

        bool have_to_command_buffer_clear = true;

        /* view input current index */
        View *view_current = &GlobalGraphic.views[GlobalGraphic.views_index];
        if (view_current->input_func != NULL)
            view_current->input_func(view_current, key);

        if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, "lt", 2) == 0)
        {
            GlobalGraphic.views[GlobalGraphic.views_index].render_func = task_list_render_func;
            GlobalGraphic.views[GlobalGraphic.views_index].input_func = task_list_input_func;
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
    printf("SOTT bytes %zd\n", sizeof(SOTT));
    printf("SpoorObject bytes %zd\n", sizeof(SpoorObject));
    printf("SOLink bytes %zd\n", sizeof(SOLink));

    GlobalGraphic.render_func = render_func;
    GlobalGraphic.input_func = input_func;

    //CONFIG_COLOR_BACKGROUND_SET(0xff883388);
    //CONFIG_GRAPHIC_SCALE_SET(2.1);
    
#if 1
    CONFIG_STATUS_BAR_FONT_SIZE_SET(40);
    CONFIG_VIEW_FONT_SIZE_SET(12);
    CONFIG_VIEW_BORDER_SIZE_SET(1);
    CONFIG_VIEW_GAP_SIZE_SET(4);
#endif

    font_load(&GlobalGraphic.font, "data/LiberationMono-Regular.ttf", CONFIG_STATUS_BAR_FONT_SIZE);
    status_bar_init();
    views_init(&GlobalGraphic.views);

    /* test */
    SpoorTimeSpan spoor_time_span;
    spoor_time_span_create(&spoor_time_span, "10m-12m", 7);
    spoor_time_span_create(&spoor_time_span, "1m800-1000", 10);
    spoor_time_span_create(&spoor_time_span, "5d-8d", 5);
    spoor_time_span_create(&spoor_time_span, "12d700-900", 10);
    spoor_time_span_create(&spoor_time_span, "-8d700-900", 10);
    spoor_time_span_create(&spoor_time_span, "12d700-12d900", 13);
    spoor_time_span_create(&spoor_time_span, "-8d700--8d900", 13);
    spoor_time_span_create(&spoor_time_span, "8d700--8d900", 12);
    spoor_time_span_create(&spoor_time_span, "700--8d900", 10);
    spoor_time_span_create(&spoor_time_span, "700-8d900", 9);
    spoor_time_span_create(&spoor_time_span, "700-900", 9);
    /*
    spoor_time_span_create(&spoor_time_span, "16.9.2024:800", 13);
    */

    graphic_init();
    graphic_main_loop();
    
    return 0;
}
