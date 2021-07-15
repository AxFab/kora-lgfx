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



PACK(struct tga_header {
   char  idlength;
   char  colourmaptype;
   char  datatypecode;
   short int colourmaporigin;
   short int colourmaplength;
   char  colourmapdepth;
   short int x_origin;
   short int y_origin;
   short width;
   short height;
   char  bitsperpixel;
   char  imagedescriptor;
});


void gfx_tga_merge_bytes(gfx_t* gfx, int n, uint8_t *px, int bytes)
{
    int x = n % gfx->width;
    int y = gfx->height - 1 - n / gfx->width;

    int v = y * gfx->width + x;
    if (bytes == 4) {
        gfx->pixels[v * 4 + 2] = px[2];
        gfx->pixels[v * 4 + 1] = px[1];
        gfx->pixels[v * 4 + 0] = px[0];
        gfx->pixels[v * 4 + 3] = px[3];
   } else if (bytes == 3) {
       gfx->pixels[v * 4 + 2] = px[2];
       gfx->pixels[v * 4 + 1] = px[1];
       gfx->pixels[v * 4 + 0] = px[0];
       gfx->pixels[v * 4 + 3] = 255;
   } else if (bytes == 2) {
       gfx->pixels[v * 4 + 2] = (px[1] & 0x7c) << 1;
       gfx->pixels[v * 4 + 1] = ((px[1] & 0x03) << 6) | ((px[0] & 0xe0) >> 2);
       gfx->pixels[v * 4 + 0] = (px[0] & 0x1f) << 3;
       gfx->pixels[v * 4 + 3] = (px[1] & 0x80);
   } else if (bytes == 1) {
       gfx->pixels[v * 4 + 2] = (px[0] & 0xe0);
       gfx->pixels[v * 4 + 1] = (px[0] & 0x1c) << 3;
       gfx->pixels[v * 4 + 0] = (px[0] & 0x3) << 6;
       gfx->pixels[v * 4 + 3] = 255;
   }
}

void gfx_tga_merge_gray(gfx_t* gfx, int n, uint8_t px)
{
    int x = n % gfx->width;
    int y = gfx->height - 1 - n / gfx->width;

    int v = y * gfx->width + x;
    gfx->pixels[v * 4 + 2] = px;
    gfx->pixels[v * 4 + 1] = px;
    gfx->pixels[v * 4 + 0] = px;
    gfx->pixels[v * 4 + 3] = 255;
}

static inline int gfx_error(const char* str) {
    fprintf(stderr, str);
    return -1;
}

int gfx_tga_read_uncompressed(gfx_t* gfx, int fd, int bytesGrp)
{
    int n = 0;
    uint8_t px[5];
    while (n < gfx->width * gfx->height) {
        if (read(fd, px, bytesGrp) != bytesGrp)
            return gfx_error("Unexpected end of file\n");
        gfx_tga_merge_bytes(gfx, n++, px, bytesGrp);
    }
    return 0;
}

int gfx_tga_read_uncompressed_grey(gfx_t* gfx, int fd)
{
    int n = 0;
    uint8_t px[5];
    while (n < gfx->width * gfx->height) {
        if (read(fd, px, 1) != 1)
            return gfx_error("Unexpected end of file\n");
        
        gfx_tga_merge_gray(gfx, n++, px[0]);
    }
    return 0;
}

int gfx_tga_read_compressed(gfx_t* gfx, int fd, int bytesGrp)
{
    int n = 0;
    uint8_t px[5];
    while (n < gfx->width * gfx->height) {
        if (read(fd, px, bytesGrp + 1) != bytesGrp)
            return gfx_error("Unexpected end of file\n");

        int i, j = px[0] & 0x7f;
        gfx_tga_merge_bytes(gfx, n++, &px[1], bytesGrp);

        if (px[0] & 0x80) { /* RLE chunk */
            for (i = 0; i < j; i++) {
                gfx_tga_merge_bytes(gfx, n++, &px[1], bytesGrp);
            }

        }
        else { /* Normal chunk */
            for (i = 0; i < j; i++) {
                if (read(fd, px, bytesGrp) != bytesGrp)
                    return gfx_error("Unexpected end of file\n");
                gfx_tga_merge_bytes(gfx, n++, px, bytesGrp);
            }
        }
    }
    return 0;
}

int gfx_tga_read_compressed_gray(gfx_t* gfx, int fd)
{
    int n = 0;
    uint8_t px[5];
    while (n < gfx->width * gfx->height) {
        if (read(fd, px, 1 + 1) != 1)
            return gfx_error("Unexpected end of file\n");

        int i, j = px[0] & 0x7f;
        gfx_tga_merge_gray(gfx, n++, px[1]);

        if (px[0] & 0x80) { /* RLE chunk */
            for (i = 0; i < j; i++) {
                gfx_tga_merge_gray(gfx, n++, px[1]);
            }
        }
        else { /* Normal chunk */
            for (i = 0; i < j; i++) {
                if (read(fd, px, 1) != 1)
                    return gfx_error("Unexpected end of file\n");
                gfx_tga_merge_gray(gfx, n++, px[1]);
            }
        }
    }
    return 0;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


int gfx_load_image_tga(gfx_t* gfx, int fd)
{
    struct tga_header head;
    if (read(fd, &head, sizeof(head)) != sizeof(head))
        return -1;

    /* What can we handle */
    if (head.colourmaptype != 0 && head.colourmaptype != 1)
        return gfx_error("Can only handle colour map types of 0 and 1\n");

    /* Skip over unnecessary stuff */
    int skipover = head.idlength + head.colourmaptype * head.colourmaplength;
    // fprintf(stderr,"Skip over %d bytes\n",skipover);
    lseek(fd, skipover, SEEK_CUR);

    gfx_resize(gfx, head.width, head.height);
    gfx_map(gfx);


    /* Read the image */
    int bytesGrp = head.bitsperpixel / 8;
    if (head.datatypecode == 2)
        return gfx_tga_read_uncompressed(gfx, fd, bytesGrp);

    else if (head.datatypecode == 3 && head.bitsperpixel == 8)
        return gfx_tga_read_uncompressed_grey(gfx, fd);

    else if (head.datatypecode == 10)
        return gfx_tga_read_compressed(gfx, fd, bytesGrp);

    else if (head.datatypecode == 11)
        return gfx_tga_read_compressed_gray(gfx, fd);

    else
        return gfx_error("\n");
    return -1;
}

int gfx_save_image_tga(gfx_t* gfx, int fd)
{
    // struct tga_header head;
    gfx_map(gfx);

    // memset(&head, 0, sizeof(head));
    // head.magic = 0x4d42;
    // head.offset = sizeof(head);
    // head.hsize = sizeof(head) - 0xE;
    // head.width = gfx->width;
    // head.height = gfx->height;
    // head.planes = 1;

    // if (mode == GFX_FRMT_BMP24) {
    //     head.bitsperpx = 24;
    //     head.isize = gfx->width * 3 * gfx->height;
    // }
    // else if (mode == GFX_FRMT_BMP32) {
    //     head.bitsperpx = 32;
    //     head.isize = ALIGN_UP(gfx->width * 4 * gfx->height, 4);
    // }

    // head.fsize = sizeof(head) + head.isize;

    // lseek(fd, 0, SEEK_SET);
    // if (write(fd, &head, sizeof(head)) != sizeof(head))
    //     return -1;

    // lseek(fd, head.offset, SEEK_SET);
    // if (mode == GFX_FRMT_BMP24)
    //     return gfx_write_bmp24(gfx, fd);
    // if (mode == GFX_FRMT_BMP32)
    //     return gfx_write_bmp32(gfx, fd);

    return -1;
}



