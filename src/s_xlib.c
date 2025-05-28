#include"spoor.h" /* for lsp reason */

#ifdef __unix__

#include<X11/Xlib.h>

#include<stdio.h>
#include<stdlib.h>

#define xlib_graphic_update graphic_update
void xlib_graphic_update(void)
{
    /* clear */
    u32 i;
    for (i = 0; i < GlobalGraphic.pixels_count; i++)
        GlobalGraphic.pixels[i] = CONFIG_COLOR_BACKGROUND;
    // GlobalGraphic.pixels[10] = 0xffaaaa;

    GlobalGraphic.render_func();

    XPutImage(GlobalGraphic.display,
              GlobalGraphic.window,
              DefaultGC(GlobalGraphic.display, 0),
              GlobalGraphic.image,
              0, 0,
              0, 0,
              GlobalGraphic.width, GlobalGraphic.height);
}

#define xlib_init graphic_init
void xlib_init(void)
{
    GlobalGraphic.display = XOpenDisplay(NULL);
    if (GlobalGraphic.display == NULL)
    {
        /* todo(Mat) Logging? */
        fprintf(stderr,
                "error[FILE: %s, LINE: %d]: failed to Open Display()\n",
                __FILE__, __LINE__);
    }

    GlobalGraphic.window = XCreateWindow(GlobalGraphic.display,
                           DefaultRootWindow(GlobalGraphic.display),
                           0, 0,
                           GlobalGraphic.width, GlobalGraphic.height,
                           0,
                           CopyFromParent,
                           InputOutput,
                           CopyFromParent,
                           0,
                           0);

    Atom protocol_wm_delete_window = XInternAtom(GlobalGraphic.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(GlobalGraphic.display, GlobalGraphic.window, &protocol_wm_delete_window, 1);

    XSelectInput(GlobalGraphic.display,
                 GlobalGraphic.window,
                 StructureNotifyMask |
                 ExposureMask |
                 KeyPressMask |
                 KeyReleaseMask);

    XMapWindow(GlobalGraphic.display, GlobalGraphic.window);

    GlobalGraphic.image = XCreateImage(GlobalGraphic.display,
                                  DefaultVisual(GlobalGraphic.display, 0),
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

    XIM xim = XOpenIM(GlobalGraphic.display,
                      NULL,
                      NULL,
                      NULL);
    if (xim == NULL)
    {
        fprintf(stderr, "FILE: %s, LINE: %d -> XOpenIM() failed\n", __FILE__, __LINE__);
        return;
    }

    GlobalGraphic.input_context = XCreateIC(xim,
                                       XNInputStyle,
                                       XIMPreeditNothing | XIMStatusNothing,
                                       NULL);

    if (GlobalGraphic.render_func == NULL)
        GlobalGraphic.render_func = render_default_func;

    GlobalGraphic.command_buffer.buffer = malloc(COMMAND_BUFFER_ALLOC_SIZE * sizeof(GlobalGraphic.command_buffer.buffer));
    GlobalGraphic.command_buffer.buffer[0] = 0;
    GlobalGraphic.command_buffer.count = 0;
}

#define xlib_graphic_main_loop graphic_main_loop
void xlib_graphic_main_loop(void)
{
    XEvent event;
    while (GlobalGraphic.running)
    {
        XNextEvent(GlobalGraphic.display, &event);
        switch (event.type)
        {
            case Expose:
            {
                printf("Expose\n");
                xlib_graphic_update();
            } break;

            case ConfigureNotify:
            {
                printf("ConfigureNotify\n");

                GlobalGraphic.width = event.xconfigure.width;
                GlobalGraphic.height = event.xconfigure.height;
                printf("Width: %d Height %d\n",
                       GlobalGraphic.width,
                       GlobalGraphic.height);

                if (GlobalGraphic.pixels)
                    free(GlobalGraphic.pixels);

                GlobalGraphic.pixels_count = GlobalGraphic.width * GlobalGraphic.height;
                GlobalGraphic.pixels = malloc(GlobalGraphic.pixels_count * sizeof(*GlobalGraphic.pixels));

                GlobalGraphic.image->width = GlobalGraphic.width;
                GlobalGraphic.image->height = GlobalGraphic.height;
                GlobalGraphic.image->bytes_per_line = GlobalGraphic.width * sizeof(*GlobalGraphic.pixels);
                GlobalGraphic.image->data = (char *)GlobalGraphic.pixels;
            } break;

            case KeyPress:
            {
                char buffer[255];
                wchar_t wbuffer[255];
                KeySym key;
                printf("KeyPress\n");
                Status status;
                int buffer_bytes = XmbLookupString(GlobalGraphic.input_context,
                                                   &event.xkey,
                                                   buffer,
                                                   255,
                                                   &key,
                                                   &status);
                int wbuffer_bytes = XwcLookupString(GlobalGraphic.input_context,
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


                GlobalGraphic.input_func(buffer[0]);
            } break;

            case KeyRelease:
            {
                printf("KeyReleased\n");
            } break;

            case ClientMessage:
            {
                printf("ClientMessage WM_DELETE_WINDOW\n");

                GlobalGraphic.running = false;
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
