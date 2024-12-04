#include"spoor.h" /* for lsp reason */
#include<wchar.h>

u32 color_alpha_blend(u32 color0, u32 color1, u32 alpha)
{
    u32 alpha1 = 0xff - alpha;
    return (((alpha *((color0 >> 16) & 0xff) + alpha1 * ((color1 >> 16) & 0xff)) / 255) << 16) |
        (((alpha * ((color0 >> 8) & 0xff) + alpha1 * ((color1 >> 8) & 0xff)) / 255) << 8) |
        ((alpha * (color0 & 0xff) + alpha1 * (color1 & 0xff)) / 255);
}

void render_line_horizontal(u16 x,
                            u16 y,
                            u16 width,
                            u32 color)
{
    if (y >= GlobalGraphic.height)
        return;

    if (x > GlobalGraphic.width)
        return;

    if (x + width > GlobalGraphic.width)
        width = GlobalGraphic.width - x;

    while (width--)
        GlobalGraphic.pixels[y * GlobalGraphic.width + x + width] = color_alpha_blend(color,
                                                                            GlobalGraphic.pixels[y * GlobalGraphic.width + x + width],
                                                                            color >> 24);
}

void render_line_vertical(u16 x,
                          u16 y,
                          u16 height,
                          u32 color)
{
    if (x >= GlobalGraphic.width)
        return;

    if (y > GlobalGraphic.height)
        return;

    if (y + height > GlobalGraphic.height)
        height = GlobalGraphic.height - y;

    while (height--)
        GlobalGraphic.pixels[(y + height) * GlobalGraphic.width + x] = color_alpha_blend(color,
                                                                               GlobalGraphic.pixels[(y + height) * GlobalGraphic.width + x],
                                                                               color >> 24);
}

void render_rectangle_fill(u16 x,
                           u16 y,
                           u16 width,
                           u16 height,
                           u32 color)
{
    if (x >= GlobalGraphic.width)
        return;

    if (y >= GlobalGraphic.height)
        return;

    if (y + height > GlobalGraphic.height)
        height = GlobalGraphic.height - y;

    if (x + width > GlobalGraphic.width)
        width = GlobalGraphic.width - x;

    u32 pixels_position;
    u32 i;
    while (height--)
        for (i = 0; i < width; i++)
        {
            pixels_position = (y + height) * GlobalGraphic.width + x + i;
            GlobalGraphic.pixels[pixels_position] = color_alpha_blend(color & 0xffffff,
                                                                 GlobalGraphic.pixels[pixels_position],
                                                                 color >> 24);
        }
}

void render_rectangle(u16 x,
                      u16 y,
                      u16 width,
                      u16 height,
                      u32 color)
{
    render_line_horizontal(x, y, width, color);
    render_line_vertical(x, y, height, color);
    render_line_horizontal(x, y + height - 1, width, color);
    render_line_vertical(x + width - 1, y, height, color);
}

#if 0 /* todo(mb) remove? */
void render_label(
                  u16 x,
                  u16 y,
                  u16 height,
                  u16 padding_top_bottom,
                  const u16 *text,
                  u32 color)
{
    u16 h = height - padding_top_bottom * 2;
}
#endif

u32 utf8_decode(const u8 *str, u32 *i) /* todo(mb) rewrite it plese */
{
    u32 c = 0;
    if (str[*i] < 0x80)
    {
        c = (u32)str[*i];
    }
    else if (str[*i] < 0xe0)
    {
        c = ((str[*i] & 0x1f) << 6) | (str[*i + 1] & 0x3f);
        *i += 1;
    }
    else if (str[*i] < 0xf0)
    {
        c = ((str[*i] & 0x0f) << 12) | ((str[*i + 1] & 0x3f) << 6) | (str[*i + 2] & 0x3F);
        *i += 2;
    }
    else if (str[*i] < 0xf8)
    {
        c = ((str[*i] & 0x07) << 18) | ((str[*i + 1] & 0x3F) << 12) | ((str[*i + 2] & 0x3F) << 6) | (str[*i + 3] & 0x3F);
        *i += 3;
    }

    return c;
}

void view_render_text(View *view,
                      u16 x,
                      u16 y,
                      const u8 *text,
                      u32 color)
{
    FT_GlyphSlot slot = GlobalGraphic.font.face->glyph;
    u16 pen_x, pen_y;

#if 0 /* todo(mb) remove ? */
    u16 height = (GlobalGraphic.font.face->size->metrics.ascender >> 6) - (GlobalGraphic.font.face->size->metrics.descender >> 6);
    render_rectangle(x, y - height - (GlobalGraphic.font.face->size->metrics.descender >> 6), 100, height, 0xffaa2266); /* todo remove? */
#endif

    pen_x = x;
    pen_y = y;

    u32 i;
    for (i = 0; text[i] != 0; i++)
    {
        FT_ULong glyph_index = FT_Get_Char_Index(GlobalGraphic.font.face, utf8_decode(text, &i));

        FT_Load_Glyph(GlobalGraphic.font.face, glyph_index, FT_LOAD_DEFAULT);

        FT_Render_Glyph(GlobalGraphic.font.face->glyph, FT_RENDER_MODE_NORMAL);

        /* todo(mb) read the freetype tutorial*/

        u16 x = pen_x + slot->bitmap_left;
        u16 y = pen_y - slot->bitmap_top;

        u16 rows;
        u16 cols;

        u16 slot_pitch = slot->bitmap.pitch;
        if ((x - view->x) + slot_pitch >= view->width)
        {
#if 0
            slot_pitch -= view->width - (x - view->x); /* for mor accuracy maybe? */
#endif
            return;
        }

        for (rows = 0; rows < slot->bitmap.rows; rows++)
        {
            for (cols = 0; cols < slot_pitch; cols++)
            {
                u32 bitmap_alpha = slot->bitmap.buffer[rows * slot->bitmap.pitch + cols];
                u32 pixels_count = (y + rows) * GlobalGraphic.width + (x + cols);
                GlobalGraphic.pixels[pixels_count] = color_alpha_blend(color,
                                                                  GlobalGraphic.pixels[pixels_count],
                                                                  bitmap_alpha);
            }
        }
        pen_x += slot->advance.x >> 6;
    }
}

void render_text(u16 x,
                 u16 y,
                 const u8 *text,
                 u32 color)
{
    FT_GlyphSlot slot = GlobalGraphic.font.face->glyph;
    u16 pen_x, pen_y;

#if 0 /* todo(mb) remove ? */
    u16 height = (GlobalGraphic.font.face->size->metrics.ascender >> 6) - (GlobalGraphic.font.face->size->metrics.descender >> 6);
    render_rectangle(x, y - height - (GlobalGraphic.font.face->size->metrics.descender >> 6), 100, height, 0xffaa2266); /* todo remove? */
#endif

    pen_x = x;
    pen_y = y;

    u32 i;
    for (i = 0; text[i] != 0; i++)
    {
        FT_ULong glyph_index = FT_Get_Char_Index(GlobalGraphic.font.face, utf8_decode(text, &i));

        FT_Load_Glyph(GlobalGraphic.font.face, glyph_index, FT_LOAD_DEFAULT);

        FT_Render_Glyph(GlobalGraphic.font.face->glyph, FT_RENDER_MODE_NORMAL);

        /* todo(mb) read the freetype tutorial*/

        u16 x = pen_x + slot->bitmap_left;
        u16 y = pen_y - slot->bitmap_top;

        u16 rows;
        u16 cols;

        for (rows = 0; rows < slot->bitmap.rows; rows++)
        {
            for (cols = 0; cols < slot->bitmap.pitch; cols++)
            {
                u32 bitmap_alpha = slot->bitmap.buffer[rows * slot->bitmap.pitch + cols];
                u32 pixels_count = (y + rows) * GlobalGraphic.width + (x + cols);
                GlobalGraphic.pixels[pixels_count] = color_alpha_blend(color,
                                                                  GlobalGraphic.pixels[pixels_count],
                                                                  bitmap_alpha);
            }
        }
        pen_x += slot->advance.x >> 6;
    }
}

void render_default_func(void)
{
#if 1
    render_line_horizontal(50, 50, 100, 0xffffffff);
    render_line_vertical(100, 0, 100, 0xffffffff);
    render_rectangle_fill(100, 100, 60, 40, 0x33ffffff);
    render_rectangle(100, 100, 60, 40, 0xff0c0c0c);
    render_rectangle_fill(GlobalGraphic.width / 4, GlobalGraphic.height / 4, GlobalGraphic.width / 2, GlobalGraphic.height / 2, 0x44aaff22);
    render_rectangle(GlobalGraphic.width / 4, GlobalGraphic.height / 4, GlobalGraphic.width / 2, GlobalGraphic.height / 2, 0xcc0c0c0c);
#endif
#if 1
    render_text(200, 200, (u8 *)"das ist öde möde öäé", 0x33eeeeee);
#endif
}
