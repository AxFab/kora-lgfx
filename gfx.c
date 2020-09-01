#include <stdlib.h>
#include "mcrs.h"
#include "gfx-disto.h"

gfx_t *gfx_create_window(void *ctx, int width, int height)
{
    width = MAX(1, MIN(16383, width));
    height = MAX(1, MIN(16383, height));

    gfx_t *gfx = gfx_create_surface(width, height);
    gfx->flags = GFX_FL_INVALID | GFX_FL_EXPOSED;
    if (ctx == NULL && gfx_open_window(gfx) == 0)
        return gfx;

    free(gfx);
    return NULL;
}

gfx_t *gfx_create_surface(int width, int height)
{
    gfx_t *gfx = calloc(sizeof(gfx_t), 1);
    gfx->width = width;
    gfx->height = height;
    gfx->pitch = ALIGN_UP(width * 4, 4);
    gfx->fd = -1;
    gfx->fi = -1;
    return gfx;
}

void gfx_destroy(gfx_t *gfx)
{
    gfx_unmap(gfx);
    if (gfx->fd != -1)
        gfx_close_window(gfx);
    free(gfx);
}


int gfx_resize(gfx_t* gfx, int width, int height)
{
    gfx_unmap(gfx);
    if (gfx->fd != -1)
        return -1;
    gfx->pitch = ALIGN_UP(width * 4, 4);
    gfx->width = width;
    gfx->height = height;
    return 0;
}


void *gfx_map(gfx_t* gfx)
{
    if (gfx->pixels == NULL) {
        if (gfx->fd != -1)
            gfx_map_window(gfx);
        else
            gfx->pixels = malloc(gfx->pitch * gfx->height);
    }
    return gfx->pixels;
}

void gfx_unmap(gfx_t* gfx)
{
    if (gfx->pixels == NULL)
        return;
    if (gfx->fd != -1)
        gfx_unmap_window(gfx);
    else
        free(gfx->pixels);
    gfx->pixels = NULL;
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

#include <assert.h>
#include <string.h>
#include <threads.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


gfx_t* gfx_opend(int fd, int fi)
{
    gfx_t* gfx = malloc(sizeof(gfx_t));
    gfx->width = 1280;
    gfx->height = 720;
    gfx->fd = fd;
    gfx->fi = fi;
    gfx->pixels = NULL;
    gfx->backup = NULL;
    gfx->pitch = gfx->width * 4;
    gfx->flags = GFX_FL_PAINTTICK | GFX_FL_INVALID;
    return gfx;
}




/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


int gfx_load_image_bmp(gfx_t* gfx, int fd);
int gfx_load_image_png(gfx_t* gfx, int fd);

#ifndef O_BINARY
#  define O_BINARY  0
#endif

gfx_t* gfx_load_image(const char* name)
{
    int res = -1;
    gfx_t* gfx = calloc(1, sizeof(gfx_t));
    if (gfx == NULL)
        return NULL;
    gfx->fd = -1;
    gfx->fi = -1;
    int fd = open(name, O_RDONLY | O_BINARY);
    if (fd == -1)
        return NULL;

    if (strcmp(&name[strlen(name) - strlen(".bmp")], ".bmp") == 0) {
        res = gfx_load_image_bmp(gfx, fd);
#if defined __USE_PNG
    } else if (strcmp(&name[strlen(name) - strlen(".png")], ".png") == 0) {
        res = gfx_load_image_png(gfx, fd);
#endif
    }
    close(fd);
    if (res < 0) {
        free(gfx);
        gfx = NULL;
    }
    return gfx;
}
