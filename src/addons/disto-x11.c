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
#include "gfx.h"

void clipboard_copy(const char *buf, int len)
{
    ((void)buf);
    ((void)len);
}

int clipboard_paste(char *buf, int len)
{
    ((void)buf);
    ((void)len);
    return 0;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

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

int gfx_open_device(gfx_t *gfx, const char *path)
{
    //     if (input_fd == 0)
    //         gfx_initialize();
    //     gfx->fd = open(path, O_WRONLY | O_DIRECT);
    //     if (gfx->fd == -1)
    //         return -1;
    //     // READ SIZE
    //     uint32_t size = fcntl(gfx->fd, FB_SIZE);
    //     gfx->width = size & 0x7FFF;
    //     gfx->height = (size >> 16) & 0x7FFF;
    //     return 0;
    return -1;
}

int gfx_open_window(gfx_t *gfx)
{
    struct xwin *wi = calloc(sizeof(struct xwin), 1);
    wi->display = XOpenDisplay(NULL);
    gfx->fd = (long)wi;
    if (wi->display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        free(wi);
        return -1;
    }
    wi->screen = DefaultScreen(wi->display);
    wi->window = XCreateSimpleWindow(wi->display,
                                     RootWindow(wi->display, wi->screen), 10, 10,
                                     gfx->width, gfx->height, 1,
                                     BlackPixel(wi->display, wi->screen),
                                     WhitePixel(wi->display, wi->screen));
    XSelectInput(wi->display, wi->window, ExposureMask | KeyPressMask | KeyReleaseMask);
    XMapWindow(wi->display, wi->window);
    wi->gc = XDefaultGC(wi->display, wi->screen);
    return 0;
}

int gfx_close_window(gfx_t *gfx)
{
    struct xwin *wi = (struct xwin *)gfx->fd;
    XCloseDisplay(wi->display);
    return 0;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

void gfx_map_window(gfx_t *gfx)
{
    struct xwin *wi = (struct xwin *)gfx->fd;
    gfx->pitch = gfx->width * 4;
    gfx->pixels = calloc(gfx->pitch, gfx->height);
    wi->img = XCreateImage(wi->display, /* DirectColor */NULL, 4, XYBitmap, 0, gfx->pixels, gfx->width, gfx->height, 0, gfx->pitch);
}

void gfx_unmap_window(gfx_t *gfx)
{
    struct xwin *wi = (struct xwin *)gfx->fd;
    free(gfx->pixels);
    gfx->pixels = NULL;
    XDestroyImage(wi->img);
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


int gfx_flip(gfx_t *gfx)
{
    struct xwin *wi = (struct xwin *)gfx->fd;
    XPutImage(wi->display, wi->window, wi->gc, wi->img, 0, 0, 0, 0, gfx->width, gfx->height);
    return 0;
}

int gfx_poll(gfx_msg_t *msg)
{
    gfx_t *gfx = NULL;
    struct xwin *wi = (struct xwin *)gfx->fd;
    XEvent e;
    XKeyEvent *ke = (XKeyEvent *)&e;
    XNextEvent(wi->display, &e);
    switch (e.type) {
    case Expose:
        break;
    }
    return 0;
}


int gfx_push(gfx_t *gfx, int type, int param)
{
    return 0;
}
