#include"spoor.h" /* for lsp reason */

#ifdef _WIN32
#include<windows.h>

Graphic *global_graphic;

#define win32_graphic_update graphic_update
void win32_graphic_update(Graphic *grapic)
{
    printf("graphic_update()\n");
    u32 i;
    for (i = 0; i < global_graphic->pixels_count; i++)
        global_graphic->pixels[i] = CONFIG_COLOR_BACKGROUND;

    global_graphic->render_func(global_graphic);



    BITMAPINFO bitmapinfo;
    bitmapinfo.bmiHeader.biSize = sizeof(bitmapinfo.bmiHeader);
    bitmapinfo.bmiHeader.biWidth = global_graphic->width;
    bitmapinfo.bmiHeader.biHeight = -global_graphic->height;
    bitmapinfo.bmiHeader.biPlanes = 1;
    bitmapinfo.bmiHeader.biBitCount = 32;
    bitmapinfo.bmiHeader.biCompression = BI_RGB;

    StretchDIBits(global_graphic->device_context,
                  0, 0,
                  global_graphic->width,
                  global_graphic->height,
                  0, 0,
                  global_graphic->width,
                  global_graphic->height,
                  global_graphic->pixels,
                  &bitmapinfo,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

LRESULT main_wndproc(HWND window,
                     UINT message,
                     WPARAM w_param,
                     LPARAM l_param)
{
    LRESULT result = 0;
    switch (message)
    {
        case WM_SIZE:
        {
            printf("wm_size\n");
            OutputDebugStringA("WM_SIZE\n");
            RECT window_size;
            GetClientRect(window, &window_size);
            
            global_graphic->width = window_size.right - window_size.left;
            global_graphic->height = window_size.bottom - window_size.top;
            
            if (global_graphic->pixels)
                free(global_graphic->pixels);
            
            global_graphic->pixels_count = global_graphic->width * global_graphic->height;
            global_graphic->pixels = malloc(global_graphic->pixels_count * sizeof(*global_graphic->pixels));
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            global_graphic->device_context = BeginPaint(window, &paint);
            printf("wm_paint\n");
            OutputDebugStringA("WM_PAINT\n");
            graphic_update(global_graphic);
            EndPaint(window, &paint);
        } break;

        case WM_CHAR:
        {
            printf("wm_char: %c\n", (char)w_param);
            global_graphic->input_func(global_graphic, (u8) w_param);
        } break;

        case WM_SYSKEYDOWN:
        {
            printf("syskeydown: %lld\n", w_param);
        } break;

        case WM_KEYDOWN:
        {
        } break;
        
        case WM_DESTROY:
        {
            printf("wm_destroy\n");
            OutputDebugStringA("WM_DESTROY\n");
            global_graphic->running = false;
        } break;
        
        case WM_CLOSE:
        {
            printf("wm_close\n");
            OutputDebugStringA("WM_CLOSE\n");
            global_graphic->running = false;
        } break;

#if 0
        case WM_MOUSEWHEEL:
        {
            int delta = GET_WHEEL_DELTA_WPARAM(w_param);
            printf("Mouse Delta %d\n", delta);
            if (delta > 0)
            {
                CONFIG_GRAPHIC_SCALE_SET((float)(CONFIG_GRAPHIC_SCALE + 0.1));
            }
            else
            {
                CONFIG_GRAPHIC_SCALE_SET(CONFIG_GRAPHIC_SCALE - 0.1);
            }
        } break;
#endif
        
        default:
        {
            result = DefWindowProc(window, message, w_param, l_param);
        } break;
    }
    return result;
}

#define win32_init graphic_init
void win32_init(Graphic *graphic)
{
    global_graphic = graphic;

    HINSTANCE instance = GetModuleHandle(NULL);
    WNDCLASS window_class = { 0 };
    window_class.lpszClassName = "SpoorClass";
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = main_wndproc;
    window_class.hInstance = instance;
    window_class.hCursor = NULL;
    window_class.hbrBackground = 0;
    window_class.hIcon = NULL; /* load a icon todo(mb) */
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    ATOM register_class_atom = RegisterClass(&window_class);
    
    if (!register_class_atom)
    {
        OutputDebugStringA("Failed to Register Class\n");
        return;
    }
    
    HWND window = CreateWindowExA(0,
                                  window_class.lpszClassName,
                                  "Spoor",
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  global_graphic->width,
                                  global_graphic->height,
                                  0,
                                  0,
                                  instance,
                                  0);
    
    HDC device_context = GetDC(NULL);
    graphic->font.dpi_x = GetDeviceCaps(device_context, LOGPIXELSX);
    graphic->font.dpi_y = GetDeviceCaps(device_context, LOGPIXELSY);

    if (graphic->render_func == NULL)
        graphic->render_func = render_default_func;

    graphic->command_buffer.buffer = malloc(COMMAND_BUFFER_ALLOC * sizeof(graphic->command_buffer.buffer));
    graphic->command_buffer.buffer[0] = 0;
    graphic->command_buffer.count = 0;
    memcpy(graphic->command_buffer.buffer,
           L"NORMAL",
           11 * sizeof(*graphic->command_buffer.buffer));
}

#define win32_main_loop graphic_main_loop
void win32_main_loop(Graphic *graphic)
{
    MSG message = { 0 };
    while (global_graphic->running)
    {
        if (GetMessage(&message, NULL, 0, 0) > 0)
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
}

#endif
