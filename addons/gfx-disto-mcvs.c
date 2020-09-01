#include "../gfx.h"
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "../mcrs.h"

#define COLOR_REF(n)  ( (((n) & 0xFF0000) >> 16) | ((n) & 0xFF00) | (((n) & 0xFF) << 16) )

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

void gfx_clipboard_copy(const char *buf, int len)
{
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), buf, len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}

int gfx_clipboard_paste(char *buf, int len)
{
    OpenClipboard(0);
    HGLOBAL hMem = GetClipboardData(CF_TEXT);
    void *ptr = GlobalLock(hMem);
    int size = GlobalSize(hMem);
    if (size < len)
        memcpy(buf, ptr, size);
    else
        size = 0;
    GlobalUnlock(hMem);
    CloseClipboard();
    return size;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

static TCHAR szWindowClass[] = _T("Krish");
static TCHAR szTitle[] = _T("Krish");
static WNDCLASSEX wcex;
HINSTANCE appInstance = NULL;

gfx_t *__win32_gfx = NULL;
void *__win32_arg = NULL;



LRESULT CALLBACK WndProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    switch (uMsg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_ERASEBKGND:
        return 1;
    case WM_SIZE:
        if (__win32_gfx) {
            gfx_unmap(__win32_gfx);
            RECT rect;
            if (GetClientRect((HWND)__win32_gfx->fd, &rect) == TRUE) {
                __win32_gfx->width = rect.right;
                __win32_gfx->height = rect.bottom;
            }
            /*
            if (__win32_handlers->resize)
                __win32_handlers->resize(__win32_gfx, __win32_arg);*/
        }
        break;
    default:
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static void gfx_win32_init_()
{
    if (appInstance != NULL)
        return;
    appInstance = GetModuleHandle(NULL);
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = 0; // CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = appInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, _T("Call to RegisterClassEx failed."), _T("Win32 Error"), 0);
        exit(-1);
    }
}

/*
static void gfx_painting(gfx_t *gfx, gfx_handlers_t *handlers, void *arg, gfx_seat_t *seat)
{
    HWND hwnd = (HWND)gfx->fd;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    gfx_map(gfx);

    handlers->expose(gfx, arg, seat);

    // Copy buffer to device
    HBITMAP backbuffer = CreateBitmap(gfx->width, gfx->height, 1, 32, gfx->pixels);
    HDC backbuffDC = CreateCompatibleDC(hdc);
    SelectObject(backbuffDC, backbuffer);
    BitBlt(hdc, 0, 0, gfx->width, gfx->height, backbuffDC, 0, 0, SRCCOPY);

    DeleteObject(backbuffer);
    DeleteDC(backbuffDC);
    EndPaint(hwnd, &ps);
}*/


int gfx_open_window(gfx_t* gfx)
{
    if (appInstance == NULL)
        gfx_win32_init_();
    gfx->fd = (int)CreateWindowEx(WS_EX_CLIENTEDGE, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, gfx->width + 16, gfx->height + 39, NULL, NULL, appInstance, NULL);
    if (gfx->fd == 0) {
        free(gfx);
        return -1;
    }

    UINT timer;
    SetTimer((HWND)gfx->fd, (UINT_PTR)&timer, 25, NULL);
    gfx->flags = GFX_FL_PAINTTICK | GFX_FL_INVALID;
    return 0;
}

int gfx_close_window(gfx_t* gfx)
{
    DestroyWindow((HWND)gfx->fd);
    if (gfx->pixels)
        _aligned_free(gfx->pixels);
    return 0;
}

int gfx_flip(gfx_t* gfx)
{
    HWND hwnd = (HWND)gfx->fd;
#if 1
    RECT r;
    r.left = 0;
    r.top = 0;
    r.right = gfx->width;
    r.bottom = gfx->height;
    InvalidateRect(hwnd, &r, FALSE);
#else
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    HBITMAP backbuffer = CreateBitmap(gfx->width, gfx->height, 1, 32, gfx->pixels);
    HDC backbuffDC = CreateCompatibleDC(hdc);
    SelectObject(backbuffDC, backbuffer);
    BitBlt(hdc, 0, 0, gfx->width, gfx->height, backbuffDC, 0, 0, SRCCOPY);
    DeleteObject(backbuffer);
    DeleteDC(backbuffDC);
    EndPaint(hwnd, &ps);
#endif
    return 0;
}

void gfx_map_window(gfx_t *gfx)
{
    RECT rect;
    if (GetClientRect((HWND)gfx->fd, &rect) != FALSE) {
        gfx->width = rect.right;
        gfx->height = rect.bottom;
        gfx->pitch = ALIGN_UP(gfx->width * 4, 4);
    }
    gfx->pixels = _aligned_malloc(gfx->pitch * gfx->height, 1024 * 16);
 }

void gfx_unmap_window(gfx_t *gfx)
{
    if (gfx->pixels != NULL) {
        _aligned_free(gfx->pixels);
        gfx->pixels = NULL;
    }
}


int gfx_poll(gfx_t* gfx, gfx_msg_t* msg)
{
    MSG wm;
    HWND hwnd = (HWND)gfx->fd;
    for (;;) {
        if (!GetMessage(&wm, hwnd, 0, 0))
            continue;
        memset(msg, 0, sizeof(*msg));
        switch (wm.message) {
        case 0:
            msg->message = GFX_EV_QUIT;
            break;
        case WM_MOUSEMOVE:
            msg->message = GFX_EV_MOUSEMOVE;
            msg->param1 = wm.lParam;
            break;
        case WM_LBUTTONDOWN:
            msg->message = GFX_EV_BTNDOWN;
            msg->param1 = 1;
            break;
        case WM_LBUTTONUP:
            msg->message = GFX_EV_BTNUP;
            msg->param1 = 1;
            break;
        case WM_RBUTTONDOWN:
            msg->message = GFX_EV_BTNDOWN;
            msg->param1 = 2;
            break;
        case WM_RBUTTONUP:
            msg->message = GFX_EV_BTNUP;
            msg->param1 = 2;
            break;
        case WM_MBUTTONDOWN:
            msg->message = GFX_EV_BTNDOWN;
            msg->param1 = 4;
            break;
        case WM_MBUTTONUP:
            msg->message = GFX_EV_BTNUP;
            msg->param1 = 4;
            break;
        case WM_MOUSEWHEEL:
            msg->message = GFX_EV_MOUSEWHEEL;
            msg->param1 = (signed short)(wm.wParam >> 16) / 40;
            break;
        case WM_KEYDOWN:
            msg->message = GFX_EV_KEYDOWN;
            msg->param1 = (wm.lParam >> 16) & 0x7F;
            break;
        case WM_KEYUP:
            msg->message = GFX_EV_KEYUP;
            msg->param1 = (wm.lParam >> 16) & 0x7F;
            break;
        case WM_TIMER:
            msg->message = GFX_EV_TIMER;
            break;
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);

                HBITMAP backbuffer = CreateBitmap(gfx->width, gfx->height, 1, 32, gfx->pixels);
                HDC backbuffDC = CreateCompatibleDC(hdc);
                SelectObject(backbuffDC, backbuffer);
                BitBlt(hdc, 0, 0, gfx->width, gfx->height, backbuffDC, 0, 0, SRCCOPY);
                DeleteObject(backbuffer);
                DeleteDC(backbuffDC);
                EndPaint(hwnd, &ps);

                TranslateMessage(&wm);
                DispatchMessage(&wm);
            }
            continue;
        case WM_USER + 1:
            msg->message = wm.wParam;
            msg->param1 = wm.lParam;
            break;
        default:
            TranslateMessage(&wm);
            DispatchMessage(&wm);
            continue;
        }
        TranslateMessage(&wm);
        DispatchMessage(&wm);
        return 0;
    }
}


int gfx_push(gfx_t* gfx, int type, int param)
{
    HWND hwnd = (HWND)gfx->fd;
    PostMessage(hwnd, WM_USER + 1, type, param);
    return 0;
}

