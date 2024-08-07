#include"spoor.h" /* for lsp reason */

u32 color_alpha_blend(u32 color0, u32 color1, u32 alpha)
{
    u32 alpha1 = 0xff - alpha;
    return (((alpha *((color0 >> 16) & 0xff) + alpha1 * ((color1 >> 16) & 0xff)) / 255) << 16) |
        (((alpha * ((color0 >> 8) & 0xff) + alpha1 * ((color1 >> 8) & 0xff)) / 255) << 8) |
        ((alpha * (color0 & 0xff) + alpha1 * (color1 & 0xff)) / 255);
}

void render_line_horizontal(Graphic *graphic,
                            u16 x,
                            u16 y,
                            u16 width,
                            u32 color)
{
    if (y >= graphic->height)
        return;

    if (x > graphic->width)
        return;

    if (x + width > graphic->width)
        width = graphic->width - x;

    while (width--)
        graphic->pixels[y * graphic->width + x + width] = color_alpha_blend(color,
                                                                            graphic->pixels[y * graphic->width + x + width],
                                                                            color >> 24);
}

void render_line_vertical(Graphic *graphic,
                          u16 x,
                          u16 y,
                          u16 height,
                          u32 color)
{
    if (x >= graphic->width)
        return;

    if (y > graphic->height)
        return;

    if (y + height > graphic->height)
        height = graphic->height - y;

    while (height--)
        graphic->pixels[(y + height) * graphic->width + x] = color_alpha_blend(color,
                                                                               graphic->pixels[(y + height) * graphic->width + x],
                                                                               color >> 24);
}

void render_rectangle_fill(Graphic *graphic,
                           u16 x,
                           u16 y,
                           u16 width,
                           u16 height,
                           u32 color)
{
    if (x >= graphic->width)
        return;

    if (y >= graphic->height)
        return;

    if (y + height > graphic->height)
        height = graphic->height - y;

    if (x + width > graphic->width)
        width = graphic->width - x;

    u32 pixels_position;
    u32 i;
    while (height--)
        for (i = 0; i < width; i++)
        {
            pixels_position = (y + height) * graphic->width + x + i;
            graphic->pixels[pixels_position] = color_alpha_blend(color & 0xffffff,
                                                                 graphic->pixels[pixels_position],
                                                                 color >> 24);
        }
}

void render_rectangle(Graphic *graphic,
                      u16 x,
                      u16 y,
                      u16 width,
                      u16 height,
                      u32 color)
{
    render_line_horizontal(graphic, x, y, width, color);
    render_line_vertical(graphic, x, y, height, color);
    render_line_horizontal(graphic, x, y + height - 1, width, color);
    render_line_vertical(graphic, x + width - 1, y, height, color);
}

#include<windows.h>

void render_label(Graphic *graphic,
                  u16 x,
                  u16 y,
                  u16 height,
                  u16 padding_top_bottom,
                  const u16 *text,
                  u32 color)
{
    u16 h = height - padding_top_bottom * 2;
}


void render_text(Graphic *graphic,
                 u16 x,
                 u16 y,
                 const u16 *text,
                 u32 color)
{
    FT_GlyphSlot slot = graphic->font.face->glyph;
    int pen_x, pen_y, i;

    int height = (graphic->font.face->height >> 6);
    height = (graphic->font.face->ascender >> 6) - (graphic->font.face->descender >> 6);
    height = (graphic->font.face->size->metrics.height >> 6);
    height = (graphic->font.face->size->metrics.ascender >> 6) - (graphic->font.face->size->metrics.descender >> 6);
    printf("height: %d\n", height);
    render_rectangle(graphic, x, y - height - (graphic->font.face->size->metrics.descender >> 6), 100, height, 0xffaa2266);

    /*
    render_line_horizontal(graphic, x, y - (graphic->font.face->bbox.yMin >> 6), 300, 0xff00ff00);
    render_line_horizontal(graphic, x, y - height, 300, 0xff00ff00);
    */

    pen_x = x;
    pen_y = y;

    for (i = 0; i < text[i] != 0; i++)
    {
        FT_ULong glyph_index = FT_Get_Char_Index(graphic->font.face, text[i]);

        FT_Load_Glyph(graphic->font.face, glyph_index, FT_LOAD_DEFAULT);

        FT_Render_Glyph(graphic->font.face->glyph, FT_RENDER_MODE_NORMAL);

        /* todo(mb) read the freetype tutorial*/

        int x = pen_x + slot->bitmap_left;
        int y = pen_y - slot->bitmap_top;

        int rows;
        int cols;

        for (rows = 0; rows < slot->bitmap.rows; rows++)
        {
            for (cols = 0; cols < slot->bitmap.pitch; cols++)
            {
                u32 bitmap_alpha = slot->bitmap.buffer[rows * slot->bitmap.pitch + cols];
                u32 pixels_count = (y + rows) * graphic->width + (x + cols);
                graphic->pixels[pixels_count] = color_alpha_blend(color,
                                                                  graphic->pixels[pixels_count],
                                                                  bitmap_alpha);
            }
        }
        pen_x += slot->advance.x >> 6;
    }
}

void render_default_func(Graphic *graphic)
{
#if 1
    render_line_horizontal(graphic, 50, 50, 100, 0xffffffff);
    render_line_vertical(graphic, 100, 0, 100, 0xffffffff);
    render_rectangle_fill(graphic, 100, 100, 60, 40, 0x33ffffff);
    render_rectangle(graphic, 100, 100, 60, 40, 0xff0c0c0c);
    render_rectangle_fill(graphic, graphic->width / 4, graphic->height / 4, graphic->width / 2, graphic->height / 2, 0x44aaff22);
    render_rectangle(graphic, graphic->width / 4, graphic->height / 4, graphic->width / 2, graphic->height / 2, 0xcc0c0c0c);
#endif
    u16 buffer[] = L"Das ist Öde";
    render_text(graphic, 200, 200, buffer, 0x33eeeeee);
}
