#include<math.h>
#include"spoor.h"

u32 argument_next(char **arguments, u32 arguments_last_length)
{
    *arguments += arguments_last_length;
    while (**arguments == ' ')
        (*arguments)++;

    if (**arguments == 0)
        return 0;

    u32 length;
    for (length = 0; (*arguments)[length] != ' ' && (*arguments)[length] != 0; length++) { }

    return length;
}

u32 priority_decode(char *argument, u32 argument_length)
{
    u32 priority = 0;
    u32 i;
    for (i = 1; i < argument_length; i++)
    {
        priority *= 10;
        priority += argument[i] - 0x30;
    }

    return priority;
}

void spoor_object_create(SpoorObject *spoor_object, char *arguments, SpoorObject *spoor_object_old)
{
    /* remove space */
    while (*arguments == ' ')
        arguments++;

    /* spoor title */
    char *ptr = strchr(arguments, ',');
    if (ptr)
    {
        u32 i = 0;
        while (arguments != ptr)
        {
            spoor_object->title[i] = *arguments;
            arguments++;
            i++;
        }
        spoor_object->title[i] = 0;
        arguments++;
    }
    if (*arguments == ' ')
        arguments++;
#if 0
    u32 i;
    for (i = 0; arguments[i] != ',' && i < SPOOR_OBJECT_TITLE_SIZE_MAX - 1; i++)
        spoor_object->title[i] = arguments[i];
    spoor_object->title[i] = 0;
    arguments += i + 1;
#endif

    SpoorTimeSpan *spoor_time_span = &spoor_object->deadline;

    u8 type;
    u8 status;
    u8 priority;
    if (spoor_object_old == NULL)
    {
        /* init values */
        memset(&spoor_object->deadline, SPOOR_TIME_DEFAULT_VALUE, sizeof(spoor_object->deadline));
        memset(&spoor_object->schedule, SPOOR_TIME_DEFAULT_VALUE, sizeof(spoor_object->schedule));

        spoor_object->id = SPOOR_OBJECT_NO_ID;
        spoor_object->schedule_id_next = 0;
        memset(&spoor_object->tracked, SPOOR_TIME_DEFAULT_VALUE, sizeof(spoor_object->tracked));
        memset(&spoor_object->complete, SPOOR_TIME_DEFAULT_VALUE, sizeof(spoor_object->complete));
        memset(&spoor_object->created, SPOOR_TIME_DEFAULT_VALUE, sizeof(spoor_object->created));

        type = SPOOR_TYPE_TASK;
        status = SPOOR_STATUS_NOT_STARTED;
        priority = 0;
    }
    else
    {
        type = spoor_object_old->type;
        status = spoor_object_old->status;
        priority = spoor_object_old->priority;
    }

    u32 argument_length = 0;
    while ((argument_length = argument_next(&arguments, argument_length)) != 0)
    {
        if (argument_length == 1)
        {
            switch (arguments[0])
            {
                case 't': type = SPOOR_TYPE_TASK; break;
                case 'p': type = SPOOR_TYPE_PROJECT; break;
                case 'e': type = SPOOR_TYPE_EVENT; break;
                case 'a': type = SPOOR_TYPE_APPOINTMENT; break;
                case 'g': type = SPOOR_TYPE_GOAL; break;
                case 'h': type = SPOOR_TYPE_HABIT; break;
                case 'i': type = SPOOR_TYPE_IDEA; break;
                case 'c': status = SPOOR_STATUS_COMPLETED; break;
                default: 
                {
                    spoor_time_span_create(spoor_time_span, arguments, argument_length);
                    spoor_time_span++;
                } break;
            }
        }
        else if (strncmp(arguments, "ip", 2) == 0)
            status = SPOOR_STATUS_IN_PROGRESS;
        else if (strncmp(arguments, "ns", 2) == 0)
            status = SPOOR_STATUS_NOT_STARTED;
        else if (strncmp(arguments, "p", 1) == 0)
        {
            priority = priority_decode(arguments, argument_length);
        }
        else
        {
            spoor_time_span_create(spoor_time_span, arguments, argument_length);
            spoor_time_span++;
        }
    }

    spoor_object->status = status;
    spoor_object->type = type;
    spoor_object->priority = priority;
}

u32 spoor_object_id_free_space(RedbasDB *db, u32 items)
{
    SpoorObject spoor_object;
    u32 i;
    for (i = 0; i < items; i++)
    {
        redbas_db_restore_cursor_set(db, i);
        redbas_db_restore(db, &spoor_object, sizeof(spoor_object));

        if (spoor_object.id == SPOOR_OBJECT_ID_DELETED)
            break;
    }

    return i;
}

u32 spoor_object_id_free_space_size(RedbasDB *db, u32 items, u32 size)
{
    SpoorObject spoor_object;
    u32 result_index = 0;
    u32 size_tmp = size;
    u32 i;
    for (i = 0; i < items; i++)
    {
        redbas_db_restore_cursor_set(db, i);
        redbas_db_restore(db, &spoor_object, sizeof(spoor_object));

        if (spoor_object.id == SPOOR_OBJECT_ID_DELETED)
        {
            size_tmp--;
            if (size_tmp == 0)
            {
                return i - (size - 1);
            }
        }
        else
        {
            size_tmp = size;
        }
    }

    return i;
}

void spoor_object_parent_save(SpoorObject *spoor_object)
{
    RedbasDB *db = redbas_db_open(".spoor/00000000.rdb", sizeof(*spoor_object));

    /* remove parent from current place */
    u32 remove_id = spoor_object->id;
    spoor_object->id = SPOOR_OBJECT_ID_DELETED;
    redbas_db_change(db, &spoor_object, sizeof(*spoor_object), remove_id);

    /* get new place  with alloc of 10 ahead */
    u32 id = spoor_object_id_free_space_size(db, db->items, 10);
    spoor_object->id = id;
    redbas_db_change(db, &spoor_boject, sizeof(*spoor_object), spoor_object->id);
    u32 i;
    for (i = 0; i < 9; i++)
    {
        redbas_db_change
    }

    redbas_db_close(db);
}

void spoor_object_child_save(SpoorObject *spoor_object)
{

}

void spoor_object_save(SpoorObject *spoor_object)
{
    RedbasDB *db = redbas_db_open(".spoor/00000000.rdb", sizeof(*spoor_object));
    u32 items = redbas_db_items(db);
    if (spoor_object->id == SPOOR_OBJECT_NO_ID)
        spoor_object->id = spoor_object_id_free_space(db, items);
    if (spoor_object->id == items)
        redbas_db_store(db, spoor_object, sizeof(*spoor_object));
    else
        redbas_db_change(db, spoor_object, sizeof(*spoor_object), spoor_object->id);
    redbas_db_close(db);
}

void spoor_object_remove(SpoorObject *spoor_object)
{
    SpoorObject spoor_object_restore;
    u32 remove_id = spoor_object->id;
    RedbasDB *db = redbas_db_open(".spoor/00000000.rdb", sizeof(*spoor_object));
    redbas_db_restore_cursor_set(db, remove_id);
    redbas_db_restore(db, &spoor_object_restore, sizeof(spoor_object_restore));
    spoor_object_restore.id = SPOOR_OBJECT_ID_DELETED;
    redbas_db_change(db, &spoor_object_restore, sizeof(spoor_object_restore), remove_id);
    redbas_db_close(db);
}

void spoor_object_print(SpoorObject *spoor_object)
{
    printf("-- Spoor Object --\n");
    printf("id: %i\n", spoor_object->id);
    printf("title: %s\n", spoor_object->title);
    spoor_time_span_print(&spoor_object->deadline, "deadline");
    spoor_time_span_print(&spoor_object->schedule, "schedule");
    printf("schedule_id_next: %i\n", spoor_object->schedule_id_next);
    spoor_time_span_print(&spoor_object->tracked, "tracked");
    spoor_time_span_print(&spoor_object->complete, "complete");
    spoor_time_span_print(&spoor_object->created, "created");
    printf("status: %i\n", spoor_object->status);
    printf("type: %i\n", spoor_object->type);
}

void spoor_objects_indexes_set(u32 *spoor_objects_indexes,
                               u32 *spoor_objects_indexes_count,
                               SpoorFilter *spoor_filter,
                               s32 (*sort_func)(const void *data0, const void *data1),
                               SpoorObject *spoor_objects,
                               u32 spoor_objects_count)
{
    *spoor_objects_indexes_count = 0;
    u32 i;
    for (i = 0; i < spoor_objects_count; i++)
    {
        if (!(spoor_filter->type & SPOOR_FLITER_TYPES_BITS[spoor_objects[i].type]))
            continue;
        if (!(spoor_filter->status & SPOOR_FILTER_STATUS_BITS[spoor_objects[i].status]))
            continue;

        /* Filter Flags */
        if (spoor_filter->flags & SPOOR_FILTER_FLAG_SCHEDULE)
        {
            if (spoor_objects[i].schedule.start.year == SPOOR_TIME_DEFAULT_VALUE)
                continue;
        }

        /* Filter Time Span Schedule */
        if (time_compare(&spoor_objects[i].schedule.start, &spoor_filter->schedule.start) < 0)
            continue;
        if (time_compare(&spoor_objects[i].schedule.end, &spoor_filter->schedule.end) > 0)
            continue;

        /* Sort */
        spoor_objects_indexes[*spoor_objects_indexes_count] = i;

        u32 tmp;
        u32 count;
        for (count = *spoor_objects_indexes_count;
             (count >= 1) && sort_func(&spoor_objects[spoor_objects_indexes[count]], &spoor_objects[spoor_objects_indexes[count - 1]]) < 0;
             count--)
        {
            tmp = spoor_objects_indexes[count - 1];
            spoor_objects_indexes[count - 1] = spoor_objects_indexes[count];
            spoor_objects_indexes[count] = tmp;
        }

        (*spoor_objects_indexes_count)++;
    }
}

void spoor_objects_indexes_sync(Data *data)
{
    IndexesSync *indexes_sync = &global_data.indexes_sync;
    u32 *spoor_objects_indexes = data->spoor_objects_indexes;
    u32 *spoor_objects_indexes_count = &data->spoor_objects_indexes_count;
    /* filter ? */


    SpoorObject *spoor_objects = global_data.spoor_objects;

    u32 tmp;
    u32 i;

    if (indexes_sync->mode & SPOOR_OBJECTS_INDEXES_SYNC_CREATE)
    {




        spoor_objects_indexes[*spoor_objects_indexes_count] = indexes_sync->index;


        for (i = *spoor_objects_indexes_count;
             (i >= 1) && data->sort_func(&spoor_objects[spoor_objects_indexes[i]], &spoor_objects[spoor_objects_indexes[i - 1]]) < 0;
             i--)
        {
            tmp = spoor_objects_indexes[i - 1];
            spoor_objects_indexes[i - 1] = spoor_objects_indexes[i];
            spoor_objects_indexes[i] = tmp;
        }
        (*spoor_objects_indexes_count)++;
    }
    else if (indexes_sync->mode & SPOOR_OBJECTS_INDEXES_SYNC_EDIT)
    {
        u32 index_start = data->index;
        if (spoor_objects_indexes[data->index] != indexes_sync->index)
        {
            for (index_start = 0; index_start < *spoor_objects_indexes_count; index_start++)
            {
                if (spoor_objects_indexes[index_start] == indexes_sync->index)
                    break;
            }
            if (index_start == *spoor_objects_indexes_count)
                return;
        }


        for (i = index_start;
             (i <= (*spoor_objects_indexes_count - 2)) && data->sort_func(&spoor_objects[spoor_objects_indexes[i]], &spoor_objects[spoor_objects_indexes[i - 1]]) > 0;
             i++)
        {
            tmp = spoor_objects_indexes[i + 1];
            spoor_objects_indexes[i + 1] = spoor_objects_indexes[i];
            spoor_objects_indexes[i] = tmp;
        }

        for (i = index_start;
             (i >= 1) && data->sort_func(&spoor_objects[spoor_objects_indexes[i]], &spoor_objects[spoor_objects_indexes[i - 1]]) < 0;
             i--)
        {
            tmp = spoor_objects_indexes[i - 1];
            spoor_objects_indexes[i - 1] = spoor_objects_indexes[i];
            spoor_objects_indexes[i] = tmp;
        }
    }
    else if (indexes_sync->mode & SPOOR_OBJECTS_INDEXES_SYNC_REMOVE)
    {
        u32 index_start = data->index;
        if (spoor_objects_indexes[data->index] != indexes_sync->index)
        {
            for (index_start = 0; index_start < *spoor_objects_indexes_count; index_start++)
            {
                if (spoor_objects_indexes[index_start] == indexes_sync->index)
                    break;
            }
            if (index_start == *spoor_objects_indexes_count)
                return;

        }
        memcpy(spoor_objects_indexes + index_start,
               spoor_objects_indexes + index_start + 1,
               (*spoor_objects_indexes_count - index_start) * sizeof(*spoor_objects_indexes));

        (*spoor_objects_indexes_count)--;
    }
}

void spoor_objects_load(SpoorObject *spoor_objects,
                       u32 *spoor_objects_count,
                       SpoorFilter *spoor_filter,
                       SpoorSort *spoor_sort)
{
    RedbasDB *db = redbas_db_open(".spoor/00000000.rdb", sizeof(*spoor_objects));

    u32 items_count = redbas_db_items(db);

    u32 i;
    u32 k;
    for (i = 0, k = 0; i < items_count; i++)
    {
        redbas_db_restore_cursor_set(db, i);
        redbas_db_restore(db, &spoor_objects[k], sizeof(*spoor_objects));
        /* Deleted Objects */
        if (k >= SPOOR_OBJECTS_MAX)
        {
            /* to do */
            /* Message over max spoor_objects elements */
            return;
        }

        if (spoor_objects[k].id == SPOOR_OBJECT_ID_DELETED)
            continue;

        /* Filter */
#if 1
        if (spoor_filter != NULL)
        {
            if (!(spoor_filter->type & SPOOR_FLITER_TYPES_BITS[spoor_objects[k].type]))
                continue;
            if (!(spoor_filter->status & SPOOR_FILTER_STATUS_BITS[spoor_objects[k].status]))
                continue;
        }
#endif

        /* Sort */
#if 1
        SpoorObject tmp;

        u32 count;
        for (count = k; (count >= 1) && sort_default_func(&spoor_objects[count], &spoor_objects[count - 1]) < 0; count--)
        {
            tmp = spoor_objects[count - 1];
            spoor_objects[count - 1] = spoor_objects[count];
            spoor_objects[count] = tmp;
        }
#else /* is not possibile with struct du dummfisch */
        u32 count;
        for (count = k; (count >= 1) && sort_default_func(&spoor_objects[count], &spoor_objects[count - 1]) < 0; count--)
        {
            spoor_objects[count] ^= spoor_objects[count - 1];
            spoor_objects[count - 1] ^= spoor_objects[count];
            spoor_objects[count] ^= spoor_objects[count - 1];
        }
#endif

        k++;
    }

    redbas_db_close(db);

    *spoor_objects_count = k;
}

void spoor_objects_put(SpoorObject *spoor_objects,
                       u32 *spoor_objects_count,
                       SpoorObject *spoor_object)
{
    SpoorObject tmp;

    if (*spoor_objects_count >= SPOOR_OBJECTS_MAX)
    {
        /* to do */
        /* Message over max spoor_objects elements */
        return;
    }

    spoor_objects[*spoor_objects_count] = *spoor_object;

    u32 count;
    for (count = *spoor_objects_count;
         (count >= 1) && sort_default_func(&spoor_objects[count], &spoor_objects[count - 1]) < 0;
         count--)
    {
        tmp = spoor_objects[count - 1];
        spoor_objects[count - 1] = spoor_objects[count];
        spoor_objects[count] = tmp;
    }

    (*spoor_objects_count)++;
}

/* retunrs new index position in the spoor_objects */
void spoor_objects_reorder(SpoorObject *spoor_objects,
                           u32 spoor_objects_count,
                           u32 spoor_object_index)
{
    if (spoor_objects_count <= 1)
        return;

    u32 index_result = spoor_object_index; /* ? */

    SpoorObject tmp;
    u32 index;

    for (index = spoor_object_index;
         (index <= (spoor_objects_count - 2)) && sort_default_func(&spoor_objects[index], &spoor_objects[index + 1]) > 0;
         index++)
    {
        tmp = spoor_objects[index + 1];
        spoor_objects[index + 1] = spoor_objects[index];
        spoor_objects[index] = tmp;
        index_result = index + 1;
    }

    for (index = spoor_object_index; (index >= 1) && sort_default_func(&spoor_objects[index], &spoor_objects[index - 1]) < 0;
         index--)
    {
        tmp = spoor_objects[index - 1];
        spoor_objects[index - 1] = spoor_objects[index];
        spoor_objects[index] = tmp;
        index_result = index - 1;
    }
}

void spoor_objects_remove(SpoorObject *spoor_objects,
                          u32 *spoor_objects_count,
                          u32 index)
{
    if (*spoor_objects_count <= 0)
        return;

    if (*spoor_objects_count - index - 1 >= 1)
    {
        memcpy(spoor_objects + index,
               spoor_objects + index + 1,
               (*spoor_objects_count - index - 1) * sizeof(*spoor_objects));
    }

    (*spoor_objects_count)--;
}

#if 0 /* I don't know what this shit is */
void spoor_objects_reorder(SpoorFilter *spoor_filter)
{
}
#endif

void spoor_object_sort(SpoorObject *spoor_objects, u32 spoor_objects_count)
{
    qsort(spoor_objects, spoor_objects_count, sizeof(*spoor_objects), sort_default_func);
}
