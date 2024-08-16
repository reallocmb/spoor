#include"spoor.h" /* for lsp reason */

#ifdef __unix__

#include<X11/Xlib.h>

#include<stdio.h>
#include<stdlib.h>

#define xlib_graphic_update graphic_update
void xlib_graphic_update(Graphic *graphic)
{
    /* clear */
    u32 i;
    for (i = 0; i < graphic->pixels_count; i++)
        graphic->pixels[i] = CONFIG_COLOR_BACKGROUND;
    // graphic->pixels[10] = 0xffaaaa;

    graphic->render_func(graphic);

    XPutImage(graphic->display,
              graphic->window,
              DefaultGC(graphic->display, 0),
              graphic->image,
              0, 0,
              0, 0,
              graphic->width, graphic->height);
}

#define xlib_init graphic_init
void xlib_init(Graphic *graphic)
{
    graphic->display = XOpenDisplay(NULL);
    if (graphic->display == NULL)
    {
        /* todo(Mat) Logging? */
        fprintf(stderr,
                "error[FILE: %s, LINE: %d]: failed to Open Display()\n",
                __FILE__, __LINE__);
    }

    graphic->window = XCreateWindow(graphic->display,
                           DefaultRootWindow(graphic->display),
                           0, 0,
                           graphic->width, graphic->height,
                           0,
                           CopyFromParent,
                           InputOutput,
                           CopyFromParent,
                           0,
                           0);

    Atom protocol_wm_delete_window = XInternAtom(graphic->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(graphic->display, graphic->window, &protocol_wm_delete_window, 1);

    XSelectInput(graphic->display,
                 graphic->window,
                 StructureNotifyMask |
                 ExposureMask |
                 KeyPressMask |
                 KeyReleaseMask);

    XMapWindow(graphic->display, graphic->window);

    graphic->image = XCreateImage(graphic->display,
                                  DefaultVisual(graphic->display, 0),
                                  24,
                                  ZPixmap,
                                  0,
                                  NULL,
                                  0,
                                  0,
                                  32, 
                                  0);


#if 0
    char *str = XSetLocaleModifiers("");
    printf("locale return: %s\n", str);
#endif

    XIM xim = XOpenIM(graphic->display,
                      NULL,
                      NULL,
                      NULL);
    if (xim == NULL)
    {
        fprintf(stderr, "FILE: %s, LINE: %d -> XOpenIM() failed\n", __FILE__, __LINE__);
        return;
    }

    graphic->input_context = XCreateIC(xim,
                                       XNInputStyle,
                                       XIMPreeditNothing | XIMStatusNothing,
                                       NULL);

    if (graphic->render_func == NULL)
        graphic->render_func = render_default_func;

    graphic->command_buffer.buffer = malloc(COMMAND_BUFFER_ALLOC * sizeof(graphic->command_buffer.buffer));
    graphic->command_buffer.buffer[0] = 0;
    graphic->command_buffer.count = 0;
    memcpy(graphic->command_buffer.buffer,
           L"NORMAL",
           7 * sizeof(*graphic->command_buffer.buffer));
}


#define xlib_graphic_main_loop graphic_main_loop
void xlib_graphic_main_loop(Graphic *graphic)
{
    XEvent event;
    while (graphic->running)
    {
        XNextEvent(graphic->display, &event);
        switch (event.type)
        {
            case Expose:
            {
                printf("Expose\n");
                xlib_graphic_update(graphic);
            } break;

            case ConfigureNotify:
            {
                printf("ConfigureNotify\n");

                graphic->width = event.xconfigure.width;
                graphic->height = event.xconfigure.height;
                printf("Width: %d Height %d\n",
                       graphic->width,
                       graphic->height);

                if (graphic->pixels)
                    free(graphic->pixels);

                graphic->pixels_count = graphic->width * graphic->height;
                graphic->pixels = malloc(graphic->pixels_count * sizeof(*graphic->pixels));

                graphic->image->width = graphic->width;
                graphic->image->height = graphic->height;
                graphic->image->bytes_per_line = graphic->width * sizeof(*graphic->pixels);
                graphic->image->data = (char *)graphic->pixels;
            } break;

            case KeyPress:
            {
                char buffer[255];
                wchar_t wbuffer[255];
                KeySym key;
                printf("KeyPress\n");
                Status status;
                int buffer_bytes = XmbLookupString(graphic->input_context,
                                                   &event.xkey,
                                                   buffer,
                                                   255,
                                                   &key,
                                                   &status);
                int wbuffer_bytes = XwcLookupString(graphic->input_context,
                                                    &event.xkey,
                                                    wbuffer,
                                                    255,
                                                    &key,
                                                    &status);
                printf("buffer: %s\n", buffer);
                printf("keysym: %ld\n", key);
                printf("keysym string: %s\n", XKeysymToString(key));
                printf("bytes: %d\n", buffer_bytes);
                puts("---");
                printf("wbuffer: %s\n", "öde");
                printf("keysym: %ld\n", key);
                printf("keysym string: %s\n", XKeysymToString(key));
                printf("bytes: %d\n", wbuffer_bytes);


                graphic->input_func(graphic, buffer[0]);
            } break;

            case KeyRelease:
            {
                printf("KeyReleased\n");
            } break;

            case ClientMessage:
            {
                printf("ClientMessage WM_DELETE_WINDOW\n");

                graphic->running = false;
            } break;

            default:
            {
                printf("Event Type: %d\n",
                       event.type);
            } break;
        }
    }
}

#endif
