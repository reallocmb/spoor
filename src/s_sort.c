#include"spoor.h"

s32 alphabet_compare(char *buffer0, char *buffer1)
{
    s32 diff = 0;
    u32 i;

    for (i = 0; !(buffer0[i] == 0 || buffer1[i] == 0); i++)
    {
        diff = buffer0[i] - buffer1[i];
        if (diff < 0)
            return -1;
        if (diff > 0)
            return 1;
    }

    if (strlen(buffer0) > strlen(buffer1))
        return 1;
    else
        return -1;

    return 0;
}

#if 0
s32 sort_time_schedule_only(const void *data0, const void *data1)
{
    SpoorObject *spoor_object0 = (SpoorObject *)data0;
    SpoorObject *spoor_object1 = (SpoorObject *)data1;

    /*
    s32 result = 
    */

    return 0;
}
#endif

s32 sort_calendar_func(const void *data0, const void *data1)
{
    SpoorObject *spoor_object0 = (SpoorObject *)data0;
    SpoorObject *spoor_object1 = (SpoorObject *)data1;

    s32 result = time_compare(&spoor_object0->schedule.start, &spoor_object1->schedule.start);
#if 0
    if (result == 0)
        result = spoor_object1->priority - spoor_object0->priority;
    if (result == 0)
        result = alphabet_compare(spoor_object0->title, spoor_object1->title);
#endif

    return result;
}

s32 sort_default_func(const void *data0, const void *data1)
{
    SpoorObject *spoor_object0 = (SpoorObject *)data0;
    SpoorObject *spoor_object1 = (SpoorObject *)data1;

    s32 result = time_compare(&spoor_object0->deadline.end, &spoor_object1->deadline.end);
    if (result == 0)
        result = time_compare(&spoor_object0->schedule.start, &spoor_object1->schedule.start);
    if (result == 0)
        result = spoor_object1->priority - spoor_object0->priority;
    if (result == 0)
        result = alphabet_compare(spoor_object0->title, spoor_object1->title);

    return result;
}
