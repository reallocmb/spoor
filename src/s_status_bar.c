#include"spoor.h"

void status_bar_init(void)
{
    /*
    CONFIG_SET(STATUS_BAR_HEIGHT, (GlobalGraphic.font.height * CONFIG(FONT_SCALE) + CONFIG_STATUS_BAR_PADDING * 2));
    */
    /* todo (reallocmb) */
    font_size_set(&GlobalGraphic.font, 30);
    CONFIG_SET(STATUS_BAR_HEIGHT, GlobalGraphic.font.height);
    font_size_set(&GlobalGraphic.font, CONFIG(FONT_DEFAULT_SIZE));

}

void status_bar_render_func(void)
{
    /* todo (reallocmb) */
    font_size_set(&GlobalGraphic.font, 30);
    // u16 height = -(GlobalGraphic.font.face->size->metrics.descender >> 6) + (GlobalGraphic.font.face->size->metrics.ascender >> 6);
    const u16 height = CONFIG(STATUS_BAR_HEIGHT);
    u16 y = GlobalGraphic.height - height;

    render_rectangle_fill(0, y, GlobalGraphic.width, height, CONFIG(STATUS_BAR_COLOR_BACKGROUND));

    if (GlobalGraphic.mode == GRAPHIC_MODE_NORMAL)
    {
        render_text(10,
                    GlobalGraphic.height + (GlobalGraphic.font.face->size->metrics.descender >> 6),
                    (u8 *)"NORMAL",
                    CONFIG(STATUS_BAR_COLOR_FOREGROUND));

        u32 offset = 0;
        if (GlobalGraphic.command_buffer.count > 5)
            offset = GlobalGraphic.command_buffer.count - 5;

        render_text(GlobalGraphic.width - 200,
                    GlobalGraphic.height + (GlobalGraphic.font.face->size->metrics.descender >> 6),
                    GlobalGraphic.command_buffer.buffer + offset,
                    CONFIG(STATUS_BAR_COLOR_FOREGROUND));
    }
    else
    {
        render_text(10,
                    GlobalGraphic.height + (GlobalGraphic.font.face->size->metrics.descender >> 6),
                    GlobalGraphic.command_buffer.buffer,
                    CONFIG(STATUS_BAR_COLOR_FOREGROUND));
    }

    // render_label(10, GlobalGraphic.height, height, 5, L"STATUS BAR TESTg", CONFIG(STATUS_BAR_COLOR_FOREGROUND));
    /* todo (reallocmb) */
    font_size_set(&GlobalGraphic.font, CONFIG(FONT_DEFAULT_SIZE));
}

