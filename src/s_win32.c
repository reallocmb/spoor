#include"spoor.h" /* for lsp reason */

#ifdef _WIN32
#include<windows.h>

#define win32_graphic_update graphic_update
void win32_graphic_update(void)
{

    GlobalGraphic.render_func();

    BITMAPINFO bitmapinfo;
    bitmapinfo.bmiHeader.biSize = sizeof(bitmapinfo.bmiHeader);
    bitmapinfo.bmiHeader.biWidth = GlobalGraphic.width;
    bitmapinfo.bmiHeader.biHeight = -GlobalGraphic.height;
    bitmapinfo.bmiHeader.biPlanes = 1;
    bitmapinfo.bmiHeader.biBitCount = 32;
    bitmapinfo.bmiHeader.biCompression = BI_RGB;

    StretchDIBits(GlobalGraphic.device_context,
                  0, 0,
                  GlobalGraphic.width,
                  GlobalGraphic.height,
                  0, 0,
                  GlobalGraphic.width,
                  GlobalGraphic.height,
                  GlobalGraphic.pixels,
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
        /*
        case WM_CREATE:
        {
            HICON icon = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);

            SendMessage(window, WM_SETICON, ICON_BIG, (LPARAM)icon);
        } break;
        */

        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
            RECT window_size;
            GetClientRect(window, &window_size);
            
            GlobalGraphic.width = window_size.right - window_size.left;
            GlobalGraphic.height = window_size.bottom - window_size.top;
            
            if (GlobalGraphic.pixels)
                free(GlobalGraphic.pixels);
            
            GlobalGraphic.pixels_count = GlobalGraphic.width * GlobalGraphic.height;
            GlobalGraphic.pixels = malloc(GlobalGraphic.pixels_count * sizeof(*GlobalGraphic.pixels));
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            GlobalGraphic.device_context = BeginPaint(window, &paint);
            OutputDebugStringA("WM_PAINT\n");
            graphic_update();
            EndPaint(window, &paint);
        } break;

        case WM_CHAR:
        {
            GlobalGraphic.input_func((u8)w_param);
        } break;

        case WM_SYSKEYDOWN:
        {
        } break;

        case WM_KEYDOWN:
        {
        } break;
        
        case WM_DESTROY:
        {
            printf("wm_destroy\n");
            OutputDebugStringA("WM_DESTROY\n");
            GlobalGraphic.running = false;
        } break;
        
        case WM_CLOSE:
        {
            printf("wm_close\n");
            OutputDebugStringA("WM_CLOSE\n");
            GlobalGraphic.running = false;
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
void win32_init(void)
{
    HINSTANCE instance = GetModuleHandle(NULL);
    WNDCLASS window_class = { 0 };
    window_class.lpszClassName = "SpoorClass";
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = main_wndproc;
    window_class.hInstance = instance;
    window_class.hCursor = NULL;
    window_class.hIcon = LoadImage(NULL, "data/icon0.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
#if 0
    window_class.hIcon = LoadIcon(NULL, IDI_ERROR); /* load a icon todo(mb) */
#endif
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
                                  GlobalGraphic.width,
                                  GlobalGraphic.height,
                                  0,
                                  0,
                                  instance,
                                  0);

    DwmSetWindowAttribute(window, 19, &(BOOL){ TRUE }, sizeof(BOOL));
    
    HDC device_context = GetDC(NULL);
#if 0
    GlobalGraphic.font.dpi_x = GetDeviceCaps(device_context, LOGPIXELSX);
    GlobalGraphic.font.dpi_y = GetDeviceCaps(device_context, LOGPIXELSY);
#endif

    if (GlobalGraphic.render_func == NULL)
        GlobalGraphic.render_func = render_default_func;

    GlobalGraphic.command_buffer.buffer = malloc(COMMAND_BUFFER_ALLOC_SIZE * sizeof(GlobalGraphic.command_buffer.buffer));
    GlobalGraphic.command_buffer.buffer[0] = 0;
    GlobalGraphic.command_buffer.count = 0;
}

#define win32_main_loop graphic_main_loop
void win32_main_loop(void)
{
    MSG message = { 0 };
    while (GlobalGraphic.running)
    {
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
}

#endif
