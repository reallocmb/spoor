#include"spoor.h"

#include<time.h>

void spoor_time_format_append(char *buffer, u32 *buffer_i, s32 value, u32 digit_count)
{
    if (value == (s32)SPOOR_TIME_DEFAULT_VALUE)
    {
        while (digit_count--)
            buffer[(*buffer_i)++] = '-';
    }
    else
    {
        u32 digit;
        u32 count = digit_count;
        while (count--)
        {
            digit = value % 10;
            value /= 10;

            buffer[*buffer_i + count] = digit + 0x30;
        }

        *buffer_i += digit_count;
    }
}

void spoor_time_date_format_create(SpoorTime *spoor_time, char *buffer) 
{
    u32 buffer_i = 0;
    spoor_time_format_append(buffer, &buffer_i, spoor_time->day, 2);
    buffer[buffer_i++] = '.';
    spoor_time_format_append(buffer, &buffer_i, spoor_time->mon, 2);
    buffer[buffer_i++] = '.';
    spoor_time_format_append(buffer, &buffer_i, spoor_time->year, 4);
    buffer[buffer_i] = 0;
}

/* buffer 17 bytes */
void spoor_time_span_deadline_format_create(SpoorTimeSpan *spoor_time_span, char *buffer)
{
    u32 buffer_i = 0;
    spoor_time_format_append(buffer, &buffer_i, spoor_time_span->start.day, 2);
    buffer[buffer_i++] = '.';
    spoor_time_format_append(buffer, &buffer_i, spoor_time_span->start.mon, 2);
    buffer[buffer_i++] = '.';
    spoor_time_format_append(buffer, &buffer_i, spoor_time_span->start.year, 4);
    buffer[buffer_i++] = ' ';
    spoor_time_format_append(buffer, &buffer_i, spoor_time_span->start.hour, 2);
    buffer[buffer_i++] = ':';
    spoor_time_format_append(buffer, &buffer_i, spoor_time_span->start.min, 2);
    buffer[buffer_i] = 0;
}

void spoor_time_span_schedule_format_create(SpoorTimeSpan *spoor_time_span, char *buffer)
{
    u32 buffer_i = 0;
    spoor_time_format_append(buffer, &buffer_i, spoor_time_span->end.day, 2);
    buffer[buffer_i++] = '.';
    spoor_time_format_append(buffer, &buffer_i, spoor_time_span->end.mon, 2);
    buffer[buffer_i++] = '.';
    spoor_time_format_append(buffer, &buffer_i, spoor_time_span->end.year, 4);
    buffer[buffer_i++] = ' ';
    spoor_time_format_append(buffer, &buffer_i, spoor_time_span->start.hour, 2);
    buffer[buffer_i++] = ':';
    spoor_time_format_append(buffer, &buffer_i, spoor_time_span->start.min, 2);
    buffer[buffer_i++] = ' ';
    spoor_time_format_append(buffer, &buffer_i, spoor_time_span->end.hour, 2);
    buffer[buffer_i++] = ':';
    spoor_time_format_append(buffer, &buffer_i, spoor_time_span->end.min, 2);
    buffer[buffer_i] = 0;
}

#if 1
void spoor_time_span_print(SpoorTimeSpan *spoor_time_span, char *info)
{
    printf("%s: %d:%d:%d %d.%d.%d - %d:%d:%d %d.%d.%d\n",
           info,
           spoor_time_span->start.hour,
           spoor_time_span->start.min,
           spoor_time_span->start.sec,
           spoor_time_span->start.day,
           spoor_time_span->start.mon,
           spoor_time_span->start.year,
           spoor_time_span->end.hour,
           spoor_time_span->end.min,
           spoor_time_span->end.sec,
           spoor_time_span->end.day,
           spoor_time_span->end.mon,
           spoor_time_span->end.year);
}
#endif

enum {
    TIME_FORMAT_ERROR,
    TIME_FORMAT_MODE,
    TIME_FORMAT_NUMBER,
    TIME_FORMAT_DASH,
    TIME_FORMAT_DOT,
    TIME_FORMAT_COLON,
};

typedef struct TimeFormat {
    u32 type;
    union {
        u32 value;
        char mode;
    };
} TimeFormat;

void time_format_print(TimeFormat *time_formats, u32 count)
{
    u32 i;
    for (i = 0; i < count; i++)
    {
        printf("(");
        /* print type */
        switch (time_formats[i].type)
        {
            case TIME_FORMAT_MODE: printf("MODE:"); break;
            case TIME_FORMAT_NUMBER: printf("NUMBER:"); break;
            case TIME_FORMAT_DASH: printf("DASH:"); break;
            case TIME_FORMAT_ERROR: printf("ERROR:"); break;
            case TIME_FORMAT_DOT: printf("DOT:"); break;
            case TIME_FORMAT_COLON: printf("COLON:"); break;
        }
        /* print value */
        if (time_formats[i].type == TIME_FORMAT_MODE)
            printf("%c", time_formats[i].mode);
        else if (time_formats[i].type == TIME_FORMAT_NUMBER)
            printf("%d", time_formats[i].value);
        printf("),\n");
    }
}

#define FLAG_NUMBER_ON 1
#define FLAG_DASH (1 << 1)
#define FLAG_MODE (1 << 2)
#define FLAG_MODE_OFFSET (1 << 3)
#define FLAG_MODE_DAY (1 << 4)
#define FLAG_MODE_WEEK (1 << 5)
#define FLAG_MODE_MONTH (1 << 6)
#define FLAG_TIME_VALUE_OFFSET (1 << 7)

enum {
    MODE_DEFAULT,
    MODE_DAY,
    MODE_WEEK,
    MODE_MONTH
};

void spoor_time_span_create(SpoorTimeSpan *spoor_time_span, char *argument, u32 argument_length)
{
    time_t time_start = time(NULL);
    time_t time_end = time_start;

    u8 modes[2] = { MODE_DEFAULT, MODE_DEFAULT };
    s32 value = 0;
    s32 mode_values[2] = { 0 };
    s32 time_values[2] = { SPOOR_TIME_DEFAULT_VALUE, SPOOR_TIME_DEFAULT_VALUE };
    u32 flags = 0;

    char *end;
    if (*argument == '-')
    {
        flags |= FLAG_DASH;
        argument++;
        argument_length--;
    }

    for (end = argument + argument_length + 1; argument != end; argument++)
    {
        switch (*argument)
        {
            case 'd':
            {
                modes[0 + (flags & FLAG_MODE_OFFSET) ?1 :0] = MODE_DAY;
                flags |= FLAG_MODE;
            } break;
            case 'w':
            {
                modes[0 + (flags & FLAG_MODE_OFFSET) ?1 :0] = MODE_WEEK;
                flags |= FLAG_MODE;
            } break;
            case 'm':
            {
                modes[0 + (flags & FLAG_MODE_OFFSET) ?1 :0] = MODE_MONTH;
                flags |= FLAG_MODE;
            } break;
        }

        if (*argument >= 0x30 && *argument <= 0x39)
        {
            flags |= FLAG_NUMBER_ON;
            value *= 10;
            value += *argument - 0x30;
        }
        else
        {
            flags ^= FLAG_NUMBER_ON;
            if (flags & FLAG_MODE)
            {
                if (flags & FLAG_DASH)
                    value = ~value + 1;

                mode_values[0 + (flags & FLAG_MODE_OFFSET) ?1 :0] = value;
                value = 0;

                flags ^= FLAG_MODE;
            }
            else if (value != 0)
            {
                time_values[0 + (flags & FLAG_TIME_VALUE_OFFSET) ?1 : 0] = value;
                value = 0;
            }
        }

        if (*argument == '-')
        {
            if (flags & FLAG_DASH)
                flags ^= FLAG_DASH;

            if (flags & FLAG_MODE_OFFSET)
                flags |= FLAG_DASH;
            else
            {
                flags |= FLAG_MODE_OFFSET;
                flags |= FLAG_TIME_VALUE_OFFSET;
            }
        }
    }

    /* time start */
    /* mode */
    switch (modes[0])
    {
        case MODE_DAY:
        {
            time_start += 24 * 60 * 60 * mode_values[0];
        } break;
        case MODE_WEEK:
        {
            time_start += 7 * 24 * 60 * 60 * mode_values[0];
        } break;
    }

    spoor_time_span->start = *(SpoorTime *)localtime(&time_start);
    /* mode month */
    if (modes[0] == MODE_MONTH)
    {
        u32 month = mode_values[0] + spoor_time_span->start.mon;
        spoor_time_span->start.year += month / 12;
        spoor_time_span->start.mon = month % 12;
    }
    if (time_values[0] != (s32)SPOOR_TIME_DEFAULT_VALUE)
    {
        spoor_time_span->start.hour = time_values[0] / 100;
        spoor_time_span->start.min = time_values[0] % 100;
    }
    else
    {
        spoor_time_span->start.hour = SPOOR_TIME_DEFAULT_VALUE;
        spoor_time_span->start.min =  SPOOR_TIME_DEFAULT_VALUE;
    }
    spoor_time_span->start.sec = SPOOR_TIME_DEFAULT_VALUE;

    switch (modes[1])
    {
        case MODE_DAY:
        {
            time_end += 24 * 60 * 60 * mode_values[1];
        } break;
        case MODE_WEEK:
        {
            time_end += 7 * 24 * 60 * 60 * mode_values[1];
        } break;
    }

    spoor_time_span->end = *(SpoorTime *)localtime(&time_end);

    if (modes[1] == MODE_DEFAULT)
        spoor_time_span->end = spoor_time_span->start;
    else if (modes[1] == MODE_MONTH)
    {
        u32 month = mode_values[1] + spoor_time_span->end.mon;
        spoor_time_span->end.year += month / 12;
        spoor_time_span->end.mon = month % 12;
    }

    if (time_values[1] != (s32)SPOOR_TIME_DEFAULT_VALUE)
    {
        spoor_time_span->end.hour = time_values[1] / 100;
        spoor_time_span->end.min = time_values[1] % 100;
    }
    else
    {
        spoor_time_span->end.hour = SPOOR_TIME_DEFAULT_VALUE;
        spoor_time_span->end.min =  SPOOR_TIME_DEFAULT_VALUE;
    }
    spoor_time_span->end.sec = SPOOR_TIME_DEFAULT_VALUE;

    spoor_time_span->start.year += 1900;
    spoor_time_span->start.mon += 1;
    spoor_time_span->end.year += 1900;
    spoor_time_span->end.mon += 1;
}

s32 time_compare(SpoorTime *time0, SpoorTime *time1)
{
    u32 time0_sum = time0->min +
        (time0->hour * 60) +
        (time0->day * 60 * 24) +
        (time0->mon * 60 * 24 * 31) +
        (time0->year * 60 * 24 * 31 * 12);
    u32 time1_sum = time1->min +
        (time1->hour * 60) +
        (time1->day * 60 * 24) +
        (time1->mon * 60 * 24 * 31) +
        (time1->year * 60 * 24 * 31 * 12);

    return time0_sum - time1_sum;
}

#if 0
u32 time_span_compare(SpoorTimeSpan *time_span0, SpoorTimeSpan *time_span1)
{
    return 0;
}
#endif
