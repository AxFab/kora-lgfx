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
#include <stdlib.h>
#include "mcrs.h"
#include "disto.h"


LIBAPI void *gfx_map(gfx_t *gfx)
{
    if (gfx->pixels == NULL) {
        if (gfx->map != NULL)
            gfx->map(gfx);
        else
            gfx->pixels = malloc(gfx->pitch * gfx->height);
    }
    return gfx->pixels;
}

LIBAPI void gfx_unmap(gfx_t *gfx)
{
    if (gfx->pixels == NULL)
        return;
    if (gfx->unmap != NULL)
        gfx->unmap(gfx);
    else
        free(gfx->pixels);
    gfx->pixels = NULL;
    gfx->backup = NULL;
}

LIBAPI int gfx_flip(gfx_t* gfx, gfx_clip_t *clip)
{
    if (gfx->flip != NULL)
        return gfx->flip(gfx, clip);
    return -1;
}

int gfx_width(gfx_t *gfx)
{
    return gfx->width;
}

int gfx_height(gfx_t *gfx)
{
    return gfx->height;
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

LIBAPI int gfx_resize(gfx_t *gfx, int width, int height)
{
    gfx_unmap(gfx);
    // if (gfx->map != NULL)
    //     return -1;
    gfx->pitch = ALIGN_UP(width * 4, 4);
    gfx->width = width;
    gfx->height = height;
    if (gfx->resize != NULL)
        gfx->resize(gfx);
    return 0;
}


