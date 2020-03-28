#include <assert.h>
#include <string.h>
#include <kora/gfx.h>
#include <threads.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static void *memcpy32(void *dest, void *src, size_t lg)
{
    assert(IS_ALIGNED(lg, 4));
    assert(IS_ALIGNED((size_t)dest, 4));
    assert(IS_ALIGNED((size_t)src, 4));
    register uint32_t *a = (uint32_t *)src;
    register uint32_t *b = (uint32_t *)dest;
    while (lg > 16) {
        b[0] = a[0];
        b[1] = a[1];
        b[2] = a[2];
        b[3] = a[3];
        lg -= 16;
        a += 4;
        b += 4;
    }
    while (lg > 0) {
        b[0] = a[0];
        lg -= 4;
        a++;
        b++;
    }
    return dest;
}

static void *memset32(void *dest, uint32_t val, size_t lg)
{
    assert(IS_ALIGNED(lg, 4));
    assert(IS_ALIGNED((size_t)dest, 4));
    register uint32_t *a = (uint32_t *)dest;
    while (lg > 16) {
        a[0] = val;
        a[1] = val;
        a[2] = val;
        a[3] = val;
        lg -= 16;
        a += 4;
    }
    while (lg > 0) {
        a[0] = val;
        lg -= 4;
        a++;
    }
    return dest;
}


void gfx_rect(gfx_t *gfx, int x, int y, int w, int h, uint32_t color)
{
    int j;
    int minx = MAX(0, x);
    int maxx = MIN(gfx->width, x + w);
    int miny = MAX(0, y);
    int maxy = MIN(gfx->height, y + h);
    (void)gfx->backup;
    if (minx >= maxx)
        return;
    for (j = miny; j < maxy; ++j)
        memset32(&gfx->pixels[j * gfx->pitch + minx * 4], color, (maxx - minx) * 4);
}


void gfx_clear(gfx_t *gfx, uint32_t color)
{
    (void)gfx->fi;
    memset32(gfx->pixels, color, gfx->pitch * gfx->height);
}


void gfx_copy(gfx_t *dest, gfx_t *src, int x, int y, int w, int h)
{
    int i;
    int minx = MAX(0, -x);
    int maxx = MIN(MIN(w, src->width), dest->width - x);
    int miny = MAX(0, -y);
    int maxy = MIN(MIN(h, src->height), dest->height - y);

    for (i = miny; i < maxy; ++i) {
        int ka = i * src->pitch;
        int kb = (i + y) * dest->pitch;
        memcpy32(&dest->pixels[kb + (minx + x) * 4], &src->pixels[ka + minx * 4], (maxx - minx) * 4);
    }
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

static void gfx_fill_alpha_blending(gfx_t* dest, uint32_t color, int minx, int maxx, int miny, int maxy)
{
    int j, i, k;
    float a0 = ((color >> 24) & 0xff) / 255.0;
    for (i = miny; i < maxy; ++i) {
        k = i * dest->pitch;
        for (j = minx; j < maxx; ++j) {
            int c = dest->pixels4[k + j];
            float a1 = ((c >> 24) & 0xff) / 255.0;
            float a = (1.0 - (1.0 - a0) * (1.0 - a1));
            int r = (a0 * GFX_RED(color) + (1.0 - a0) * a1 * GFX_RED(c)) / a;
            int g = (a0 * GFX_GREEN(color) + (1.0 - a0) * a1 * GFX_GREEN(c)) / a;
            int b = (a0 * GFX_BLUE(color) + (1.0 - a0) * a1 * GFX_BLUE(c)) / a;
            dest->pixels4[k + j] = GFX_ARGB((int)(a * 255), r, g, b);
        }
    }
}

void gfx_fill(gfx_t* dest, uint32_t color, gfx_blendmode_t blend, gfx_clip_t* clip)
{
    int i;
    int minx = clip == NULL ? 0 : MAX(0, clip->left);
    int maxx = clip == NULL ? dest->width : MIN(dest->width, clip->right);
    int miny = clip == NULL ? 0 : MAX(0, clip->top);
    int maxy = clip == NULL ? dest->height : MIN(dest->height, clip->bottom);

    switch (blend) {
    case GFX_COPY_BLEND:
        for (i = miny; i < maxy; ++i) {
            int k = i * dest->pitch;
            memset32(&dest->pixels[k + minx * 4], color, (maxx - minx) * 4);
        }
        break;
    case GFX_ALPHA_BLENDING:
        gfx_fill_alpha_blending(dest, color, minx, maxx, miny, maxy);
        break;
    }
}


void gfx_blit(gfx_t* dest, gfx_t* src, gfx_blendmode_t blend, gfx_clip_t* clip)
{
    int i;
    int minx = clip == NULL ? 0 : MAX(0, clip->left);
    int maxx = clip == NULL ? dest->width : MIN(dest->width, clip->right);
    int miny = clip == NULL ? 0 : MAX(0, clip->top);
    int maxy = clip == NULL ? dest->height : MIN(dest->height, clip->bottom);

    switch (blend) {
    case GFX_COPY_BLEND:
        for (i = miny; i < maxy; ++i) {
            int k0 = i * dest->pitch;
            int k1 = i * src->pitch;
            memcpy32(&dest->pixels[k0 + minx * 4], &src->pixels[k1 + minx * 4], (maxx - minx) * 4);
        }
        break;
    case GFX_ALPHA_BLENDING:
        break;
    }
    //for (i = miny; i < maxy; ++i) {
    //    k0 = i * dest->pitch;
    //    k1 = (i % src->height)  * src->pitch;
    //    for (j = minx; j < maxx; ++j) {
    //        uint32_t c = src->pixels4[k1 + (j  % src->width)];
    //        dest->pixels4[k0 + j] = c;
    //    }
    //}
}

void gfx_blitmask(gfx_t* dest, gfx_t* src, gfx_blendmode_t blend, gfx_t* mask)
{
    int i, j, k0, k1;
    int minx = 0;
    int maxx = dest->width;
    int miny = 0;
    int maxy = dest->height;

    for (i = miny; i < maxy; ++i) {
        k0 = i * dest->pitch;
        k1 = (i % src->height) * src->pitch;
        for (j = minx; j < maxx; ++j) {
            int c = src->pixels4[k1 + (j % src->width)];
            dest->pixels4[k0 + j] = c;
        }
    }

    ((void)blend);
    ((void)mask);
}

void gfx_transform(gfx_t* dest, gfx_t* src, gfx_blendmode_t blend, gfx_clip_t* clip, float* matrix)
{
    ((void)dest);
    ((void)src);
    ((void)blend);
    ((void)clip);
    ((void)matrix);
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


int gfx_handle(gfx_t* gfx, gfx_msg_t *msg, gfx_seat_t *seat)
{
    int key, key2;
    struct timespec ts;

    switch (msg->message) {
    case GFX_EV_MOUSEMOVE:
        seat->mouse_x = msg->param1 & 0x7fff;
        seat->mouse_y = msg->param1 >> 16;
        break;
    case GFX_EV_BTNDOWN:
        seat->btn_status |= msg->param1;
        break;
    case GFX_EV_BTNUP:
        seat->btn_status &= ~msg->param1;
        break;
    case GFX_EV_KEYDOWN:
        key = keyboard_down(msg->param1, &seat->kdb_status, &key2);
        msg->param2 = key;
        if (key2 != 0)
            gfx_push_msg(gfx, GFX_EV_UNICODE, key2);
        gfx_push_msg(gfx, GFX_EV_UNICODE, key);
        break;
    case GFX_EV_KEYUP:
        key = keyboard_up(msg->param1, &seat->kdb_status);
        msg->param2 = key;
        break;
    case GFX_EV_TIMER:
        if (gfx->flags & GFX_FL_INVALID && gfx->flags & GFX_FL_PAINTTICK)
            gfx_push_msg(gfx, GFX_EV_PAINT, 0);
        break;
    case GFX_EV_PAINT:
        gfx->flags &= ~GFX_FL_INVALID;
        break;
    case GFX_EV_RESIZE:
        gfx_unmap(gfx);
        gfx->width = msg->param1 >> 16;
        gfx->height = msg->param1 & 0xffff;
        break;
    case GFX_EV_DELAY:
        ts.tv_sec = msg->param1 / 1000000;
        ts.tv_sec = msg->param1 % 1000000;
        thrd_sleep(&ts, NULL);
        break;
    default:
        break;
    }
    return msg->message;
}


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

void gfx_invalid(gfx_t* gfx)
{
    gfx->flags |= GFX_FL_INVALID;
}



int gfx_resize(gfx_t* gfx, int width, int height)
{
    gfx_unmap(gfx);
    if (gfx->fd != -1)
        return -1;
    gfx->pitch = width * 4;
    gfx->width = width;
    gfx->height = height;
    return 0;
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
