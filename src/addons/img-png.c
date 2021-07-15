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
#include <stdlib.h>
#include <unistd.h>
#include <png.h>
#include "gfx.h"

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

int gfx_png_error_ptr;

void gfx_png_error_fn(png_structp png_ptr, png_const_charp png_charp)
{
    ((void)png_ptr);
    ((void)png_charp);
}

void gfx_png_warning_fn(png_structp png_ptr, png_const_charp png_charp)
{
    ((void)png_ptr);
    ((void)png_charp);
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

int gfx_load_image_png(gfx_t *gfx, int fd)
{
    uint8_t sig[16];
    read(fd, sig, 16);
    if (png_sig_cmp(sig, 0, 16) != 0)
        return -1;

    png_structp png_ptr = png_create_read_struct
                          (PNG_LIBPNG_VER_STRING, (png_voidp)&gfx_png_error_ptr,
                           gfx_png_error_fn, (png_error_ptr)gfx_png_warning_fn);

    if (!png_ptr)
        return -1;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr,
                                (png_infopp)NULL, (png_infopp)NULL);
        return -1;
    }

    lseek(fd, 0, SEEK_SET);
    FILE *fp = fdopen(fd, "rb");

    png_init_io(png_ptr, fp);

    png_read_info(png_ptr, info_ptr);
    gfx->width = png_get_image_width(png_ptr, info_ptr);
    gfx->height = png_get_image_height(png_ptr, info_ptr);
    gfx->pitch = gfx->width * 4;

    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);
    if (bit_depth == 16) {
        png_set_strip_16(png_ptr);
        // png_set_swap(png_ptr);
    }
    if (bit_depth < 8) {
        png_set_packing(png_ptr);
        png_set_packswap(png_ptr);
    }


    gfx_map(gfx);

    png_bytep *row_ptrs = malloc(gfx->height * sizeof(png_bytep *));
    for (int i = 0; i < gfx->height; ++i)
        row_ptrs[i] = &gfx->pixels[i * gfx->pitch];

    png_read_image(png_ptr, row_ptrs);


    // TODO -- Should be done by libpng !!
    for (int i = 0; i < gfx->height; ++i) {
        for (int j = 0; j < gfx->width; ++j) {
            uint32_t value = gfx->pixels4[i * gfx->width + j];
            value = (value & 0xff00ff00) | ((value & 0xff) << 16) | ((value & 0xff0000) >> 16);
            gfx->pixels4[i * gfx->width + j] = value;
        }
    }

    free(row_ptrs);
    // fclose(fp); -- Without closing fd.
    return 0;
}

int gfx_save_image_png(gfx_t* gfx, int fd)
{
    ((void)gfx);
    ((void)fd);
    return -1;
}

