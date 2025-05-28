#include"spoor.h"

u32 global_ttf_cache_sizes_count = 0;

void font_size_set(SpoorFont *font, u32 size)
{
    FT_Set_Char_Size(font->face, 0, size * CONFIG(FONT_SCALE) * 64, font->dpi_x, font->dpi_y);
    //FT_Set_Pixel_Sizes(font->face, 0, size);
    font->size = size;
    font->height = (font->face->size->metrics.height >> 6);

    u32 i;
    for (i = 0; i < global_ttf_cache_sizes_count; i++)
    {
        if (size < global_ttf_cache_sizes[i].font_size)
            break;
    }

    global_ttf_cache_sizes[i].font_size = size;
    global_ttf_cache_data = global_ttf_cache_sizes[i].ttf_cache_data;

    memset(global_ttf_cache_data, 0, 256 * sizeof(*global_ttf_cache_data));
}

void font_load(SpoorFont *font, const char *path, u32 size)
{
    FT_Error error;
    FT_Init_FreeType(&font->library);
    error = FT_New_Face(font->library, path, 0, &font->face);
    if (error)
    {
        fprintf(stderr, "Can't load Font: %s\n", path);
        exit(1);
    }

    font_size_set(font, size);
}

u32 font_text_width_in_pixels(SpoorFont *font, const u8 *text)
{
    u32 text_length;
    for (text_length = 0; text[text_length] != 0; text_length++) { }
    return (font->face->size->metrics.max_advance >> 6) * text_length;
}
