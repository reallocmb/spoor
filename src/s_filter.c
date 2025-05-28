#include"spoor.h"

void spoor_filter_set(SpoorFilter *spoor_filter, SpoorObject *spoor_objects, u32 *spoor_objects_count)
{
    u32 i, j;
    for (i = 0, j = 0; i < *spoor_objects_count; i++)
    {
        if (!(spoor_filter->type & SPOOR_FLITER_TYPES_BITS[spoor_objects[i].type]))
            continue;
        if (!(spoor_filter->status & SPOOR_FILTER_STATUS_BITS[spoor_objects[i].status]))
            continue;

        spoor_objects[j] = spoor_objects[i];
        j++;
    }

    *spoor_objects_count = j;
}

void spoor_filter_change(SpoorFilter *spoor_filter, char *arguments)
{
    if (arguments[0] == ' ')
        arguments++;

    u32 argument_length = 0;
    while ((argument_length = argument_next(&arguments, argument_length)) != 0)
    {
        if (arguments[0] == 't')
        {
            bool minus = false;

            u32 i;
            for (i = 1; i < argument_length; i++)
            {
                if (minus)
                {
                    switch (arguments[i])
                    {
                        case 't': spoor_filter->type &= ~SPOOR_FILTER_TYPE_TASK; break;
                        case 'p': spoor_filter->type &= ~SPOOR_FILTER_TYPE_PROJECT; break;
                        case 'e': spoor_filter->type &= ~SPOOR_FILTER_TYPE_EVENT; break;
                        case 'a': spoor_filter->type &= ~SPOOR_FILTER_TYPE_APPOINTMENT; break;
                        case 'g': spoor_filter->type &= ~SPOOR_FILTER_TYPE_GOAL; break;
                        case 'h': spoor_filter->type &= ~SPOOR_FILTER_TYPE_HABIT; break;
                        case 'i': spoor_filter->type &= ~SPOOR_FILTER_TYPE_IDEA; break;
                        case '+': minus = false; break;
                        case '.': spoor_filter->type = SPOOR_FILTER_TYPE_ALL; break;
                        case '0': spoor_filter->type = 0; break;
                    }
                }
                else
                {
                    switch (arguments[i])
                    {
                        case 't': spoor_filter->type |= SPOOR_FILTER_TYPE_TASK; break;
                        case 'p': spoor_filter->type |= SPOOR_FILTER_TYPE_PROJECT; break;
                        case 'e': spoor_filter->type |= SPOOR_FILTER_TYPE_EVENT; break;
                        case 'a': spoor_filter->type |= SPOOR_FILTER_TYPE_APPOINTMENT; break;
                        case 'g': spoor_filter->type |= SPOOR_FILTER_TYPE_GOAL; break;
                        case 'h': spoor_filter->type |= SPOOR_FILTER_TYPE_HABIT; break;
                        case 'i': spoor_filter->type |= SPOOR_FILTER_TYPE_IDEA; break;
                        case '-': minus = true; break;
                        case '.': spoor_filter->type = SPOOR_FILTER_TYPE_ALL; break;
                        case '0': spoor_filter->type = 0; break;
                    }
                }
            }
        }
        else if (arguments[0] == 's')
        {
            bool minus = false;

            u32 i;
            for (i = 1; i < argument_length; i++)
            {
                if (minus)
                {
                    switch (arguments[i])
                    {
                        case 'n': spoor_filter->status &= ~SPOOR_FILTER_STATUS_NOT_STARTED; break;
                        case 'i': spoor_filter->status &= ~SPOOR_FILTER_STATUS_IN_PROGRESS; break;
                        case 'c': spoor_filter->status &= ~SPOOR_FILTER_STATUS_COMPLETED; break;
                        case '+': minus = false; break;
                        case '.': spoor_filter->status = SPOOR_FILTER_STATUS_ALL; break;
                        case '0': spoor_filter->status = 0; break;
                    }
                }
                else
                {
                    switch (arguments[i])
                    {
                        case 'n': spoor_filter->status |= SPOOR_FILTER_STATUS_NOT_STARTED; break;
                        case 'i': spoor_filter->status |= SPOOR_FILTER_STATUS_IN_PROGRESS; break;
                        case 'c': spoor_filter->status |= SPOOR_FILTER_STATUS_COMPLETED; break;
                        case '-': minus = true; break;
                        case '.': spoor_filter->status = SPOOR_FILTER_STATUS_ALL; break;
                        case '0': spoor_filter->status = 0; break;
                    }
                }
            }
        }
    }
}
