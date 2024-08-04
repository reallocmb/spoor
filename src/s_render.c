#include"spoor.h" /* for lsp reason */

void render_line_horizontal(Graphic *graphic,
                            u16 x,
                            u16 y,
                            u16 width,
                            u32 color)
{
    while (width--)
        graphic->pixels[y * graphic->width + x + width] = color;
}

void render_line_vertical(Graphic *graphic,
                          u16 x,
                          u16 y,
                          u16 height,
                          u32 color)
{
    while (height--)
        graphic->pixels[(y + height) * graphic->width + x] = color;
}

void render_default_func(Graphic *graphic)
{
    render_line_horizontal(graphic, 50, 50, 100, 0xffffff);
    render_line_vertical(graphic, 100, 0, 100, 0xffffff);
}
