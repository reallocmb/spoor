#include"spoor.h"

int sort_func(const void *data0, const void *data1)
{
    SpoorObject *spoor_object0 = (SpoorObject *)data0;
    SpoorObject *spoor_object1 = (SpoorObject *)data1;

    s32 result = time_compare(&spoor_object0->schedule.start, &spoor_object1->schedule.start);
    if (result == 0)
        result = time_compare(&spoor_object0->deadline.end, &spoor_object1->deadline.end);

    return result;
}
