#include<math.h>
#include <string.h>
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

void spoor_object_create(SpoorObject *spoor_object, char *arguments, u32 arguments_size, SpoorObject *spoor_object_old)
{
    printf("arguments: '%s'\ncount: %d\n\n", arguments, arguments_size);

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
    }
#if 0
    u32 i;
    for (i = 0; arguments[i] != ',' && i < SPOOR_OBJECT_TITLE_SIZE_MAX - 1; i++)
        spoor_object->title[i] = arguments[i];
    spoor_object->title[i] = 0;
    arguments += i + 1;
#endif

    /* initial spoor object's values */
    if (spoor_object_old == NULL)
    {
        memset(&spoor_object->deadline, SPOOR_TIME_DEFAULT_VALUE, sizeof(spoor_object->deadline));
        memset(&spoor_object->schedule, SPOOR_TIME_DEFAULT_VALUE, sizeof(spoor_object->schedule));
    }

    SpoorTimeSpan *spoor_time_span = &spoor_object->deadline;

    u8 type;
    u8 status;
    if (spoor_object_old == NULL)
    {
        type = SPOOR_TYPE_TASK;
        status = SPOOR_STATUS_NOT_STARTED;
    }
    else
    {
        type = spoor_object_old->type;
        status = spoor_object_old->status;
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
        else
        {
            spoor_time_span_create(spoor_time_span, arguments, argument_length);
            spoor_time_span++;
        }
    }

    /* handle other argumentss */

    if (spoor_object_old == NULL)
    {
        spoor_object->id = SPOOR_OBJECT_NO_ID;
        spoor_object->id_parent = SPOOR_OBJECT_NO_PARENT_ID;
        spoor_object->schedule_id_next = 0;
        memset(&spoor_object->tracked, SPOOR_TIME_DEFAULT_VALUE, sizeof(spoor_object->tracked));
        memset(&spoor_object->complete, SPOOR_TIME_DEFAULT_VALUE, sizeof(spoor_object->complete));
        memset(&spoor_object->created, SPOOR_TIME_DEFAULT_VALUE, sizeof(spoor_object->created));
    }
    spoor_object->status = status;
    spoor_object->type = type;
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
    printf("id_parent: %i\n", spoor_object->id_parent);
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

u32 spoor_object_load(SpoorObject **spoor_objects)
{
    RedbasDB *db = redbas_db_open(".spoor/00000000.rdb", sizeof(**spoor_objects));

    u32 items_count = redbas_db_items(db);
    *spoor_objects = malloc(items_count * sizeof(**spoor_objects));

    u32 i;
    u32 k;

    for (i = 0, k = 0; i < items_count; i++)
    {
        redbas_db_restore_cursor_set(db, i);
        redbas_db_restore(db, &(*spoor_objects)[k], sizeof(**spoor_objects));
        printf("id: %d == %d\n", (*spoor_objects)[k].id, SPOOR_OBJECT_ID_DELETED);
        spoor_object_print(&(*spoor_objects)[k]);
        if ((*spoor_objects)[k].id != SPOOR_OBJECT_ID_DELETED)
            k++;
    }

    redbas_db_close(db);

    return k;
}
