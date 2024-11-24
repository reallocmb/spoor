#include"spoor.h" /* for completion */

void task_list_render_func(View *view)
{
    TaskListData *task_list_data = (TaskListData *)view->data;

    u32 spoor_objects_count = spoor_object_load(&task_list_data->spoor_objects);
    spoor_object_sort(task_list_data->spoor_objects, spoor_objects_count);

    if (spoor_objects_count == 0)
        task_list_data->hand_index = TASK_LIST_HAND_INDEX_DEFAULT;

    u16 x = view->x;
    u16 y = view->y;
#if 0
    u16 width = view->width;
    u16 height = view->height;
#endif

    u32 size = GlobalGraphic.font.size;
    font_size_set(&GlobalGraphic.font, 15);

    /* hand highlight */
    if (task_list_data->hand_index >= spoor_objects_count)
        task_list_data->hand_index = spoor_objects_count - 1;
    render_rectangle_fill(x, y + task_list_data->hand_index * GlobalGraphic.font.height,
                          view->width, GlobalGraphic.font.height,
                          CONFIG_COLOR_STATUS_BAR_BACKGROUND);

    y += GlobalGraphic.font.height;
    x += 30;

    u16 x_backup;
    u32 i;
    for (i = 0; i < spoor_objects_count; i++, x = x_backup)
    {
        x_backup = x;
        char buffer_i[5];
        sprintf(buffer_i, "%i", i);
        view_render_text(view, x, y, (u8 *)buffer_i, CONFIG_COLOR_FOREGROUND);
        x += 20;
        view_render_text(view, x, y, (u8 *)task_list_data->spoor_objects[i].title, CONFIG_COLOR_FOREGROUND);
        char time_span_format[22];

        /* deadline */
		spoor_time_span_deadline_format_create(&task_list_data->spoor_objects[i].deadline,
											   time_span_format);
        x += 340;
        view_render_text(view, x, y, (u8 *)time_span_format, CONFIG_COLOR_FOREGROUND);

        /* schedule */
        spoor_time_span_schedule_format_create(&task_list_data->spoor_objects[i].schedule,
                                               time_span_format);
        x += 150;
        view_render_text(view, x, y, (u8 *)time_span_format, CONFIG_COLOR_FOREGROUND);

        /* tracked */
        spoor_time_span_schedule_format_create(&task_list_data->spoor_objects[i].tracked,
                                               time_span_format);
        x += 210;
        view_render_text(view, x, y, (u8 *)time_span_format, CONFIG_COLOR_FOREGROUND);

        /* type */
        x += 210;
        view_render_text(view, x, y, (u8 *)SPOOR_TYPES[task_list_data->spoor_objects[i].type], CONFIG_COLOR_FOREGROUND);

        /* status */
        x += 80;
        view_render_text(view, x, y, (u8 *)SPOOR_STATUS[task_list_data->spoor_objects[i].status], CONFIG_COLOR_STATUS[task_list_data->spoor_objects[i].status]);

        y += GlobalGraphic.font.height;
    }

    font_size_set(&GlobalGraphic.font, size);
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
    else if(strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 1, "e", 1) == 0)
    {
        GlobalGraphic.mode = GRAPHIC_MODE_COMMAND_BUFFER;
        GlobalGraphic.command_buffer.buffer[0] = ':';
        GlobalGraphic.command_buffer.buffer[1] = 'e';
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
