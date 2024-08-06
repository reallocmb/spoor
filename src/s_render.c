#include"spoor.h" /* for lsp reason */
#include <wchar.h>

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
            graphic->pixels[pixels_position] = color_alpha_blend(color,
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

void render_text(Graphic *graphic,
                 u16 x,
                 u16 y,
                 const u16 *text,
                 u32 color)
{
    FT_Library library;
    FT_Face face;

    FT_Init_FreeType(&library);
#if 0
    FT_New_Face(library, "../data/FiraCode-Regular.ttf", 0, &face);
#else
    FT_New_Face(library, "../data/FreeMono.ttf", 0, &face);
#endif


    HDC device_context = GetDC(NULL);
    u32 dpi_x = GetDeviceCaps(device_context, LOGPIXELSX);
    u32 dpi_y = GetDeviceCaps(device_context, LOGPIXELSY);
    printf("dpi: %d-%d\n", dpi_x, dpi_y);
    printf("glyphs_num: %d\n", face->num_glyphs);

#if 1
    FT_Set_Char_Size(face, 0, 30 * 64, dpi_x, dpi_y);
#else
    FT_Set_Pixel_Sizes(face, 0, 10);
#endif


    FT_GlyphSlot slot = face->glyph;
    int pen_x, pen_y, n;

    pen_x = x;
    pen_y = y;

    render_line_horizontal(graphic, pen_x - 2, pen_y, 5, 0xff0000aa);
    render_line_vertical(graphic, pen_x, pen_y - 2, 5, 0xff0000aa);
    graphic->pixels[pen_y * graphic->width + pen_x] = 0xffff0000;

#if 1
    render_line_horizontal(graphic, pen_x, pen_y, graphic->width - pen_x, 0xff00ff00);
#endif

    for (n = 0; n < wcslen(text); n++)
    {

        FT_ULong glyph_index = FT_Get_Char_Index(face, text[n]);
        printf("glyph_index: %d\n", glyph_index);

        FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

            FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

        int x = pen_x + slot->bitmap_left;
        int y = pen_y - slot->bitmap_top;;

        int rows;
        int cols;

        for (rows = 0; rows < slot->bitmap.rows; rows++)
        {
            for (cols = 0; cols < slot->bitmap.pitch; cols++)
            {
                u32 bitmap_alpha = slot->bitmap.buffer[rows * slot->bitmap.pitch + cols];
                graphic->pixels[(y + rows) * graphic->width + (x + cols)] = color_alpha_blend(color, graphic->pixels[(y + rows) * graphic->width + (x + cols)], bitmap_alpha);
                // graphic->pixels[(y + rows) * graphic->width + (x + cols)] = 0xffff0000;
            }
        }

        pen_x += slot->advance.x >> 6;
    }


    FT_Done_FreeType(library);
}

void render_default_func(Graphic *graphic)
{
#if 0
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
