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
#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include "gfx.h"
#include "../disto.h"

int gfx_clipboard_copy(const char *buf, int len)
{
    ((void)buf);
    ((void)len);
    return 0;
}

int gfx_clipboard_paste(char *buf, int len)
{
    ((void)buf);
    ((void)len);
    return 0;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

Display *__display = NULL;

struct xwin {
    Display *display;
    Window window;
    GC gc;
    int screen;
    XImage *img;
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

// Status XInitImage(XImage *image);
// XImage *XCreateImage(Display *display, Visual *visual,
//      unsigned int depth, int format, int offset,
//      char *data, unsigned int width, unsigned int height,
//      int bitmap_pad, int bytes_per_line);
// unsigned long XGetPixel(XImage *ximage, int x, int y);
// int XPutPixel(XImage *ximage, int x, int y, unsigned long pixel);
// XImage *XSubImage(XImage *ximage, int x, int y, unsigned int subimage_width, unsigned int subimage_height);
// int XAddPixel(XImage *ximage, long value);
// int XDestroyImage(XImage *ximage);

int gfx_close_x11(gfx_t *gfx)
{
    struct xwin *wi = (struct xwin *)gfx->fd;
    XUnmapWindow(wi->display, wi->window);
    XCloseDisplay(wi->display);
    return 0;
}

void gfx_map_x11(gfx_t *gfx)
{
    if (gfx->pixels != NULL)
        return;
    struct xwin *wi = (struct xwin *)gfx->fd;
    gfx->pitch = gfx->width * 4;
    gfx->pixels = calloc(gfx->pitch, gfx->height * 2);
    Visual *vinfo = XDefaultVisual(wi->display, wi->screen);
    wi->img = XCreateImage(wi->display, vinfo, 32, XYPixmap, 0, gfx->pixels, gfx->width, gfx->height, 32, 0);
    if (wi->img == NULL) {
        fprintf(stderr, "Unable to allocate x11 image\n");
    }
    return 0;
}

void gfx_unmap_x11(gfx_t *gfx)
{
    struct xwin *wi = (struct xwin *)gfx->fd;
    free(gfx->pixels);
    gfx->pixels = NULL;
    XDestroyImage(wi->img);
    return 0;
}

int gfx_flip_x11(gfx_t *gfx)
{
    struct xwin *wi = (struct xwin *)gfx->fd;
    // Pixmap pxm = XCreatePixmap(wi->display, wi->window, gfx->width, gfx->height, 24);
    XPutImage(wi->display, wi->window, wi->gc, wi->img, 0, 0, 0, 0, gfx->width, gfx->height);
    // XCopyArea(wi->display, pxm, wi->window, wi->gc, 0, 0, gfx->width, gfx->height, 0, 0);
    XFlush(wi->display);
    return 0;
}

int gfx_open_x11(gfx_t *gfx)
{
    struct xwin *wi = calloc(sizeof(struct xwin), 1);
    if (__display == NULL)
        __display = XOpenDisplay(NULL);
    wi->display = __display;
    gfx->fd = (long)wi;
    if (wi->display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        free(wi);
        return -1;
    }
    wi->screen = DefaultScreen(wi->display);
    Visual *vinfo = XDefaultVisual(wi->display, wi->screen);
    // wi->window = XCreateWindow(wi->display,
    //     RootWindow(wi->display, wi->screen),
    //     10, 10, gfx->width, gfx->height, 2, 32, InputOutput,
    //     vinfo, 0, NULL);

    wi->window = XCreateSimpleWindow(wi->display,
                                     RootWindow(wi->display, wi->screen), 0, 0,
                                     gfx->width, gfx->height, 2,
                                     BlackPixel(wi->display, wi->screen),
                                     WhitePixel(wi->display, wi->screen));
    XSelectInput(wi->display, wi->window, ExposureMask | KeyPressMask | KeyReleaseMask);
    XMapWindow(wi->display, wi->window);

    // Status st = XMatchVisualInfo(wi->display, wi->screen, 4, class, &wi->vinfo);

    wi->gc = XDefaultGC(wi->display, wi->screen);
    gfx->map = gfx_map_x11;
    gfx->unmap = gfx_unmap_x11;
    gfx->flip = gfx_flip_x11;
    gfx->close = gfx_close_x11;
    return 0;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

int gfx_open_device(gfx_t *gfx, const char *path)
{
    return -1;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


int gfx_poll_x11(gfx_msg_t *msg)
{
    // gfx_t *gfx = NULL;
    // struct xwin *wi = (struct xwin *)gfx->fd;
    XEvent e;
    XKeyEvent *ke = (XKeyEvent *)&e;
    XButtonEvent *xb = (XButtonEvent *)&e;
    XMotionEvent *xm = (XMotionEvent *)&e;
    XCrossingEvent *xc = (XCrossingEvent *)&e;
    XNextEvent(__display, &e);
    switch (e.type) {
    case Expose:
        break;
    case KeyPress:
        break;
    case KeyRelease:
        break;
    case ButtonPress:
        break;
    case ButtonRelease:
        break;
    case MotionNotify:
        break;
    case EnterNotify:
        break;
    case LeaveNotify:
        break;
    }
    return -1;
}


void _gfx_itimer_handler(int n)
{
    gfx_push(NULL, GFX_EV_TIMER, 0, 0);
}

int gfx_timer_x11(int delay, int interval)
{
    struct itimerval val;
    memset(&val, 0, sizeof(val));
    val.it_interval.tv_sec = delay / 1000;
    val.it_interval.tv_usec = (delay % 1000) * 1000;
    val.it_value.tv_sec = delay / 1000;
    val.it_value.tv_usec = (delay % 1000) * 1000;
    signal(SIGALRM, _gfx_itimer_handler);
    // signal(SIGVTALRM, _gfx_itimer_handler);
    int ret = setitimer(ITIMER_REAL, &val, NULL);
    return ret == 0 ? 1 : -1;
}


gfx_ctx_t gfx_ctx_x11 = {
    .open = gfx_open_x11,
    .poll = gfx_poll_x11,
    .timer = gfx_timer_x11,
};
