#include"spoor.h" /* for lsp reason */

void views_init(View **views)
{
    *views = malloc(SPOOR_VIEWS_ALLOC_SIZE * sizeof(*views));
}

void view_border_render(View *view)
{

    u16 x = view->x;
    u16 y = view->y;
    u16 width = view->width;
    u16 height = view->height;

    x += CONFIG_VIEW_GAP_SIZE;
    y += CONFIG_VIEW_GAP_SIZE;
    width -= CONFIG_VIEW_GAP_SIZE * 2;
    height -= CONFIG_VIEW_GAP_SIZE * 2;


    u32 color = 0xff555555;
    if (view - GlobalGraphic.views == GlobalGraphic.views_index)
        color = CONFIG_VIEW_BORDER_COLOR;

#if 0
    render_rectangle_fill(x, y, width, height, color_fill);
#endif
    u32 i;
    for (i = 0; i < CONFIG_VIEW_BORDER_SIZE; i++)
    {
        render_rectangle(x + i,
                         y + i,
                         width - i * 2,
                         height - i * 2,
                         color);
    }

#if 1
    view->x += CONFIG_VIEW_BORDER_SIZE + CONFIG_VIEW_GAP_SIZE;
    view->y += CONFIG_VIEW_BORDER_SIZE + CONFIG_VIEW_GAP_SIZE;
    view->width -= CONFIG_VIEW_BORDER_SIZE * 2 + CONFIG_VIEW_GAP_SIZE * 2;
    view->height -= CONFIG_VIEW_BORDER_SIZE * 2 + CONFIG_VIEW_GAP_SIZE * 2;
#endif
}

void view_default_render_func(View *view)
{
    SpoorFont *font = &GlobalGraphic.font;

    u16 x = view->x;
    u16 y = view->y;
#if 0
    u16 width = view->width;
    u16 height = view->height;
#endif

    u32 size = font->size;

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

#if 0
    sprintf(buffer, "AREA: (%i, %i) (%i, %i)",
            view->x, view->y,
            width, height);
#endif
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

#if 1
    font_size_set(font, size);
#endif
}

View *views_childs_render(View **views) /* todo(mb) it's crap */
{
    View *parent = *views;
    View *child = *views + 1;

    u16 childs_count = parent->childs_count;

    u16 parent_width = parent->width;
    u16 parent_height = parent->height;

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
    for (k = 0; k < childs_count; k++)
    {
        child->width = parent_width;
        child->height = parent_height;
        child->x = parent->x + k * cw;
        child->y = parent->y + k * ch;
        child->id = child - GlobalGraphic.views;

        if (child->flags & VIEW_FLAG_PARENT)
        {
            child = views_childs_render(&child);
        }
        else
        {
            view_border_render(child);
            child->render_func(child);
            child++;
        }
    }
    
    return child;
}

void views_render(View *views)
{
    if (GlobalGraphic.views_count == 0)
        return;

    views->width = GlobalGraphic.width;
    views->height = GlobalGraphic.height - CONFIG_STATUS_BAR_HEIGHT;
    views->x = 0;
    views->y = 0;

    if (views->flags & VIEW_FLAG_PARENT)
        views_childs_render(&views);
    else
    {
        view_border_render(views);
        views->render_func(views);
    }
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

    if (*views_count == 0)
    {
        views[0].flags = flags;
        views[0].id = 0;
        views[0].parent_id = VIEW_NO_PARENT;
        views[0].childs_count = 0;
        views[0].render_func = render_func;
        views[0].input_func = NULL;

        *views_index = 0;
        (*views_count)++;
    }
    else if (*views_count == 1)
    {
        u16 child0_id = (*views_count)++;
        u16 child1_id = (*views_count)++;

        View *parent = &views[*views_index];
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
        child1->input_func = NULL;

        parent->childs_count = 2;
        parent->flags = VIEW_FLAG_PARENT | flags;

        *views_index = child1_id;
    }
    else if (((views[*views_index].flags & VIEW_FLAG_CHILD) | flags) == VIEW_FLAG_CHILD)
    {
        View *parent = &views[*views_index];
        memcpy(parent + 3, parent + 1, (&views[*views_count - 1] - parent) * sizeof(*views));

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
        child1->input_func = NULL;

        parent->childs_count = 2;
        parent->flags |= VIEW_FLAG_PARENT;

        *views_index = child1_id;

        *views_count += 2;
    }
    else
    {
        View *parent = &views[views[*views_index].parent_id];
        View *index = &views[*views_index];

        memcpy(index + 2, index + 1, (&views[*views_count - 1] - index) * sizeof(*views));

        u16 child0_id = index - views + 1;

        View *child0 = &views[child0_id];

        child0->flags = flags;
        child0->id = child0_id;
        child0->parent_id = parent->id;
        child0->childs_count = 0;
        child0->render_func = render_func;
        child0->input_func = NULL;

        parent->childs_count++;

        *views_index = child0_id;

        (*views_count)++;
    }
}

void view_close(u32 index)
{
    u32 *views_count = &GlobalGraphic.views_count;

    if (index == 1)
    {
        *views_count = 0;
    }
}

void view_focus_left(u32 *views_index)
{
    do
    {
        if (*views_index == 0)
            return;
        *views_index -= 1;
    }
    while (GlobalGraphic.views[*views_index].flags & VIEW_FLAG_PARENT);
}

void view_focus_down(u32 *views_index)
{
    do
    {
        if (GlobalGraphic.views_count - 1 == *views_index)
            return;
        *views_index += 1;
    }
    while (GlobalGraphic.views[*views_index].flags & VIEW_FLAG_PARENT);
}

void view_focus_up(u32 *views_index)
{
    do
    {
        if (*views_index == 0)
            return;
        *views_index -= 1;
    }
    while (GlobalGraphic.views[*views_index].flags & VIEW_FLAG_PARENT);
}

void view_focus_right(u32 *views_index)
{
    do
    {
        if (GlobalGraphic.views_count - 1 == *views_index)
            return;
        *views_index += 1;
    }
    while (GlobalGraphic.views[*views_index].flags & VIEW_FLAG_PARENT);
}
