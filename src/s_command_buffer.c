void command_buffer_execute(CommandBuffer *command_buffer)
{

    if (memcmp(command_buffer->buffer, L":open", 5 * sizeof(*command_buffer->buffer)) == 0)
    {
    }

    if (command_buffer->count == 2)
    {
        if (memcmp(command_buffer->buffer, L":q", 2 * sizeof(*command_buffer->buffer)) == 0)
            exit(0);
    }

    /* todo(mb) get rid of swprintf stuff */
    wchar_t error_buffer[250] = { 0 };
    swprintf(error_buffer, 250, L"command '%ls' not found try ':help'", command_buffer->buffer);
    memcpy(command_buffer->buffer, error_buffer, wcslen(error_buffer) + 1 * sizeof(error_buffer));
}
