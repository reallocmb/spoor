#include"spoor.h"

void command_buffer_execute(void)
{
    CommandBuffer *command_buffer = &GlobalGraphic.command_buffer;
    if (memcmp(command_buffer->buffer, ":open", 5 * sizeof(*command_buffer->buffer)) == 0)
    {
    }
    else if (memcmp(command_buffer->buffer, ":close", 6 * sizeof(*command_buffer->buffer)) == 0)
    {
        view_close(GlobalGraphic.views_index);
        return;
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
