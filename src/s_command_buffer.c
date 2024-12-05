#include"spoor.h"

void command_buffer_execute(void)
{
    CommandBuffer *command_buffer = &GlobalGraphic.command_buffer;
    if (memcmp(command_buffer->buffer, ":open", 5 * sizeof(*command_buffer->buffer)) == 0)
    {
    }
    else if (strncmp((char *)command_buffer->buffer, ":c", 2) == 0)
    {
        SpoorObject spoor_object;
        spoor_object_create(&spoor_object, (char *)command_buffer->buffer + 2, command_buffer->count - 2, NULL);
        spoor_object_save(&spoor_object);
        spoor_object_print(&spoor_object);
    }
    else if (strncmp((char *)command_buffer->buffer, ":e", 2) == 0)
    {
        Data *data = (Data *)GlobalGraphic.views[GlobalGraphic.views_index].data;
        SpoorObject *spoor_objects = data->spoor_objects;
        SpoorObject *spoor_object = &spoor_objects[data->index];
        spoor_object_create(spoor_object, (char *)command_buffer->buffer + 2, command_buffer->count - 2, spoor_object);
        spoor_object_save(spoor_object);
        spoor_object_print(spoor_object);
        printf(":e ->>>>>>>> test\n");
    }
    else if (strncmp((char *)command_buffer->buffer, ":f", 2) == 0)
    {
        TaskListData *data = (TaskListData *)GlobalGraphic.views[GlobalGraphic.views_index].data;
        spoor_filter_change(&data->spoor_filter, (char *)command_buffer->buffer + 2);
    }
    else if (memcmp(command_buffer->buffer, ":q", 2 * sizeof(*command_buffer->buffer)) == 0)
        exit(0);
    else
    {
        /* todo(mb) get rid of swprintf stuff */
        char error_buffer[250] = { 0 };
        sprintf(error_buffer, "command '%s' not found try ':help'", command_buffer->buffer);
        memcpy(command_buffer->buffer, error_buffer, strlen(error_buffer) + 1 * sizeof(error_buffer));
    }
}

void command_buffer_clear(CommandBuffer *command_buffer)
{
    command_buffer->buffer[0] = 0;
    command_buffer->count = 0;
}

u32 command_buffer_counter_detect(CommandBuffer *command_buffer, u32 counter_default)
{
    u32 counter = 0;

    u32 i = command_buffer->count - 1;
    u32 digit_mul = 1;
    while (i--)
    {
        if (!(command_buffer->buffer[i] >= 0x30 && command_buffer->buffer[i] <= 0x39))
            break;
        counter += (command_buffer->buffer[i] - 0x30) * digit_mul;
        digit_mul *= 10;
    }

    if (counter == 0)
        counter = counter_default;

    return counter;
}
