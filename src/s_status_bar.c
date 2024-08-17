#include"spoor.h"

void status_bar_init(void)
{
    CONFIG_STATUS_BAR_HEIGHT_SET(GlobalGraphic.font.height * CONFIG_GRAPHIC_SCALE + CONFIG_STATUS_BAR_PADDING * 2);
}

void status_bar_render_func(void)
{
    printf("CONFIG_STATUS_BAR_HEIGHT: %d\n", CONFIG_STATUS_BAR_HEIGHT);
    u16 height = CONFIG_STATUS_BAR_HEIGHT;
    u16 y = GlobalGraphic.height - height;

    render_rectangle_fill(0, y, GlobalGraphic.width, height, CONFIG_COLOR_STATUS_BAR_BACKGROUND);

    if (GlobalGraphic.mode == GRAPHIC_MODE_NORMAL)
    {
        render_text(10,
                    GlobalGraphic.height + (GlobalGraphic.font.face->size->metrics.descender >> 6) - CONFIG_STATUS_BAR_PADDING,
                    (u8 *)"NORMAL",
                    CONFIG_COLOR_STATUS_BAR_FOREGROUND);

        u32 offset = 0;
        if (GlobalGraphic.command_buffer.count > 5)
            offset = GlobalGraphic.command_buffer.count - 5;

        render_text(GlobalGraphic.width - 200,
                    GlobalGraphic.height + (GlobalGraphic.font.face->size->metrics.descender >> 6) - CONFIG_STATUS_BAR_PADDING,
                    GlobalGraphic.command_buffer.buffer + offset,
                    CONFIG_COLOR_STATUS_BAR_FOREGROUND);
    }
    else
    {
        render_text(10,
                    GlobalGraphic.height + (GlobalGraphic.font.face->size->metrics.descender >> 6) - CONFIG_STATUS_BAR_PADDING,
                    GlobalGraphic.command_buffer.buffer,
                    CONFIG_COLOR_STATUS_BAR_FOREGROUND);
    }

    // render_label(10, GlobalGraphic.height, height, 5, L"STATUS BAR TESTg", CONFIG_COLOR_STATUS_BAR_FOREGROUND);
}

