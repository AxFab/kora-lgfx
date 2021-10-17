/*
 *      This file is part of the KoraOS project.
 *  Copyright (C) 2015-2021  <Fabien Bavent>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   - - - - - - - - - - - - - - -
 */
#include "gfx.h"
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "../disto.h"
#include "../mcrs.h"

#define COLOR_REF(n)  ( (((n) & 0xFF0000) >> 16) | ((n) & 0xFF00) | (((n) & 0xFF) << 16) )

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

LIBAPI int gfx_clipboard_copy(const char *buf, int len)
{
    if (buf == NULL)
        return -1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    if (hMem == 0)
        return -1;
    LPVOID ptr = GlobalLock(hMem);
    if (ptr == NULL)
        return -1;
    memcpy(ptr, buf, len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
    return len;
}

LIBAPI int gfx_clipboard_paste(char *buf, int len)
{
    if (buf == NULL)
        return -1;
    OpenClipboard(0);
    HGLOBAL hMem = GetClipboardData(CF_TEXT);
    if (hMem == 0)
        return -1;
    void *ptr = GlobalLock(hMem);
    if (ptr == NULL)
        return -1;
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

typedef struct gfxhandle gfxhandle_t;
struct gfxhandle {
    gfx_t *gfx;
    HWND hwnd;
    gfxhandle_t *next;
};

gfxhandle_t *__handle = NULL;
gfx_t *__build_win = NULL;

gfx_t *__gfx_from_hwnd(HWND hwnd)
{
    if (__handle == NULL)
        return __build_win;
    gfxhandle_t *handle = __handle;
    while (handle->hwnd != hwnd) {
        handle = handle->next;
        if (handle == NULL)
            return __build_win;
    }
    return handle->gfx;
}

void __gfx_rm_hwnd(HWND hwnd)
{
    if (__handle == NULL)
        return;

    gfxhandle_t *handle = __handle;
    if (__handle->hwnd == hwnd) {
        __handle = __handle->next;
        free(handle);
        return;
    }

    while (handle->next && handle->next->hwnd != hwnd)
        handle = handle->next;

    if (handle->next && handle->next->hwnd == hwnd) {
        gfxhandle_t *old = handle->next;
        handle->next = handle->next->next;
        free(old);
    }
}

LRESULT CALLBACK WndProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    gfx_t *gfx = __gfx_from_hwnd(hwnd);
    switch (uMsg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        gfx_push(gfx, GFX_EV_QUIT, 0);
        break;
    case WM_ERASEBKGND:
        return 1;
    case WM_SIZE:
        gfx = __gfx_from_hwnd(hwnd);
        if (gfx) {
            gfx_unmap(gfx);
            RECT rect;
            if (GetClientRect((HWND)gfx->fd, &rect) == TRUE) {
                gfx->width = rect.right;
                gfx->height = rect.bottom;
            }
            gfx_push(gfx, GFX_EV_RESIZE, GFX_POINT(gfx->width, gfx->height));
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

void _gfx_paint(HWND hwnd, gfx_t *gfx)
{
    PAINTSTRUCT ps;
    // Find the GFX who match
    HDC hdc = BeginPaint(hwnd, &ps);
    void *pixels = gfx_map(gfx);
    HBITMAP backbuffer = CreateBitmap(gfx->width, gfx->height, 1, 32, pixels);
    HDC backbuffDC = CreateCompatibleDC(hdc);
    SelectObject(backbuffDC, backbuffer);
    BitBlt(hdc, 0, 0, gfx->width, gfx->height, backbuffDC, 0, 0, SRCCOPY);
    DeleteObject(backbuffer);
    DeleteDC(backbuffDC);
    EndPaint(hwnd, &ps);
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

void gfx_map_win32(gfx_t *gfx)
{
    RECT rect;
    if (GetClientRect((HWND)gfx->fd, &rect) != FALSE) {
        gfx->width = rect.right;
        gfx->height = rect.bottom;
        gfx->pitch = ALIGN_UP(gfx->width * 4, 4);
    }
    gfx->pixels = _aligned_malloc(gfx->pitch * gfx->height, 1024 * 16);
}

void gfx_unmap_win32(gfx_t *gfx)
{
    if (gfx->pixels != NULL) {
        _aligned_free(gfx->pixels);
        gfx->pixels = NULL;
    }
}

int gfx_flip_win32(gfx_t *gfx, gfx_clip_t *clip)
{
    HWND hwnd = (HWND)gfx->fd;
    RECT r;
    r.left = clip ? clip->left : 0;
    r.top = clip ? clip->top : 0;
    r.right = clip ? clip->right : gfx->width;
    r.bottom = clip ? clip->bottom : gfx->height;
    InvalidateRect(hwnd, &r, FALSE);
    return 0;
}

int gfx_close_win32(gfx_t *gfx)
{
    HWND hwnd = (HWND)gfx->fd;
    DestroyWindow(hwnd);
    if (gfx->pixels)
        _aligned_free(gfx->pixels);

    __gfx_rm_hwnd(hwnd);
    return 0;
}

int gfx_open_win32(gfx_t *gfx)
{
    if (appInstance == NULL)
        gfx_win32_init_();
    __build_win = gfx;
    gfx->fd = (int)CreateWindowEx(WS_EX_CLIENTEDGE, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, gfx->width + 16, gfx->height + 39, NULL, NULL, appInstance, NULL);
    // gfx->fd = (int)CreateWindowEx(WS_EX_CLIENTEDGE, szWindowClass, szTitle, WS_POPUP | WS_VISIBLE, 0, 0, gfx->width, gfx->height, NULL, NULL, appInstance, NULL);
    __build_win = NULL;
    if (gfx->fd == 0) {
        free(gfx);
        return -1;
    }

    gfxhandle_t *handle = calloc(sizeof(gfxhandle_t), 1);
    if (handle == NULL) {
        free(gfx);
        return -1;
    }
    handle->gfx = gfx;
    handle->hwnd = (HWND)gfx->fd;
    handle->next = __handle;
    __handle = handle;

    SetWindowPos(gfx->fd, HWND_TOPMOST, 50, 50, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
    // UINT timer;
    // SetTimer((HWND)gfx->fd, (UINT_PTR)&timer, 25, NULL);
    // gfx->flags = GFX_FL_PAINTTICK | GFX_FL_INVALID;
    gfx->map = gfx_map_win32;
    gfx->unmap = gfx_unmap_win32;
    gfx->flip = gfx_flip_win32;
    gfx->close = gfx_close_win32;
    return 0;
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

int gfx_poll_win32(gfx_msg_t *msg)
{
    MSG wm;
    for (;;) {
        if (!GetMessage(&wm, NULL, 0, 0))
            continue;
        memset(msg, 0, sizeof(*msg));
        msg->gfx = __gfx_from_hwnd(wm.hwnd);
        switch (wm.message) {
        case 0:
        case WM_QUIT:
        case WM_CLOSE:
        case WM_DESTROY:
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
        case WM_USER + 1:
            msg->message = wm.wParam;
            msg->param1 = wm.lParam;
            break;
        case WM_PAINT:
            _gfx_paint(wm.hwnd, msg->gfx);
        default:
            if (wm.message < 0x60 && wm.message != WM_PAINT)
                printf("Need to handle message %x\n", wm.message);
            TranslateMessage(&wm);
            DispatchMessage(&wm);
            continue;
        }
        TranslateMessage(&wm);
        DispatchMessage(&wm);
        return 0;
    }
}

int gfx_timer_win32(int delay, int interval)
{
    if (interval != 0) {
        UINT timer = 0;
        SetTimer(NULL, (UINT_PTR)&timer, interval, NULL);
        return timer;
    } else {
        // TODO DELAY !?
        return 0;
    }
}

gfx_ctx_t gfx_ctx_win32 = {
    .open = gfx_open_win32,
    .poll = gfx_poll_win32,
    .timer = gfx_timer_win32,
};
