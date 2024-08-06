#include"spoor.h"

void font_size_set(Font *font, u32 size)
{
    font->size = size;
    FT_Set_Pixel_Sizes(font->face, 0, size);
}

void font_load(Font *font, const char *path, u32 size)
{
    u32 error;
    error = FT_Init_FreeType(&font->library);
    if (error)
        exit(1);

    error = FT_New_Face(font->library,
                        path,
                        0,
                        &font->face);
    if (error)
        exit(1);

    font_size_set(font, size);
}
