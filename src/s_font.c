#include"spoor.h"

void font_size_set(Font *font, u32 size)
{
    FT_Set_Char_Size(font->face, 0, size * CONFIG_GRAPHIC_SCALE * 64, font->dpi_x, font->dpi_y);
    //FT_Set_Pixel_Sizes(font->face, 0, size);
}

void font_load(Font *font, const char *path, u32 size)
{
    FT_Init_FreeType(&font->library);
    FT_New_Face(font->library, path, 0, &font->face);

    font_size_set(font, size);

    font->height = (font->face->size->metrics.height >> 6);
}
