#include"spoor.h" /* for lsp reason */

void views_init(View **views)
{
    *views = malloc(SPOOR_VIEWS_ALLOC_SIZE * sizeof(*views));
}

void view_default_render_func(View *view)
{
    SpoorFont *font = &GlobalGraphic.font;

    u16 x = view->x;
    u16 y = view->y;
    u16 width = view->width;
    u16 height = view->height;

    const u16 gap = 20;

    x += gap / 2;
    y += gap / 2;
    width -= gap;
    height -= gap;

    render_rectangle_fill(x, y, width, height, 0x22aaaaaa);
    render_rectangle(x, y, width, height, 0xff550000);

    u32 size = font->size;
    font_size_set(font, 30);

    x += 10;
    y += font->height;

    render_text(x, y, (u8 *)"Flags:", CONFIG_COLOR_FOREGROUND);
    y += font->height;

    font_size_set(font, 15);
    if (view->flags & VIEW_FLAG_VERTICAL)
    {
        render_text(x, y, (u8 *)"VIEW_FLAG_VERTICAL", CONFIG_COLOR_FOREGROUND);
        y += font->height;
    }

    if (view->flags & VIEW_FLAG_HORIZONTAL)
    {
        render_text(x, y, (u8 *)"VIEW_FLAG_HORIZONTAL", CONFIG_COLOR_FOREGROUND);
        y += font->height;
    }

    if (view->flags & VIEW_FLAG_PARENT)
    {
        render_text(x, y, (u8 *)"VIEW_FLAG_PARENT", CONFIG_COLOR_FOREGROUND);
        y += font->height;
    }

    font_size_set(font, size);
}

void views_render(View *views, u32 views_count)
{
    if (GlobalGraphic.views_count == 0)
        return;

    views->width = GlobalGraphic.width;
    views->height = GlobalGraphic.height - CONFIG_STATUS_BAR_HEIGHT;
    views->x = 0;
    views->y = 0;

    views->render_func(views);
}

void view_append(View **head,
                 u32 *views_count,
                 u32 flags,
                 void (*render_func)(View *view))
{
    if (*views_count % SPOOR_VIEWS_ALLOC_SIZE == 0)
        *head = realloc(*head, *views_count + SPOOR_VIEWS_ALLOC_SIZE * sizeof(**head));

    (*head)->render_func = render_func;
    (*head)->flags = flags;

    (*views_count)++;
}
