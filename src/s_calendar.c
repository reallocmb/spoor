#include"spoor.h"

const char *DAYS_NAME[7] = {
    "Sunday",
    "Monday",
    "Thuesday",
    "Wednesday",
    "Thursday", 
    "Friday",
    "Saturday",
};

void calendar_render_func(View *view)
{
    CalendarData *data = (CalendarData *)view->data;

    u32 spoor_objects_count = spoor_object_load(&data->spoor_objects);

    time_t today_sec = time(NULL);
    SpoorTime today = *(SpoorTime *)localtime(&today_sec);
    today_sec += data->date_offset * DAY_SECONDS;

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

    u16 i, j;
    for (j = 0; j < data->days_count; j++)
    {
        u16 y = DAY_Y;
        u16 day_width = DAY_WIDTH + ((day_rest-- > 0) ?1 :0);
        printf("day_width[%d]: %d\n", j, day_width);

        struct tm *today_date = localtime(&today_sec);
        
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

        today_sec += DAY_SECONDS;
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
                render_line_horizontal(x + 55, y, day_width - 55, 0x88555555);
        }
        x += day_width;
    }

    y = DAY_Y;

    /* render spoor objects */
    u32 k;
    printf("test cound: %d\n", spoor_objects_count);
    for (k = 0; k < spoor_objects_count; k++)
    {
        printf("x: %d, y: %d\n", x, y);
        view_render_text(view, x + 50, y + 50, (u8 *)data->spoor_objects[k].title, CONFIG_COLOR_FOREGROUND);
        view_render_text(view, x, y, (u8 *)"Test", CONFIG_COLOR_FOREGROUND);
    }

}

void calendar_input_func(View *view, u8 key)
{
    CalendarData *calendar_data = (CalendarData *)view->data;

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
        case 'a': calendar_data->date_offset--; break;
        case 'n': calendar_data->time_offset++; break;
        case 'r': if (calendar_data->time_offset != 0) { calendar_data->time_offset--; } break;
        case 'e': calendar_data->date_offset++; break;
    }

    graphic_update();
}
