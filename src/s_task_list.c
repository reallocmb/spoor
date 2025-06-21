#include"spoor.h"

void task_list_render_func(View *view)
{
    font_size_set(&GlobalGraphic.font, CONFIG(FONT_DEFAULT_SIZE));

    TaskListData *data = (TaskListData *)view->data;
    data->spoor_filter.flags = 0;
    data->spoor_filter.schedule.start = SPOOR_FILTER_TIME_SPAN_DEFAULT_START;
    data->spoor_filter.schedule.end = SPOOR_FILTER_TIME_SPAN_DEFAULT_END;

    if (global_data.spoor_objects_count == 0)
        spoor_objects_load(global_data.spoor_objects,
                           &global_data.spoor_objects_count,
                           NULL,
                           NULL);

#if 1
    data->spoor_objects_indexes_count = 0;
    spoor_objects_indexes_set(data->spoor_objects_indexes,
                              &data->spoor_objects_indexes_count,
                              &data->spoor_filter,
                              sort_default_func,
                              global_data.spoor_objects,
                              global_data.spoor_objects_count);

#else
    if (data->spoor_objects_indexes_count == 0)
    {
        spoor_objects_indexes_set(data->spoor_objects_indexes,
                                  &data->spoor_objects_indexes_count,
                                  &data->spoor_filter,
                                  global_data.spoor_objects,
                                  global_data.spoor_objects_count);
    }
    else if (global_data.indexes_sync.mode != 0)
    {
        spoor_objects_indexes_sync((Data *)data);
    }
#endif

    if (data->spoor_objects_indexes_count == 0)
        data->index = TASK_LIST_HAND_INDEX_DEFAULT;

    /* Render */
    const u32 FONT_CHAR_WIDTH = font_text_width_in_pixels(&GlobalGraphic.font, "_");

    u16 x = view->x;
    u16 y = view->y;

    /* Render Task List */

    const u16 LINE_NUMBER_WIDTH_MAX = 4;
    const u16 LINE_NUMBER_WIDTH = FONT_CHAR_WIDTH * 5;

    const u16 STATUS_WIDTH = FONT_CHAR_WIDTH * 5;
    const u16 PRIORITY_WIDTH = FONT_CHAR_WIDTH * 5;
    const u16 TYPE_WIDTH = FONT_CHAR_WIDTH * 6;
    const u16 TIME_TRACKED_WIDTH = FONT_CHAR_WIDTH * 25;
    const u16 TIME_SCHEDULE_WIDTH = FONT_CHAR_WIDTH * 25;
    const u16 TIME_DEADLINE_WIDTH = FONT_CHAR_WIDTH * 19;
    const u16 TIME_DEADLINE_DAY_WIDTH = FONT_CHAR_WIDTH * 6;

    const u16 TITLE_WIDTH_MIN = 40 * FONT_CHAR_WIDTH;

    u16 SUM_RIGHT = LINE_NUMBER_WIDTH
        + STATUS_WIDTH
        + PRIORITY_WIDTH
        + TYPE_WIDTH
        + TIME_TRACKED_WIDTH
        + TIME_SCHEDULE_WIDTH
        + TIME_DEADLINE_WIDTH
        + TIME_DEADLINE_DAY_WIDTH;

    u16 TITLE_WIDTH = view->width - SUM_RIGHT;

    if (TITLE_WIDTH < TITLE_WIDTH_MIN ||
        view->width <= SUM_RIGHT)
        TITLE_WIDTH = TITLE_WIDTH_MIN;

    u16 LINES_USE_COUNT = 1;

    u16 sim_x = view->x + LINE_NUMBER_WIDTH + TITLE_WIDTH;
    if (sim_x + TIME_DEADLINE_DAY_WIDTH + TIME_DEADLINE_WIDTH > view->x + view->width)
    {
       sim_x = view->x + LINE_NUMBER_WIDTH; 
       LINES_USE_COUNT++;
    }
    sim_x += TIME_DEADLINE_DAY_WIDTH + TIME_DEADLINE_WIDTH;
    if (sim_x + TIME_SCHEDULE_WIDTH > view->x + view->width)
    {
       sim_x = view->x + LINE_NUMBER_WIDTH; 
       LINES_USE_COUNT++;
    }
    sim_x += TIME_SCHEDULE_WIDTH;
    if (sim_x + TIME_TRACKED_WIDTH > view->x + view->width)
    {
       sim_x = view->x + LINE_NUMBER_WIDTH; 
       LINES_USE_COUNT++;
    }
    sim_x += TIME_TRACKED_WIDTH;
    if (sim_x + TYPE_WIDTH > view->x + view->width)
    {
       sim_x = view->x + LINE_NUMBER_WIDTH; 
       LINES_USE_COUNT++;
    }
    sim_x += TYPE_WIDTH;
    if (sim_x + PRIORITY_WIDTH > view->x + view->width)
    {
       sim_x = view->x + LINE_NUMBER_WIDTH; 
       LINES_USE_COUNT++;
    }
    sim_x += PRIORITY_WIDTH;
    if (sim_x + STATUS_WIDTH > view->x + view->width)
    {
       sim_x = view->x + LINE_NUMBER_WIDTH; 
       LINES_USE_COUNT++;
    }

#if 0
    char buffer_info[100] = { 0 };
    sprintf(buffer_info, "view->width: %d, font_char_width: %d, title_width: %d", view->width, FONT_CHAR_WIDTH, TITLE_WIDTH);
    view_render_text(view, 50, 200, buffer_info, CONFIG(COLOR_FOREGROUND));
#endif

    /* Render Selection */
#if 0
    u32 render_index_start = 0;
    u32 render_max = data->spoor_objects_indexes_count;
    u32 render_index_step = 1;
    u32 render_y_step = GlobalGraphic.font.height;

    if (GlobalGraphic.render_flags & RENDER_FLAG_INDEX_CHANGE)
    {
        if (data->index_last < data->index)
        {
            render_index_start = data->index_last;
            render_max = data->index + 1;
            render_index_step = data->index - data->index_last;
        }
        else
        {
            render_index_start = data->index;
            render_max = data->index_last + 1;
            render_index_step = data->index_last - data->index;
        }
        render_y_step = GlobalGraphic.font.height * render_index_step;

        /* clear background of last index object */
        render_rectangle_fill(x, y + data->index_last * GlobalGraphic.font.height * LINES_USE_COUNT,
                              view->width, GlobalGraphic.font.height * LINES_USE_COUNT - 1,
                              CONFIG(COLOR_BACKGROUND));

        render_rectangle_fill(x, y + data->index * GlobalGraphic.font.height * LINES_USE_COUNT,
                              view->width, GlobalGraphic.font.height * LINES_USE_COUNT - 1,
                              CONFIG(COLOR_BACKGROUND));
    }
#endif

    /* hand highlight */
    if (data->index >= data->spoor_objects_indexes_count)
        data->index = data->spoor_objects_indexes_count - 1;

    render_rectangle_fill(x, y + data->index * GlobalGraphic.font.height * LINES_USE_COUNT,
                          view->width, GlobalGraphic.font.height * LINES_USE_COUNT - 1,
                          CONFIG(STATUS_BAR_COLOR_BACKGROUND));

    /* highlight selection */
    u32 s;
    for (s = 0; s < data->selection_count; s++)
    {
        render_rectangle_fill(x, y + data->selection[s] * GlobalGraphic.font.height * LINES_USE_COUNT,
                              view->width, GlobalGraphic.font.height * LINES_USE_COUNT - 1,
                              0xff5555ff);
    }


    y += GlobalGraphic.font.height + (GlobalGraphic.font.face->size->metrics.descender >> 6);

    u32 i;
    for (i = data->offset_y; i < data->spoor_objects_indexes_count - data->offset_y; i++, y += GlobalGraphic.font.height)
    {
        x = view->x;
        /* background */
#if 1
        u32 color = 0xccc0c0c0;
        color = 0x44884422;
        if (i % 2 == 0 && i != data->index)
            render_rectangle_fill(view->x,
                                  view->y + i * GlobalGraphic.font.height * LINES_USE_COUNT,
                                  view->width,
                                  GlobalGraphic.font.height * LINES_USE_COUNT,
                                  color);
#else
        if (i % 2 == 0 && i != data->index)
            render_rectangle_fill(x, y - GlobalGraphic.font.height - (GlobalGraphic.font.face->size->metrics.descender >> 6),
                                  view->width,
                                  GlobalGraphic.font.height * LINES_USE_COUNT,
                                  0x44884422);

#endif

        /* day separator line */
        if (!((i + 1) >= data->spoor_objects_indexes_count))
        {
            if (global_data.spoor_objects[data->spoor_objects_indexes[i]].deadline.end.year != global_data.spoor_objects[data->spoor_objects_indexes[i + 1]].deadline.end.year ||
                global_data.spoor_objects[data->spoor_objects_indexes[i]].deadline.end.mon != global_data.spoor_objects[data->spoor_objects_indexes[i + 1]].deadline.end.mon ||
                global_data.spoor_objects[data->spoor_objects_indexes[i]].deadline.end.day != global_data.spoor_objects[data->spoor_objects_indexes[i + 1]].deadline.end.day)
            {
                // view_render_line_horizontal(view, view->x, y - (GlobalGraphic.font.face->size->metrics.descender >> 6) - 1, view->width - 1, 0x66000000);
                view_render_line_horizontal(view,
                                            view->x,
                                            view->y + i * GlobalGraphic.font.height * LINES_USE_COUNT + GlobalGraphic.font.height * LINES_USE_COUNT - 1,
                                            view->width,
                                            0x66000000);
            }
        }


        /* Line Number */
        char line_number_buffer[5] = { ' ', ' ', ' ', '0', '\0' };
        int einfach[10] = { 15 };
        s32 relativ_number = i - data->index - data->offset_y;
        if (relativ_number == 0)
            relativ_number = i;
        else if (relativ_number < 0)
            relativ_number = ~relativ_number + 1;
        u16 line_number_index = LINE_NUMBER_WIDTH_MAX;
        line_number_buffer[line_number_index] = 0;
        while (line_number_index-- && relativ_number != 0)
        {
            line_number_buffer[line_number_index] = relativ_number % 10 + 0x30;
            relativ_number /= 10;
        }
        view_render_text(view, x, y, (u8 *)line_number_buffer, CONFIG_COLOR_FOREGROUND);
        x += LINE_NUMBER_WIDTH;

        /* Title */
        // x += font_text_width_in_pixels(&GlobalGraphic.font, line_number_buffer) * 2;
        view_render_text(view, x, y, (u8 *)global_data.spoor_objects[data->spoor_objects_indexes[i]].title, CONFIG_COLOR_FOREGROUND);
        x += TITLE_WIDTH;


        /* deadline feature 1d, -1d */
        char time_span_format[22];
        //x += 425;
        //x += FONT_CHAR_WIDTH * 50;
        char time_offset_buffer[6] = { 0 };
        time_t today_sec = time(NULL);
        today_sec += 60 * 60; /* I don't know why wtf */
        struct tm test = { 0 };
        test.tm_mday = global_data.spoor_objects[data->spoor_objects_indexes[i]].deadline.end.day;
        test.tm_sec = 0;
        test.tm_mon = global_data.spoor_objects[data->spoor_objects_indexes[i]].deadline.end.mon - 1;
        test.tm_year = global_data.spoor_objects[data->spoor_objects_indexes[i]].deadline.end.year - 1900;
        test.tm_hour = 1; /* I don't know why wtf */
        time_t spoor_object_sec = mktime(&test);

        u32 time_color = 0xff007700;

        u32 diff_day = 0;
        char negativ_buffer = '+';

        u32 today_day = today_sec / 60 / 60 / 24;
        u32 spoor_object_day = spoor_object_sec / 60 / 60 / 24;

        if (x + TIME_DEADLINE_DAY_WIDTH + TIME_DEADLINE_WIDTH > view->x + view->width)
        {
            x = view->x + LINE_NUMBER_WIDTH;
            y += GlobalGraphic.font.height;
        }
        if (spoor_object_day != today_day)
        {
            if (spoor_object_sec > today_sec)
            {
                diff_day = spoor_object_day - today_day;
            }
            else
            {
                negativ_buffer = '-';
                time_color = 0xff770000;
                diff_day = today_day - spoor_object_day;
            }

            u32 off = 0;
            if (diff_day < 100)
                time_offset_buffer[off++] = ' ';
            if (diff_day < 10)
                time_offset_buffer[off++] = ' ';

            if (diff_day > 999)
            {
                u32 i;
                for (i = 0; time_offset_buffer[i] != 0; i++)
                    time_offset_buffer[i] = negativ_buffer;
                strcpy(time_offset_buffer, "+++++");
            }
            else
                sprintf(time_offset_buffer + off, "%c%id", negativ_buffer, diff_day);

            view_render_text(view, x, y, (u8 *)time_offset_buffer, time_color);
        }
        x += TIME_DEADLINE_DAY_WIDTH;


        /* deadline */
#if 0
        if (x + TIME_DEADLINE_WIDTH > view->x + view->width)
        {
            x = view->x + LINE_NUMBER_WIDTH;
            y += GlobalGraphic.font.height;
        }
#endif
		spoor_time_span_deadline_format_create(&global_data.spoor_objects[data->spoor_objects_indexes[i]].deadline,
                                               time_span_format);
        view_render_text(view, x, y, (u8 *)time_span_format, CONFIG_COLOR_FOREGROUND);
        x += TIME_DEADLINE_WIDTH;

        /* schedule */
        if (x + TIME_SCHEDULE_WIDTH > view->x + view->width)
        {
            x = view->x + LINE_NUMBER_WIDTH;
            y += GlobalGraphic.font.height;
        }
        spoor_time_span_schedule_format_create(&global_data.spoor_objects[data->spoor_objects_indexes[i]].schedule,
                                               time_span_format);
        view_render_text(view, x, y, (u8 *)time_span_format, CONFIG_COLOR_FOREGROUND);
        x += TIME_SCHEDULE_WIDTH;

        /* tracked */
        if (x + TIME_TRACKED_WIDTH > view->x + view->width)
        {
            x = view->x + LINE_NUMBER_WIDTH;
            y += GlobalGraphic.font.height;
        }
        spoor_time_span_schedule_format_create(&global_data.spoor_objects[data->spoor_objects_indexes[i]].tracked,
                                               time_span_format);
        view_render_text(view, x, y, (u8 *)time_span_format, CONFIG_COLOR_FOREGROUND);
        x += TIME_TRACKED_WIDTH;

        /* type */
        if (x + TYPE_WIDTH > view->x + view->width)
        {
            x = view->x + LINE_NUMBER_WIDTH;
            y += GlobalGraphic.font.height;
        }
        view_render_text(view, x, y, (u8 *)SPOOR_TYPES[global_data.spoor_objects[data->spoor_objects_indexes[i]].type], CONFIG_COLOR_FOREGROUND);
        x += TYPE_WIDTH;

        /* Priority */
        char buffer_priority[4];
        sprintf(buffer_priority, "%d", global_data.spoor_objects[data->spoor_objects_indexes[i]].priority);
        if (x + PRIORITY_WIDTH > view->x + view->width)
        {
            x = view->x + LINE_NUMBER_WIDTH;
            y += GlobalGraphic.font.height;
        }
        view_render_text(view, x, y, (u8 *)buffer_priority, CONFIG_COLOR_FOREGROUND);
        x += PRIORITY_WIDTH;

        /* status */
        if (x + STATUS_WIDTH > view->x + view->width)
        {
            x = view->x + LINE_NUMBER_WIDTH;
            y += GlobalGraphic.font.height;
        }
        view_render_text(view, x, y, (u8 *)SPOOR_STATUS[global_data.spoor_objects[data->spoor_objects_indexes[i]].status], SPOOR_STATUS_COLORS[global_data.spoor_objects[data->spoor_objects_indexes[i]].status]);
        x += STATUS_WIDTH;

        if (y >= view->y + view->height) 
            break;
    }

    /* Info Bar Render */

    const u16 INFO_BAR_HEIGHT = GlobalGraphic.font.height - (GlobalGraphic.font.face->size->metrics.descender >> 6);

    x = view->x;
    y = view->y + view->height - INFO_BAR_HEIGHT;

    /* Info Bar Background */
    render_rectangle_fill(x, y,
                          view->width, INFO_BAR_HEIGHT,
                          CONFIG(TASK_LIST_INFO_BAR_COLOR_BACKGROUND));

    y += INFO_BAR_HEIGHT + (GlobalGraphic.font.face->size->metrics.descender >> 6) - 1;
    x += 10;
    /* render filter information */
#if 0
    x += 20;
    // y += view->height - 2 * (GlobalGraphic.font.height - (GlobalGraphic.font.face->size->metrics.descender >> 6));
    y += GlobalGraphic.font.height + (GlobalGraphic.font.face->size->metrics.descender >> 6);
    INFO_BAR_HEIGHT += GlobalGraphic.font.height + (GlobalGraphic.font.face->size->metrics.descender >> 6);
#endif

    char filter_buffer[40];

    sprintf(filter_buffer, "Info -> T: [%c%c%c%c%c%c%c]  S: [%c%c%c]",
           (data->spoor_filter.type & SPOOR_FILTER_TYPE_TASK) ?'T' :'-',
           (data->spoor_filter.type & SPOOR_FILTER_TYPE_PROJECT) ?'P' :'-',
           (data->spoor_filter.type & SPOOR_FILTER_TYPE_EVENT) ?'E' :'-',
           (data->spoor_filter.type & SPOOR_FILTER_TYPE_APPOINTMENT) ?'A' :'-',
           (data->spoor_filter.type & SPOOR_FILTER_TYPE_GOAL) ?'G' :'-',
           (data->spoor_filter.type & SPOOR_FILTER_TYPE_HABIT) ?'H' :'-',
           (data->spoor_filter.type & SPOOR_FILTER_TYPE_IDEA) ?'I' :'-',
           (data->spoor_filter.status & SPOOR_FILTER_STATUS_NOT_STARTED) ?'N' :'-',
           (data->spoor_filter.status & SPOOR_FILTER_STATUS_IN_PROGRESS) ?'I' :'-',
           (data->spoor_filter.status & SPOOR_FILTER_STATUS_COMPLETED) ?'C' :'-');

    view_render_text(view, x, y, (u8 *)filter_buffer, CONFIG(TASK_LIST_INFO_BAR_COLOR_FOREGROUND));

#if 0
    y -= (GlobalGraphic.font.face->size->metrics.descender >> 6);
    INFO_BAR_HEIGHT -= (GlobalGraphic.font.face->size->metrics.descender >> 6);
#endif
}

void task_list_input_func(View *view, u8 key)
{
    TaskListData *task_list_data = (TaskListData *)view->data;

    if (GlobalGraphic.command_buffer.count >= 2)
    {
        if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, "dd", 2) == 0)
        {
            if (task_list_data->index != TASK_LIST_HAND_INDEX_DEFAULT)
            {
                printf("Delete task with index: %d\n", task_list_data->index);
                SpoorObject *spoor_objects = global_data.spoor_objects;
                SpoorObject *spoor_object = &spoor_objects[task_list_data->spoor_objects_indexes[task_list_data->index]];
                spoor_object_remove(spoor_object);
                spoor_objects_remove(spoor_objects,
                                     &global_data.spoor_objects_count,
                                     task_list_data->spoor_objects_indexes[task_list_data->index]);

#if 0
                global_data.indexes_sync.index = data->spoor_objects_indexes[data->index];
                global_data.indexes_sync.mode = SPOOR_OBJECTS_INDEXES_SYNC_EDIT;
#endif



                command_buffer_clear(&GlobalGraphic.command_buffer);
            }
        }
    }

    if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 1, "c", 1) == 0)
    {
        GlobalGraphic.mode = GRAPHIC_MODE_COMMAND_BUFFER;
        GlobalGraphic.command_buffer.buffer[0] = ':';
        GlobalGraphic.command_buffer.buffer[1] = 'c';
        GlobalGraphic.command_buffer.buffer[2] = 0;
        GlobalGraphic.command_buffer.count = 2;
    }
    else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 1, "e", 1) == 0)
    {
        GlobalGraphic.mode = GRAPHIC_MODE_COMMAND_BUFFER;
        GlobalGraphic.command_buffer.buffer[0] = ':';
        GlobalGraphic.command_buffer.buffer[1] = 'e';
        GlobalGraphic.command_buffer.buffer[2] = 0;
        GlobalGraphic.command_buffer.count = 2;
    }
    else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 1, "f", 1) == 0)
    {
        GlobalGraphic.mode = GRAPHIC_MODE_COMMAND_BUFFER;
        GlobalGraphic.command_buffer.buffer[0] = ':';
        GlobalGraphic.command_buffer.buffer[1] = 'f';
        GlobalGraphic.command_buffer.buffer[2] = 0;
        GlobalGraphic.command_buffer.count = 2;
    }

    switch (key)
    {
        case SPOOR_INPUT_ASCII_KEY_CONTROL_U:
        {
            if (task_list_data->offset_y > 0)
                task_list_data->offset_y -= 1;
        } break;
        case SPOOR_INPUT_ASCII_KEY_CONTROL_D:
        {
            task_list_data->offset_y += 1;
        } break;
        case 'n':
        {
            u32 counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 1);
            task_list_data->index_last = task_list_data->index;
            task_list_data->index += counter;
        } break;
        case 'r':
        {
            u32 counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 1);
            if (task_list_data->index >= counter)
            {
                task_list_data->index_last = task_list_data->index;
                task_list_data->index -= counter;
            }
        } break;
        case '+':
        {
            u32 counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 1);
            SpoorObject *spoor_object = &global_data.spoor_objects[task_list_data->spoor_objects_indexes[task_list_data->index]];
            spoor_object->priority += counter;
            spoor_object_save(spoor_object);
        } break;
        case '-':
        {
            u32 counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 1);
            SpoorObject *spoor_object = &global_data.spoor_objects[task_list_data->spoor_objects_indexes[task_list_data->index]];
            if (spoor_object->priority >= counter)
                spoor_object->priority -= counter;
            spoor_object_save(spoor_object);
        } break;

        /* test projekt join thing */
        case 'g':
        {
            u32 *childs_indexes = &task_list_data->selection;
            u32 *childs_count = &task_list_data->selection_count;
            u32 parent_index = task_list_data->index;

            /* parent */
            SpoorObject *spoor_object_parent = &global_data.spoor_objects[task_list_data->spoor_objects_indexes[parent_index]];
            spoor_object_parent->flag &= SPOOR_OBJECT_FLAG_PARENT;

            spoor_object_parent_save(spoor_object_parent);


            /* child's */
            SpoorObject *spoor_object_child = NULL;
            u32 i;
            for (i = 0; i < *childs_count; i++)
            {
                spoor_object_child = &global_data.spoor_objects[task_list_data->spoor_objects_indexes[childs_indexes[i]];
                spoor_object_child_save(spoor_object_child);
            }

            SpoorObject *spoor_object_child = NULL;

            /* clear selection */
            *childs_count = 0;
        } break;
        case 'V':
        {
        } break;
        case 'v':
        {
            u32 *selection = &task_list_data->selection;
            u32 *count = &task_list_data->selection_count;
            u32 index = task_list_data->index;

            u32 i;
            for (i = 0; i < *count; i++)
            {
                if (index == selection[i])
                    break;
            }

            if (i >= *count)
            {
                selection[*count] = index;
                (*count)++;
            }
            else
            {
                memcpy(selection + i, selection + i + 1, ((*count - 1) - i) * sizeof(*selection));
                (*count)--;
            }

        } break;
    }
}
