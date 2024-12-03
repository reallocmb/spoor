#include"spoor.h" /* for completion */

void task_list_render_func(View *view)
{
    TaskListData *task_list_data = (TaskListData *)view->data;

    u32 spoor_objects_count = spoor_object_load(&task_list_data->spoor_objects);
    spoor_object_sort(task_list_data->spoor_objects, spoor_objects_count);
    spoor_filter_set(&task_list_data->spoor_filter, task_list_data->spoor_objects, &spoor_objects_count);

    if (spoor_objects_count == 0)
        task_list_data->hand_index = TASK_LIST_HAND_INDEX_DEFAULT;

    u16 x = view->x;
    u16 y = view->y;

    /* hand highlight */
    if (task_list_data->hand_index >= spoor_objects_count)
        task_list_data->hand_index = spoor_objects_count - 1;
    render_rectangle_fill(x, y + task_list_data->hand_index * GlobalGraphic.font.height,
                          view->width, GlobalGraphic.font.height,
                          CONFIG_COLOR_STATUS_BAR_BACKGROUND);

    y += GlobalGraphic.font.height + (GlobalGraphic.font.face->size->metrics.descender >> 6);

    u32 i;
    for (i = 0; i < spoor_objects_count; i++)
    {
        /* spoor_filter_set is invented 
        if (!(task_list_data->spoor_filter.type & SPOOR_FLITER_TYPES_BITS[task_list_data->spoor_objects[i].type]))
            continue;
        if (!(task_list_data->spoor_filter.status & SPOOR_FILTER_STATUS_BITS[task_list_data->spoor_objects[i].status]))
            continue;
            */

        x = view->x + 10;

        char buffer_i[5];
        sprintf(buffer_i, "%i", i);
        view_render_text(view, x, y, (u8 *)buffer_i, CONFIG_COLOR_FOREGROUND);

        x += 20;
        view_render_text(view, x, y, (u8 *)task_list_data->spoor_objects[i].title, CONFIG_COLOR_FOREGROUND);
        char time_span_format[22];

        /* deadline feature */
        x += 325;
        char time_offset_buffer[6] = { 0 };
        time_t today_sec = time(NULL);
        struct tm test = { 0 };
        test.tm_mday = task_list_data->spoor_objects[i].deadline.end.day;
        test.tm_mon = task_list_data->spoor_objects[i].deadline.end.mon - 1;
        test.tm_year = task_list_data->spoor_objects[i].deadline.end.year - 1900;
        struct tm loc = *localtime(&today_sec);
        time_t spoor_object_sec = mktime(&test);

        u32 time_color = 0xff00ff00;

        u32 diff_day = 0;
        char negativ_buffer = '+';

        u32 today_day = today_sec / 60 / 60 / 24;
        u32 spoor_object_day = spoor_object_sec / 60 / 60 / 24 + 1;

        if (spoor_object_day != today_day)
        {
            if (spoor_object_sec > today_sec)
            {
                diff_day = spoor_object_day - today_day;
            }
            else
            {
                negativ_buffer = '-';
                time_color = 0xffff0000;
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


        /* deadline */
        x += 50;
		spoor_time_span_deadline_format_create(&task_list_data->spoor_objects[i].deadline,
											   time_span_format);
        view_render_text(view, x, y, (u8 *)time_span_format, CONFIG_COLOR_FOREGROUND);

        /* schedule */
        x += 150;
        spoor_time_span_schedule_format_create(&task_list_data->spoor_objects[i].schedule,
                                               time_span_format);
        view_render_text(view, x, y, (u8 *)time_span_format, CONFIG_COLOR_FOREGROUND);

        /* tracked */
        x += 210;
        spoor_time_span_schedule_format_create(&task_list_data->spoor_objects[i].tracked,
                                               time_span_format);
        view_render_text(view, x, y, (u8 *)time_span_format, CONFIG_COLOR_FOREGROUND);

        /* type */
        x += 210;
        view_render_text(view, x, y, (u8 *)SPOOR_TYPES[task_list_data->spoor_objects[i].type], CONFIG_COLOR_FOREGROUND);

        /* status */
        x += 80;
        view_render_text(view, x, y, (u8 *)SPOOR_STATUS[task_list_data->spoor_objects[i].status], CONFIG_COLOR_STATUS[task_list_data->spoor_objects[i].status]);

        y += GlobalGraphic.font.height;
    }

    x = view->x;
    y = view->y;

    /* render filter information */
    x += view->width - 180;
    y += view->height - 2 * (GlobalGraphic.font.height - (GlobalGraphic.font.face->size->metrics.descender >> 6));

    char filter_buffer[8 + 8 + 6];


    sprintf(filter_buffer, "Filter Types: %c%c%c%c%c%c%c",
           (task_list_data->spoor_filter.type & SPOOR_FILTER_TYPE_TASK) ?'T' :'-',
           (task_list_data->spoor_filter.type & SPOOR_FILTER_TYPE_PROJECT) ?'P' :'-',
           (task_list_data->spoor_filter.type & SPOOR_FILTER_TYPE_EVENT) ?'E' :'-',
           (task_list_data->spoor_filter.type & SPOOR_FILTER_TYPE_APPOINTMENT) ?'A' :'-',
           (task_list_data->spoor_filter.type & SPOOR_FILTER_TYPE_GOAL) ?'G' :'-',
           (task_list_data->spoor_filter.type & SPOOR_FILTER_TYPE_HABIT) ?'H' :'-',
           (task_list_data->spoor_filter.type & SPOOR_FILTER_TYPE_IDEA) ?'I' :'-');

    view_render_text(view, x, y, (u8 *)filter_buffer, CONFIG_COLOR_FOREGROUND);

    y += GlobalGraphic.font.height - (GlobalGraphic.font.face->size->metrics.descender >> 6);
    sprintf(filter_buffer, "Filter Status: %c%c%c",
           (task_list_data->spoor_filter.status & SPOOR_FILTER_STATUS_NOT_STARTED) ?'N' :'-',
           (task_list_data->spoor_filter.status & SPOOR_FILTER_STATUS_IN_PROGRESS) ?'I' :'-',
           (task_list_data->spoor_filter.status & SPOOR_FILTER_STATUS_COMPLETED) ?'C' :'-');

    view_render_text(view, x, y, (u8 *)filter_buffer, CONFIG_COLOR_FOREGROUND);
}

void task_list_input_func(View *view, u8 key)
{
    TaskListData *task_list_data = (TaskListData *)view->data;

    if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 2, "dd", 2) == 0)
    {
        if (task_list_data->hand_index != TASK_LIST_HAND_INDEX_DEFAULT)
        {
            printf("Delete task with index: %d\n", task_list_data->hand_index);
            SpoorObject *spoor_objects = ((TaskListData *)view->data)->spoor_objects;
            SpoorObject *spoor_object = &spoor_objects[task_list_data->hand_index];
            spoor_object_remove(spoor_object);
            command_buffer_clear(&GlobalGraphic.command_buffer);
        }
    }
    else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 1, "c", 1) == 0)
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
        case 'n': task_list_data->hand_index++; break;
        case 'r': if (task_list_data->hand_index != 0) { task_list_data->hand_index--; } break;
    }

    graphic_update();
}
