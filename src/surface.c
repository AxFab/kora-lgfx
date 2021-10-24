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
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "mcrs.h"
#include "disto.h"

#ifdef _WIN32
gfx_ctx_t* __gfx_ctx = &gfx_ctx_win32;
#else
gfx_ctx_t* __gfx_ctx = &gfx_ctx_wns;
#endif

LIBAPI gfx_ctx_t *gfx_context(const char *name)
{
    if (name == NULL)
        return __gfx_ctx;
#ifdef _WIN32
    if (stricmp(name, "win32") == 0)
        __gfx_ctx = &gfx_ctx_win32;
#endif
    if (stricmp(name, "wns") == 0)
        __gfx_ctx = &gfx_ctx_wns;
    return __gfx_ctx;
}

LIBAPI gfx_t *gfx_create_window(int width, int height)
{
    width = MAX(1, MIN(16383, width));
    height = MAX(1, MIN(16383, height));

    gfx_t *gfx = gfx_create_surface(width, height);
    gfx->seat = calloc(sizeof(gfx_seat_t), 1);
    gfx_ctx_t *ctx = gfx_context(NULL);
    if (ctx->open(gfx) == 0)
        return gfx;

    free(gfx);
    return NULL;
}

LIBAPI gfx_t *gfx_create_surface(int width, int height)
{
    gfx_t *gfx = calloc(sizeof(gfx_t), 1);
    if (gfx == NULL)
        return NULL;
    gfx->width = width;
    gfx->height = height;
    gfx->pitch = ALIGN_UP(width * 4, 4);
    gfx->fd = -1;
    return gfx;
}

//LIBAPI gfx_t* gfx_open_surface(const char* path)
//{
//    gfx_t* gfx = calloc(sizeof(gfx_t), 1);
//    if (gfx_open_device(gfx, path) == -1) {
//        free(gfx);
//        return NULL;
//    }
//
//    gfx->seat = calloc(sizeof(gfx_seat_t), 1);
//    gfx->pitch = ALIGN_UP(gfx->width * 4, 4);
//    return gfx;
//}


LIBAPI void gfx_destroy(gfx_t *gfx)
{
    gfx_unmap(gfx);
    if (gfx->close != NULL)
        gfx->close(gfx);
    if (gfx->seat)
        free(gfx->seat);
    free(gfx);
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */





int gfx_load_image_bmp(gfx_t *gfx, int fd);
int gfx_load_image_tga(gfx_t *gfx, int fd);
int gfx_load_image_png(gfx_t *gfx, int fd);
// int gfx_load_image_jpg(gfx_t* gfx, int fd);
// int gfx_load_image_ppm(gfx_t* gfx, int fd);

int gfx_save_image_bmp(gfx_t *gfx, int fd, int mode);
int gfx_save_image_tga(gfx_t *gfx, int fd);
int gfx_save_image_png(gfx_t *gfx, int fd);
// int gfx_save_image_jpg(gfx_t* gfx, int fd);
// int gfx_save_image_ppm(gfx_t* gfx, int fd);

#define GFX_FRMT_BMP24 24
#define GFX_FRMT_BMP32 32

#ifndef O_BINARY
#  define O_BINARY  0
#endif

LIBAPI gfx_t *gfx_load_image(const char *name)
{
    int res = -1;
    gfx_t *gfx = calloc(1, sizeof(gfx_t));
    if (gfx == NULL)
        return NULL;
    gfx->fd = -1;
    int fd = open(name, O_RDONLY | O_BINARY);
    if (fd == -1)
        return NULL;

    if (strcmp(&name[strlen(name) - strlen(".bmp")], ".bmp") == 0)
        res = gfx_load_image_bmp(gfx, fd);
    else if (strcmp(&name[strlen(name) - strlen(".tga")], ".tga") == 0)
        res = gfx_load_image_tga(gfx, fd);
#if defined __USE_PNG
    else if (strcmp(&name[strlen(name) - strlen(".png")], ".png") == 0)
        res = gfx_load_image_png(gfx, fd);
#endif

    close(fd);
    if (res < 0) {
        free(gfx);
        gfx = NULL;
    }
    return gfx;
}

LIBAPI int gfx_save_image(gfx_t *gfx, const char *name)
{
    int res = -1;
    int fd = open(name, O_WRONLY | O_CREAT | O_BINARY);
    if (fd == -1)
        return -1;
    if (strcmp(&name[strlen(name) - strlen(".bmp")], ".bmp") == 0)
        res = gfx_save_image_bmp(gfx, fd, GFX_FRMT_BMP24);
    else if (strcmp(&name[strlen(name) - strlen(".tga")], ".tga") == 0)
        res = gfx_save_image_tga(gfx, fd);
#if defined __USE_PNG
    else if (strcmp(&name[strlen(name) - strlen(".png")], ".png") == 0)
        res = gfx_save_image_png(gfx, fd);
#endif

    close(fd);
    return res;
}
