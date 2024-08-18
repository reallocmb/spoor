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

    const u16 gap = 0;

    x += gap / 2;
    y += gap / 2;
    width -= gap;
    height -= gap;


    if (view - GlobalGraphic.views == GlobalGraphic.views_index)
    {
        render_rectangle_fill(x, y, width, height, 0x55333333);
        render_rectangle(x, y, width, height, 0xffffffff);
        render_rectangle(x + 1, y + 1, width - 2, height - 2, 0xffffffff);
        render_rectangle(x + 2, y + 2, width - 4, height - 4, 0xffffffff);
        render_rectangle(x + 3, y + 3, width - 6, height - 6, 0xffffffff);
    }
    else
    {
        render_rectangle_fill(x, y, width, height, 0x22aaaaaa);
        render_rectangle(x, y, width, height, CONFIG_COLOR_FOREGROUND);
        render_rectangle(x + 1, y + 1, width - 2, height - 2, CONFIG_COLOR_FOREGROUND);
        render_rectangle(x + 2, y + 2, width - 4, height - 4, CONFIG_COLOR_FOREGROUND);
        render_rectangle(x + 3, y + 3, width - 6, height - 6, CONFIG_COLOR_FOREGROUND);
    }

    u32 size = font->size;
    font_size_set(font, 15);

    x += 10;
    y += font->height;

    render_text(x, y, (u8 *)"Flags:", CONFIG_COLOR_FOREGROUND);
    y += font->height;

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

    char buffer[100];

    sprintf(buffer, "ID: %i", view->id);
    render_text(x, y, (u8 *)buffer, CONFIG_COLOR_FOREGROUND);
    y += font->height;

    sprintf(buffer, "AREA: (%i, %i) (%i, %i)",
            view->x, view->y,
            view->width, view->height);
    render_text(x, y, (u8 *)buffer, CONFIG_COLOR_FOREGROUND);
    y += font->height;

    if (view->parent_id == VIEW_NO_PARENT)
        sprintf(buffer, "PARENT-ID: %s", "VIEW_NO_PARENT");
    else
        sprintf(buffer, "PARENT-ID: %i", view->parent_id);
    render_text(x, y, (u8 *)buffer, CONFIG_COLOR_FOREGROUND);
    y += font->height;

    sprintf(buffer, "CHILDS-COUNT: %i", view->childs_count);
    render_text(x, y, (u8 *)buffer, CONFIG_COLOR_FOREGROUND);
    y += font->height;

    // void (*render_func)(struct View *view); /* void *graphic => Graphic *graphic */

    font_size_set(font, size);
}

void views_childs_render(View *views, u16 *i)
{
    u16 parent_id = *i;
    View *child = &views[++(*i)];
    u16 childs_count = views[parent_id].childs_count;

    u16 parent_width = views[parent_id].width;
    u16 parent_height = views[parent_id].height;

    u16 cw = parent_width / childs_count;
    u16 ch = 0;

    if (child->flags & VIEW_FLAG_VERTICAL)
    {
        cw = ch;
        ch = parent_height / childs_count;
        parent_height = ch;
    }
    else
        parent_width = cw;

    u16 k;
    for (k = 0; k < views->childs_count; k++)
    {
        child = &views[*i];
        child->width = parent_width;
        child->height = parent_height;
        child->x = views[parent_id].x + k * cw;
        child->y = views[parent_id].y + k * ch;

        if (views[*i].flags & VIEW_FLAG_PARENT)
            views_childs_render(&views[*i], i);
        else
        {
            views[*i].render_func(&views[*i]);
            (*i)++;
        }
    }
}

void views_render(View *views)
{
    if (GlobalGraphic.views_count == 0)
        return;

    views->width = GlobalGraphic.width;
    views->height = GlobalGraphic.height - CONFIG_STATUS_BAR_HEIGHT;
    views->x = 0;
    views->y = 0;

    u16 i = 0;
    if (views->flags & VIEW_FLAG_PARENT)
        views_childs_render(views, &i);
    else
        views->render_func(views);
}

void view_append(View **head,
                 u32 *views_count,
                 u32 *views_index,
                 u32 flags,
                 void (*render_func)(View *view))
{
    if (*views_count % SPOOR_VIEWS_ALLOC_SIZE == 0)
        *head = realloc(*head, *views_count + SPOOR_VIEWS_ALLOC_SIZE * sizeof(**head));

    View *views = *head;
    View *parent = &views[*views_index];

    if (*views_count == 0)
    {
        views[0].flags = flags;
        views[0].id = 0;
        views[0].parent_id = VIEW_NO_PARENT;
        views[0].childs_count = 0;
        views[0].render_func = render_func;

        *views_index = 0;
        (*views_count)++;
    }
    else if (*views_count == 1)
    {
        u16 child0_id = (*views_count)++;
        u16 child1_id = (*views_count)++;

        View *child0 = &views[child0_id];
        View *child1 = &views[child1_id];

        *child0 = *parent;
        child0->flags = flags;
        child0->id = child0_id;
        child0->parent_id = parent->id;
        child0->childs_count = 0;
        
        child1->flags = flags;
        child1->id = child1_id;
        child1->parent_id = parent->id;
        child1->childs_count = 0;
        child1->render_func = render_func;

        parent->childs_count = 2;
        parent->flags = VIEW_FLAG_PARENT | flags;

        *views_index = child1_id;
    }
    else if (((parent->flags & 3) | flags) == 3)
    {
        memcpy(parent + 3, parent + 1, (parent - &views[*views_count - 1]) * sizeof(*views));

        u16 child0_id = parent->id + 1;
        u16 child1_id = parent->id + 2;

        View *child0 = &views[child0_id];
        View *child1 = &views[child1_id];

        *child0 = *parent;
        child0->flags = flags;
        child0->id = child0_id;
        child0->parent_id = parent->id;
        child0->childs_count = 0;
        
        child1->flags = flags;
        child1->id = child1_id;
        child1->parent_id = parent->id;
        child1->childs_count = 0;
        child1->render_func = render_func;

        parent->childs_count = 2;
        parent->flags |= flags;

        *views_index = child1_id;

        *views_count += 2;
    }
    else
    {
        memcpy(parent + 2, parent + 1, (parent - &views[*views_count - 1]) * sizeof(*views));

        u16 child0_id = parent->id + 1;

        View *child0 = &views[child0_id];

        child0->flags = flags;
        child0->id = child0_id;
        child0->parent_id = parent->id;
        child0->childs_count = 0;
        child0->render_func = render_func;

        parent->childs_count++;

        *views_index = child0_id;

        (*views_count)++;
    }
}
