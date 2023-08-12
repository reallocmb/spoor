#include"spoor_internal.h"

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include<time.h>
#include<uuid/uuid.h>

void spoortime_parse(char *argv, SpoorTime *spoor_time)
{
    int count = 0;
    char mode = 0;
    int hour = 0;
    int minute = 0;
    int sign = 1;

    /* count */
    int32_t j = 0;

    if (argv[0] == '-')
    {
        sign = -1;
        j++;
    }

    for (; j < strlen(argv); j++)
    {
        if (argv[j] >= 0x30 && argv[j] <= 0x39)
        {
            count *= 10;
            count += argv[j] - 0x30;
        }
        else
            break;
    }

    /* mode */
    mode = argv[j];
    j++;

    /* time_ */
    for (; j < strlen(argv); j++)
    {
        if (argv[j] >= 0x30 && argv[j] <= 0x39)
        {
            minute *= 10;
            minute += argv[j] - 0x30;
        }
        else
            break;
    }

    hour = minute / 100;
    minute = minute % 100;

    time_t current_time;
    current_time = time(NULL);

    if (mode == 'd')
        current_time += count * 60 * 60 * 24 * sign;

    if (mode == 'w')
        current_time += count * 60 * 60 * 24 * 7 * sign;

    spoor_time->start = *localtime(&current_time);
    if (mode == 'm')
    {
        spoor_time->start.tm_mon += count * sign;
        if (spoor_time->start.tm_mon <= -1)
        {
            spoor_time->start.tm_year--;
            spoor_time->start.tm_mon = 11; /* wrong */
        }

        if (spoor_time->start.tm_mon >= 12)
        {
            spoor_time->start.tm_year++;
            spoor_time->start.tm_mon = 0; /* wrong */
        }
    }

    spoor_time->start.tm_hour = hour;
    spoor_time->start.tm_min = minute;

    /* time end */
    if (argv[j] == '-')
    {
        hour = 0;
        minute = 0;

        j++;
        for (; j < strlen(argv); j++)
        {
            if (argv[j] >= 0x30 && argv[j] <= 0x39)
            {
                minute *= 10;
                minute += argv[j] - 0x30;
            }
            else
                break;
        }

        hour = minute / 100;
        minute = minute % 100;
    }
    else
    {
        hour = -1;
        minute = -1;
    }

    spoor_time->end = spoor_time->start;
    spoor_time->end.tm_hour = hour;
    spoor_time->end.tm_min = minute;
}

SpoorObject *spoor_arguments_parse(int argc, char **argv)
{
    SpoorObject *spoor_object = malloc(sizeof(*spoor_object));
    assert(spoor_object != NULL);
    memset(spoor_object, 0, sizeof(*spoor_object));

    /* create title */
    char *title;
    if (argc < 1)
        return spoor_object;

    title = argv[0];
    uint32_t i;
    for (i = 0; i < argc - 1; i++)
    {
        uint32_t argv_len = strlen(argv[i]);

        /* check argv is not title */
        if (argv[i][argv_len - 1] == ',')
        {
            argv[i][argv_len - 1] = 0;
            i++;
            break;
        }

        argv[i][argv_len] = ' ';
    }

    memcpy(spoor_object->title, title, strlen(title));
    spoor_object->title[strlen(title)] = 0;

    /* create time arguments */
    if (i < argc && argc != 1)
        spoortime_parse(argv[i], &spoor_object->deadline);
    else
        spoor_object->deadline.start.tm_year = -1;
    i++;
    if (i < argc)
        spoortime_parse(argv[i], &spoor_object->schedule);
    else
        spoor_object->schedule.start.tm_year = -1;


    spoor_object->type = TYPE_EVENT;
    spoor_object->status = STATUS_NOT_STARTED;

    uuid_t uuid;
    uuid_generate(uuid);
    uuid_unparse(uuid, spoor_object->id);

    /* created time */
    time_t current_time;
    current_time = time(NULL);
    spoor_object->created.start = *localtime(&current_time);

    /* parent */
    spoor_object->parent_title[0] = '-';
    spoor_object->parent_title[1] = 0;
    spoor_object->next = NULL;

    return spoor_object;
}
