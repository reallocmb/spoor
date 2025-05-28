#include"spoor.h"
#include"s_redbas.c"
#include"s_time.c"
#include"s_sort.c"
#include"s_object.c"
#include"s_render.c"
#include"s_font.c"
#include"s_status_bar.c"
#include"s_view.c"
#include"s_filter.c"
#include"s_command_buffer.c"
#include"s_xlib.c"
#include"s_win32.c"
#include"s_task_list.c"
#include"s_calendar.c"
#include"s_settings.c"

void render_func(void)
{
    if (GlobalGraphic.render_flags & RENDER_FLAG_ALL || GlobalGraphic.render_flags & RENDER_FLAG_INDEX_CHANGE)
    {
        u32 i;
        for (i = 0; i < GlobalGraphic.pixels_count; i++)
            GlobalGraphic.pixels[i] = CONFIG_COLOR_BACKGROUND;
        views_render(GlobalGraphic.views);
    }
    if (GlobalGraphic.render_flags & RENDER_FLAG_ALL || GlobalGraphic.render_flags & RENDER_FLAG_COMMAND_BUFFER)
        status_bar_render_func();
}

void input_func(u8 key)
{
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
                view_append(&GlobalGraphic.views,
                            &GlobalGraphic.views_count,
                            &GlobalGraphic.views_index,
                            VIEW_FLAG_VERTICAL,
                            view_default_render_func);
            } break;

            case SPOOR_INPUT_ASCII_KEY_CONTROL_H:
            {
                view_append(&GlobalGraphic.views,
                            &GlobalGraphic.views_count,
                            &GlobalGraphic.views_index,
                            VIEW_FLAG_HORIZONTAL,
                            view_default_render_func);
            } break;

            case 'I':
            {
                font_size_set(&GlobalGraphic.font, GlobalGraphic.font.size + 1);
            } break;

            case 'i':
            {
                font_size_set(&GlobalGraphic.font, GlobalGraphic.font.size - 1);
            } break;

            default:
            {
                GlobalGraphic.command_buffer.buffer[GlobalGraphic.command_buffer.count++] = key;
                GlobalGraphic.command_buffer.buffer[GlobalGraphic.command_buffer.count] = 0;
            } break;
        }

        bool have_to_command_buffer_clear = true;

        if (GlobalGraphic.command_buffer.count >= 2)
        {
            /* test font size */
            if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, "lt", 2) == 0)
            {
                GlobalGraphic.views[GlobalGraphic.views_index].data = malloc(sizeof(TaskListData));
                ((TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->index = 0;
                ((TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_filter.type = SPOOR_FILTER_TYPE_TASK | SPOOR_FILTER_TYPE_APPOINTMENT | SPOOR_FILTER_TYPE_EVENT | SPOOR_FILTER_TYPE_HABIT | SPOOR_FILTER_TYPE_PROJECT;
                ((TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_filter.status = SPOOR_FILTER_STATUS_NOT_STARTED | SPOOR_FILTER_STATUS_IN_PROGRESS;
                ((TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_objects_indexes_count = 0;
                ((TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->sort_func = sort_default_func;

                GlobalGraphic.views[GlobalGraphic.views_index].render_func = task_list_render_func;
                GlobalGraphic.views[GlobalGraphic.views_index].input_func = task_list_input_func;
            }
            else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, "lc", 2) == 0)
            {
                GlobalGraphic.views[GlobalGraphic.views_index].data = malloc(sizeof(CalendarData));
                ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->index = 0;
                ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_filter.type = SPOOR_FILTER_TYPE_ALL;
                ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_filter.status = SPOOR_FILTER_STATUS_ALL;
                ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_objects_indexes_count = 0;
                ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->sort_func = sort_calendar_func;
                ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->days_count = 5;
                ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->time_offset = 8;
                ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->date_offset = 0;
#if 0
                ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_objects_indexes = NULL;
#endif

                GlobalGraphic.views[GlobalGraphic.views_index].render_func = calendar_render_func;
                GlobalGraphic.views[GlobalGraphic.views_index].input_func = calendar_input_func;
            }
            else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, "ls", 2) == 0)
            {
                GlobalGraphic.views[GlobalGraphic.views_index].data = malloc(sizeof(Data));
                GlobalGraphic.views[GlobalGraphic.views_index].render_func = settings_render_func;
                GlobalGraphic.views[GlobalGraphic.views_index].input_func = settings_input_func;
            }
            else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, " c", 2) == 0)
            {
                view_close(GlobalGraphic.views_index);
            }
            else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, " s", 2) == 0)
            {
                view_focus_left(&GlobalGraphic.views_index);
            }
            else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, " n", 2) == 0)
            {
                view_focus_down(&GlobalGraphic.views_index);
            }
            else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, " r", 2) == 0)
            {
                view_focus_up(&GlobalGraphic.views_index);
            }
            else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, " t", 2) == 0)
            {
                view_focus_right(&GlobalGraphic.views_index);
            }
            else
                have_to_command_buffer_clear = false;
        }
        else
            have_to_command_buffer_clear = false;

        if (have_to_command_buffer_clear)
            command_buffer_clear(&GlobalGraphic.command_buffer);
        else
        {
            /* view input current index */
            View *view_current = &GlobalGraphic.views[GlobalGraphic.views_index];
            if (view_current->input_func != NULL)
                view_current->input_func(view_current, key);
        }
    }
    else if (GlobalGraphic.mode == GRAPHIC_MODE_COMMAND_BUFFER)
    {
        switch (key)
        {
            case SPOOR_INPUT_ASCII_KEY_RETURN: 
            {
            } break;

            case SPOOR_INPUT_ASCII_KEY_BACKSPACE:
            {
                GlobalGraphic.command_buffer.buffer[--GlobalGraphic.command_buffer.count] = 0;
            } break;
            
            case 0:
            {
            } break;

            default:
            {
                GlobalGraphic.command_buffer.buffer[GlobalGraphic.command_buffer.count++] = key;
                GlobalGraphic.command_buffer.buffer[GlobalGraphic.command_buffer.count] = 0;
            } break;
        }
        if (key == SPOOR_INPUT_ASCII_KEY_RETURN)
        {
            GlobalGraphic.mode = GRAPHIC_MODE_NORMAL;
            command_buffer_execute();
            command_buffer_clear(&GlobalGraphic.command_buffer);
            GlobalGraphic.render_flags = RENDER_FLAG_ALL;
        }
        else
            GlobalGraphic.render_flags = RENDER_FLAG_COMMAND_BUFFER;
    }

    graphic_update();
}

#if _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(void)
#endif
{
    GlobalGraphic.render_func = render_func;
    GlobalGraphic.input_func = input_func;

    mkdir(".spoor");

    font_load(&GlobalGraphic.font, "data/FiraCode-Regular.ttf", CONFIG(FONT_DEFAULT_SIZE));

    status_bar_init();
    views_init(&GlobalGraphic.views);

    graphic_init();

    /* remove after */
    view_append(&GlobalGraphic.views,
                &GlobalGraphic.views_count,
                &GlobalGraphic.views_index,
                VIEW_FLAG_VERTICAL,
                view_default_render_func);
    GlobalGraphic.views[GlobalGraphic.views_index].data = malloc(sizeof(TaskListData));
    ((TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->index = 0;
    ((TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_filter.type = SPOOR_FILTER_TYPE_TASK | SPOOR_FILTER_TYPE_APPOINTMENT | SPOOR_FILTER_TYPE_EVENT | SPOOR_FILTER_TYPE_HABIT;
    ((TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_filter.status = SPOOR_FILTER_STATUS_NOT_STARTED | SPOOR_FILTER_STATUS_IN_PROGRESS;
    ((TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_objects_indexes_count = 0;
    ((TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->sort_func = sort_default_func;
    ((TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->offset_y = 0;

    GlobalGraphic.views[GlobalGraphic.views_index].render_func = task_list_render_func;
    GlobalGraphic.views[GlobalGraphic.views_index].input_func = task_list_input_func;

#if 0
    view_append(&GlobalGraphic.views,
                &GlobalGraphic.views_count,
                &GlobalGraphic.views_index,
                VIEW_FLAG_VERTICAL,
                view_default_render_func);
    GlobalGraphic.views[GlobalGraphic.views_index].data = malloc(sizeof(CalendarData));
    ((Data *)GlobalGraphic.views[GlobalGraphic.views_index].data)->index = 0;
    ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_filter.type = SPOOR_FILTER_TYPE_ALL;
    ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_filter.status = SPOOR_FILTER_STATUS_ALL;
    ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->days_count = 5;
    ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->time_offset = 8;
    ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->date_offset = 0;
    ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->sort_func = sort_calendar_func;
#if 0
    ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_objects = NULL;
    ((CalendarData *)GlobalGraphic.views[GlobalGraphic.views_index].data)->spoor_objects_count = 0;
#endif

    GlobalGraphic.views[GlobalGraphic.views_index].render_func = calendar_render_func;
    GlobalGraphic.views[GlobalGraphic.views_index].input_func = calendar_input_func;
#endif

    graphic_main_loop();
    
    return 0;
}
