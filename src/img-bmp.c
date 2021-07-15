/*
 *      This file is part of the KoraOS project.
 *  Copyright (C) 2015-2019  <Fabien Bavent>
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
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "gfx.h"
#include "mcrs.h"


PACK(struct bmp_header {
    uint16_t magic;
    uint32_t fsize;
    uint32_t reserved;
    uint32_t offset;
    uint32_t hsize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitsperpx;
    uint32_t compression;
    uint32_t isize;
    uint32_t xresol;
    uint32_t yresol;
    uint32_t a;
    uint32_t b;
});

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

static int gfx_read_bmp8(gfx_t *gfx, int fd)
{
    int c, x, y = gfx->height;
    int r = gfx->width % 4;
    while (y-- > 0) {
        for (x = 0; x < gfx->width; ++x) {
            c = 0;
            read(fd, &c, 2);
            // c =
            gfx->pixels4[y * gfx->width + x] = c;
        }
        if (r != 0)
            read(fd, &x, r);
    }
    return 0;
}

static int gfx_read_bmp15(gfx_t *gfx, int fd)
{
    int c, x, y = gfx->height;
    int r = gfx->width % 4;
    while (y-- > 0) {
        for (x = 0; x < gfx->width; ++x) {
            c = 0;
            read(fd, &c, 2);
            // c =
            gfx->pixels4[y * gfx->width + x] = c;
        }
        if (r != 0)
            read(fd, &x, r);
    }
    return 0;
}

static int gfx_read_bmp16(gfx_t *gfx, int fd)
{
    int c, x, y = gfx->height;
    int r = gfx->width % 4;
    while (y-- > 0) {
        for (x = 0; x < gfx->width; ++x) {
            c = 0;
            read(fd, &c, 2);
            // c =
            gfx->pixels4[y * gfx->width + x] = c;
        }
        if (r != 0)
            read(fd, &x, r);
    }
    return 0;
}

static int gfx_read_bmp24(gfx_t *gfx, int fd)
{
    int x, y = gfx->height;
    int r = gfx->width % 4;
    while (y-- > 0) {
        for (x = 0; x < gfx->width; ++x) {
            gfx->pixels4[y * gfx->width + x] = 0;
            read(fd, &gfx->pixels4[y * gfx->width + x], 3);
        }
        if (r != 0)
            read(fd, &x, r);
    }
    return 0;
}

static int gfx_read_bmp32(gfx_t *gfx, int fd)
{
    int y = gfx->height;
    while (y-- > 0)
        read(fd, &gfx->pixels4[y * gfx->width], gfx->pitch);
    return 0;
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

static int gfx_write_bmp24(gfx_t *gfx, int fd)
{
    int x, y = gfx->height;
    int r = gfx->pitch % 4;
    while (y-- > 0) {
        for (x = 0; x < gfx->width; ++x)
            write(fd, &gfx->pixels4[y * gfx->width + x], 3);
        if (r != 0) {
            x = 0;
            write(fd, &x, r);
        }
    }
    return 0;
}

static int gfx_write_bmp32(gfx_t *gfx, int fd)
{
    int y = gfx->height;
    while (y-- > 0)
        write(fd, &gfx->pixels4[y * gfx->width], gfx->pitch);
    return 0;
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

#define GFX_FRMT_BMP24 24
#define GFX_FRMT_BMP32 32

int gfx_load_image_bmp(gfx_t *gfx, int fd)
{
    struct bmp_header head;
    if (read(fd, &head, sizeof(head)) != sizeof(head))
        return -1;
    if (head.magic != 0x4d42)
        return -1;
    gfx_resize(gfx, head.width, head.height);
    gfx_map(gfx);
    lseek(fd, head.offset, SEEK_SET);

    if (head.planes == 1 && head.compression == 0) {
        if (head.bitsperpx == 24)
            return gfx_read_bmp24(gfx, fd);
        if (head.bitsperpx == 32)
            return gfx_read_bmp32(gfx, fd);
        if (head.bitsperpx == 16)
            return gfx_read_bmp16(gfx, fd);
        if (head.bitsperpx == 15)
            return gfx_read_bmp15(gfx, fd);
        if (head.bitsperpx == 8)
            return gfx_read_bmp8(gfx, fd);
    }

    return -1;
}

int gfx_save_image_bmp(gfx_t *gfx, int fd, int mode)
{
    struct bmp_header head;
    gfx_map(gfx);

    memset(&head, 0, sizeof(head));
    head.magic = 0x4d42;
    head.offset = sizeof(head);
    head.hsize = sizeof(head) - 0xE;
    head.width = gfx->width;
    head.height = gfx->height;
    head.planes = 1;

    if (mode == GFX_FRMT_BMP24) {
        head.bitsperpx = 24;
        head.isize = gfx->width * 3 * gfx->height;
    } else if (mode == GFX_FRMT_BMP32) {
        head.bitsperpx = 32;
        head.isize = ALIGN_UP(gfx->width * 4 * gfx->height, 4);
    }

    head.fsize = sizeof(head) + head.isize;

    lseek(fd, 0, SEEK_SET); 
    if (write(fd, &head, sizeof(head)) != sizeof(head))
        return -1;

    lseek(fd, head.offset, SEEK_SET);
    if (mode == GFX_FRMT_BMP24)
        return gfx_write_bmp24(gfx, fd);
    if (mode == GFX_FRMT_BMP32)
        return gfx_write_bmp32(gfx, fd);


    return -1;
}

