#include"spoor.h"

void command_buffer_execute(void)
{
    CommandBuffer *command_buffer = &GlobalGraphic.command_buffer;
    if (memcmp(command_buffer->buffer, ":open", 5 * sizeof(*command_buffer->buffer)) == 0)
    {
    }

    if (command_buffer->count == 2)
    {
        if (memcmp(command_buffer->buffer, ":q", 2 * sizeof(*command_buffer->buffer)) == 0)
            exit(0);
    }

    /* todo(mb) get rid of swprintf stuff */
    char error_buffer[250] = { 0 };
    sprintf(error_buffer, "command '%s' not found try ':help'", command_buffer->buffer);
    memcpy(command_buffer->buffer, error_buffer, strlen(error_buffer) + 1 * sizeof(error_buffer));
}

void command_buffer_clear(CommandBuffer *command_buffer)
{
    GlobalGraphic.command_buffer.buffer[0] = 0;
    GlobalGraphic.command_buffer.count = 0;
}
