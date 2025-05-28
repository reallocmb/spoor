#include"spoor.h" /* compile_commands */

const char *DAYS_NAME[7] = {
    "SUNDAY",
    "MONDAY",
    "THUESDAY",
    "WEDNESDAY",
    "THURSDAY", 
    "FRIDAY",
    "SATURDAY",
};

void calendar_render_func(View *view)
{
    CalendarData *data = (CalendarData *)view->data;

    /* today */
    time_t today_sec = time(NULL);
    const time_t TODAY_SEC = today_sec;
    SpoorTime today = *(SpoorTime *)localtime(&today_sec);
    today.mon++;
    today.year += 1900;

    today_sec += data->date_offset * DAY_SECONDS;

    time_t time_start_sec = TODAY_SEC + data->date_offset * DAY_SECONDS;
    time_t time_end_sec = TODAY_SEC + data->days_count * DAY_SECONDS;
    SpoorTime time_start = *(SpoorTime *)localtime(&time_start_sec);
    time_start.hour = 0;
    time_start.min = 0;
    time_start.mon++;
    time_start.year += 1900;
    SpoorTime time_end = *(SpoorTime *)localtime(&time_end_sec);
    time_end.hour = 23;
    time_end.mon = 59;
    time_end.mon++;
    time_end.year += 1900;

    /* filter flags */
    data->spoor_filter.flags |= SPOOR_FILTER_FLAG_SCHEDULE;
    data->spoor_filter.schedule.start = time_start;
    data->spoor_filter.schedule.end = time_end;

    if (global_data.spoor_objects_count == 0)
        spoor_objects_load(global_data.spoor_objects,
                           &global_data.spoor_objects_count,
                           NULL,
                           NULL);


#if 1
    data->spoor_objects_indexes_count = 0;
    u32 spoor_objects_index_backup = data->spoor_objects_indexes[data->index];
    spoor_objects_indexes_set(data->spoor_objects_indexes,
                              &data->spoor_objects_indexes_count,
                              &data->spoor_filter,
                              sort_calendar_func,
                              global_data.spoor_objects,
                              global_data.spoor_objects_count);

    u32 i;
    for (i = 0; i < data->spoor_objects_indexes_count; i++)
    {
        if (spoor_objects_index_backup == data->spoor_objects_indexes[i])
        {
            data->index = i;
            break;
        }
    }


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

    u16 x = view->x;
    u16 y = view->y;

    const u16 HEADER_HEIGHT = GlobalGraphic.font.height + 20;

    /* header */
    render_rectangle_fill(x, y, view->width, HEADER_HEIGHT, 0x55646464);
    y += HEADER_HEIGHT;

    const u16 DAY_HEIGHT = view->height - HEADER_HEIGHT;
    const u16 DAY_Y = y;

    const u16 DAY_WIDTH = view->width / data->days_count;
    s16 day_rest = view->width % data->days_count;
    printf("view->height: %d\n", view->height);
    printf("day_width: %d\n", DAY_WIDTH);
    printf("day_rest: %d\n", day_rest);

    const u16 text_day_y = DAY_Y - GlobalGraphic.font.height + (GlobalGraphic.font.face->size->metrics.descender >> 6);
    const u16 text_date_y = DAY_Y + (GlobalGraphic.font.face->size->metrics.descender >> 6);

    u32 j;
    u32 schedule_i = 0;
    for (j = 0; j < data->days_count; j++, today_sec += DAY_SECONDS)
    {
        u16 y = DAY_Y;
        u16 day_width = DAY_WIDTH + ((day_rest-- > 0) ?1 :0);
        printf("day_width[%d]: %d\n", j, day_width);

        struct tm *today_date = localtime(&today_sec);
#if 1
        today_date->tm_mon++;
        today_date->tm_year += 1900;
#endif
        
        /* week days */
        char today_date_buffer[11];
        spoor_time_date_format_create((SpoorTime *)today_date, today_date_buffer);
        if (time_compare(&today, (SpoorTime *)today_date) == 0)
        {
            view_render_text(view, x + 10, text_day_y, (u8 *)DAYS_NAME[today_date->tm_wday], 0xccffffff);
            view_render_text(view, x + 10, text_date_y, (u8 *)today_date_buffer, 0xccffffff);
        }
        else
        {
            view_render_text(view, x + 10, text_day_y, (u8 *)DAYS_NAME[today_date->tm_wday], CONFIG_COLOR_FOREGROUND);
            view_render_text(view, x + 10, text_date_y, (u8 *)today_date_buffer, CONFIG_COLOR_FOREGROUND);
        }
        /* date */

        /*
        Noday_sec += DAY_SECONDS;
        */
        //render_line_horizontal(x, y, view->width, CONFIG_COLOR_FOREGROUND);
        
        u16 day_height_limit = DAY_HEIGHT;
        if ((24 - data->time_offset) * 60 < DAY_HEIGHT)
            day_height_limit = (24 - data->time_offset) * 60;
        
        /* seperation line */
        render_line_vertical(x, y, DAY_HEIGHT * (j > 0), CONFIG_COLOR_FOREGROUND);
        for (i = 0; i < day_height_limit; i++, y++)
        {
            if (i % 60 == 0)
            {
                char time_buffer[6];
                sprintf(time_buffer, "%s%i:00", ((i / 60) + data->time_offset  > 9) ?"" :"0", (i / 60) + data->time_offset);
                view_render_text(view, x + 5, y + GlobalGraphic.font.height, (u8 *)time_buffer, CONFIG_COLOR_FOREGROUND);
                render_line_horizontal(x, y, day_width, 0xeeeeeeee);
            }
            else if (i % 30 == 0)
            {
                render_line_horizontal(x + 40, y, day_width - 40,  0xaaeeeeee);
            }
            else if (i % 15 == 0)
            {
                render_line_horizontal(x + 50, y, day_width - 50, 0x66eeeeee);
            }
            else if (i % 5 == 0)
                render_line_horizontal(x + 55, y, day_width - 55, 0x88888888);
        }
        /* spoor_objects schedule */
        u16 spoor_object_y = DAY_Y;
        for (schedule_i = 0; schedule_i < data->spoor_objects_indexes_count; schedule_i++)
        {
            SpoorObject *spoor_object = &global_data.spoor_objects[data->spoor_objects_indexes[schedule_i]];

            if ((u32)spoor_object->schedule.start.year == SPOOR_TIME_DEFAULT_VALUE)
                continue;
            if (time_compare_day_only(&spoor_object->schedule.end, (SpoorTime *)today_date) != 0)
                continue;

            s32 rect_y = spoor_object_y + (spoor_object->schedule.start.hour - data->time_offset) * 60 + spoor_object->schedule.start.min;
            s32 rect_y_end = spoor_object_y + (spoor_object->schedule.end.hour - data->time_offset) * 60 + spoor_object->schedule.end.min; 

            if (rect_y < DAY_Y)
            {
                if (rect_y_end > DAY_Y)
                    rect_y = DAY_Y;
                else
                    continue;
            }

            if (rect_y > view->height + view->y)
                continue;


            u32 height = 0;
            if (rect_y_end > view->height + view->y)
                height = view->height + view->y - rect_y;
            else
                height = rect_y_end - rect_y;


            u32 spoor_object_color = CALENDAR_OBJECT_COLORS[spoor_object->type];


            if (spoor_object->status == SPOOR_STATUS_COMPLETED)
            {
                spoor_object_color ^= 0x00ffffff;
                spoor_object_color &= 0xff225522;
            }

            render_rectangle_fill(x + 60, rect_y, DAY_WIDTH - 65, height, spoor_object_color);
            u32 color_outline = CONFIG(CALENDAR_OBJECT_OUTLINE_COLOR);
            if (schedule_i == data->index)
                if (data->is_grab)
                    color_outline = CONFIG(CALENDAR_OBJECT_OUTLINE_GRABED_COLOR);
                else
                    color_outline = CONFIG(CALENDAR_OBJECT_OUTLINE_MARKED_COLOR);

            render_rectangle(x + 60, rect_y, DAY_WIDTH - 65, height, color_outline);
            render_rectangle(x + 60 + 1, rect_y + 1, DAY_WIDTH - 65 - 2, height - 2, color_outline);

            view_render_text(view, x + 65, rect_y + GlobalGraphic.font.height, (u8 *)spoor_object->title, CONFIG_COLOR_FOREGROUND);
        }

        x += day_width;
    }

    /* debugging index */
    char index_buffer[50];
    sprintf(index_buffer, "Index: %d", data->index);
    view_render_text(view,
                     view->width - 100,
                     view->height - 50,
                     index_buffer,
                     0xff000000);



#if 0
    y = DAY_Y;
    x = view->x;
    x += 100;

    /* render spoor objects */
    u32 k;
    printf("test cound: %d\n", global_data.spoor_objects_count);
    for (k = 0; k < global_data.spoor_objects_count; k++)
    {
        printf("x: %d, y: %d\n", x, y);
        view_render_text(view, x + 50, y + 50, (u8 *)data->spoor_objects[k].title, CONFIG_COLOR_FOREGROUND);
        view_render_text(view, x, y, (u8 *)"Test", CONFIG_COLOR_FOREGROUND);
    }
#endif
}

void calendar_input_func(View *view, u8 key)
{
    CalendarData *calendar_data = (CalendarData *)view->data;
    u32 counter;

    if (calendar_data->is_grab)
    {
        switch (key)
        {
            case 'v':
            {
                calendar_data->is_grab = false;
            } break;
            case 'n':
            {
                counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 5);
                SpoorObject *spoor_object = &global_data.spoor_objects[calendar_data->spoor_objects_indexes[calendar_data->index]];
                time_minutes_add(&spoor_object->schedule.start, counter);
                time_minutes_add(&spoor_object->schedule.end, counter);
                spoor_object_save(spoor_object);
                /*
                spoor_objects_reorder(global_data.spoor_objects,
                                      global_data.spoor_objects_count,
                                      calendar_data->spoor_objects_indexes[calendar_data->index]);
                                      */
            } break;
            case 'r':
            {
                counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 5);
                SpoorObject *spoor_object = &global_data.spoor_objects[calendar_data->spoor_objects_indexes[calendar_data->index]];
                time_minutes_add(&spoor_object->schedule.start, -counter);
                time_minutes_add(&spoor_object->schedule.end, -counter);
                spoor_object_save(spoor_object);
                /*
                spoor_objects_reorder(global_data.spoor_objects,
                                      global_data.spoor_objects_count,
                                      calendar_data->spoor_objects_indexes[calendar_data->index]);
                                      */
            } break;
            case 's':
            {
                counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 1);
                SpoorObject *spoor_object = &global_data.spoor_objects[calendar_data->spoor_objects_indexes[calendar_data->index]];
                time_days_add(&spoor_object->schedule.start, -counter);
                time_days_add(&spoor_object->schedule.end, -counter);
                spoor_object_save(spoor_object);
                /*
                spoor_objects_reorder(global_data.spoor_objects,
                                                  global_data.spoor_objects_count,
                                                  calendar_data->spoor_objects_indexes[calendar_data->index]);
                                                  */
            } break;
            case 't':
            {
                counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 1);
                SpoorObject *spoor_object = &global_data.spoor_objects[calendar_data->spoor_objects_indexes[calendar_data->index]];
                time_days_add(&spoor_object->schedule.start, counter);
                time_days_add(&spoor_object->schedule.end, counter);
                spoor_object_save(spoor_object);
                /*
                spoor_objects_reorder(global_data.spoor_objects,
                                                  global_data.spoor_objects_count,
                                                  calendar_data->spoor_objects_indexes[calendar_data->index]);
                                                  */
            } break;

            case 'N':
            {
                counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 10);
                SpoorObject *spoor_object = &global_data.spoor_objects[calendar_data->spoor_objects_indexes[calendar_data->index]];
                time_minutes_add(&spoor_object->schedule.end, counter);
                spoor_object_save(spoor_object);
                /*
                spoor_objects_reorder(global_data.spoor_objects,
                                                  global_data.spoor_objects_count,
                                                  calendar_data->spoor_objects_indexes[calendar_data->index]);
                                                  */
            } break;
            case 'R':
            {
                counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 10);
                SpoorObject *spoor_object = &global_data.spoor_objects[calendar_data->spoor_objects_indexes[calendar_data->index]];
                time_minutes_add(&spoor_object->schedule.end, -counter);
                spoor_object_save(spoor_object);
                /*
                spoor_objects_reorder(global_data.spoor_objects,
                                                  global_data.spoor_objects_count,
                                                  calendar_data->spoor_objects_indexes[calendar_data->index]);
                                                  */
            } break;
        }
    }
    else
    {
        if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 1, "+", 1) == 0)
        {
            calendar_data->days_count++;
        }
        else if (strncmp((char *)GlobalGraphic.command_buffer.buffer + GlobalGraphic.command_buffer.count - 1, "-", 1) == 0)
        {
            calendar_data->days_count--;
        }

        switch (key)
        {
            case 'v':
            {
                calendar_data->is_grab = true;
            } break;
            case 'N':
            {
                calendar_data->time_offset++;
            } break;
            case 'R':
            {
                if (calendar_data->time_offset != 0)
                {
                    calendar_data->time_offset--;
                }
            } break;
            case 'a':
            {
                u32 counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 1);
                calendar_data->date_offset -= counter;
            } break;
            case 'e':
            {
                u32 counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 1);
                calendar_data->date_offset += counter;
            } break;
            case 'n':
            {
                u32 counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 1);
                if (calendar_data->index < calendar_data->spoor_objects_indexes_count - 1)
                    calendar_data->index += counter;
            } break;
            case 'r':
            {
                u32 counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 1);
                if (calendar_data->index >= counter)
                    calendar_data->index -= counter;
            } break;
            case 's':
            {
                u32 counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 1);

                u32 index = calendar_data->index;

                SpoorObject *spoor_object = NULL;
                SpoorObject *spoor_object_current = &global_data.spoor_objects[calendar_data->spoor_objects_indexes[calendar_data->index]];

                /* get next day spoor object */
                if (index == 0)
                    break;

                index--;
                while (index != (u32)-1 &&
                       (spoor_object = &global_data.spoor_objects[calendar_data->spoor_objects_indexes[index]])->schedule.start.day == spoor_object_current->schedule.start.day)
                {
                    index--;
                }

                u32 day = spoor_object->schedule.start.day;

                u32 spoor_object_current_time = spoor_object_current->schedule.start.hour * 60 + spoor_object_current->schedule.start.min;
                u32 spoor_object_time = spoor_object->schedule.start.hour * 60 + spoor_object->schedule.start.min;

                if (spoor_object_current_time < spoor_object_time)
                {
                    index--;
                    while (index != (u32)-1 &&
                           (spoor_object = &global_data.spoor_objects[calendar_data->spoor_objects_indexes[index]])->schedule.start.day != spoor_object_current->schedule.start.day)
                    {
                        if (spoor_object->schedule.start.day != day)
                        {
                            index++;
                            break;
                        }
                        u32 time = spoor_object->schedule.start.hour * 60 + spoor_object->schedule.start.min;

                        if (time < spoor_object_current_time)
                        {
                            if (spoor_object_current_time - time > spoor_object_time - spoor_object_current_time)
                            {
                                index++;
                            }
                            break;
                        }
                        spoor_object_time = time;
                        index--;
                    }
                }



                if (index == (u32)-1)
                    index = 0;

                calendar_data->index = index;



#if 0
                if (calendar_data->index >= counter)
                    calendar_data->index -= counter;
#endif
            } break;
            case 't':
            {
                u32 counter = command_buffer_counter_detect(&GlobalGraphic.command_buffer, 1);


                u32 index = calendar_data->index;

                SpoorObject *spoor_object = NULL;
                SpoorObject *spoor_object_current = &global_data.spoor_objects[calendar_data->spoor_objects_indexes[calendar_data->index]];

                /* get next day spoor object */
                index++;
                while (index < calendar_data->spoor_objects_indexes_count &&
                       (spoor_object = &global_data.spoor_objects[calendar_data->spoor_objects_indexes[index]])->schedule.start.day == spoor_object_current->schedule.start.day)
                {
                    index++;
                }

                u32 day = spoor_object->schedule.start.day;

                u32 spoor_object_current_time = spoor_object_current->schedule.start.hour * 60 + spoor_object_current->schedule.start.min;
                u32 spoor_object_time = spoor_object->schedule.start.hour * 60 + spoor_object->schedule.start.min;

                if (spoor_object_current_time > spoor_object_time)
                {
                    index++;
                    while (index < calendar_data->spoor_objects_indexes_count &&
                           (spoor_object = &global_data.spoor_objects[calendar_data->spoor_objects_indexes[index]])->schedule.start.day != spoor_object_current->schedule.start.day)
                    {
                        if (spoor_object->schedule.start.day != day)
                        {
                            index--;
                            break;
                        }
                        u32 time = spoor_object->schedule.start.hour * 60 + spoor_object->schedule.start.min;

                        if (time > spoor_object_current_time)
                        {
                            if (time - spoor_object_current_time >= spoor_object_current_time - spoor_object_time)
                            {
                                index--;
                            }
                            break;
                        }

                        spoor_object_time = time;
                        index++;
                    }
                }

                if (index >= calendar_data->spoor_objects_indexes_count)
                    index = calendar_data->spoor_objects_indexes_count - 1;

                calendar_data->index = index;

#if 0
                if (calendar_data->index < calendar_data->spoor_objects_indexes_count - 1)
                    calendar_data->index += counter;
#endif
            } break;
        }
    }

    graphic_update();
}
