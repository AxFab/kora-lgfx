#include "gfx.h"
#include <assert.h>
#include <string.h>
#include "mcrs.h"



typedef uint32_t (*gfx_blend_proc_t)(uint32_t low, uint32_t upr);


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

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


// Mix two colors with transparency
static inline uint32_t gfx_alpha_blend_(uint32_t low, uint32_t upr)
{
    int va = upr >> 24;
    if (va == 0)
        return low;
    if (va == 255)
        return va;
    int vb = low >> 24;
    if (vb == 0)
        return upr;

    float al = (low >> 24) / 255.0f;
    float au = (upr >> 24) / 255.0f;
    float ar = (1.0f - au) * al;
    float a = (1.0f - (1.0f - au) * (1.0f - al));
    int r = (int)((au * GFX_RED(upr) + ar * GFX_RED(low)) / a);
    int g = (int)((au * GFX_GREEN(upr) + ar * GFX_GREEN(low)) / a);
    int b = (int)((au * GFX_BLUE(upr) + ar * GFX_BLUE(low)) / a);
    return GFX_ARGB((int)(a * 255), r, g, b);
}


// Mix two colors with transparency for the upper color only
static inline uint32_t gfx_upper_alpha_blend_(uint32_t low, uint32_t upr)
{
    int va = upr >> 24;
    if (va == 0)
        return low | 0xff000000;
    if (va == 255)
        return upr | 0xff000000;
    float au = va / 255.0f;
    float ar = 1.0f - au;
    int r = (int)(au * GFX_RED(upr) + ar * GFX_RED(low));
    int g = (int)(au * GFX_GREEN(upr) + ar * GFX_GREEN(low));
    int b = (int)(au * GFX_BLUE(upr) + ar * GFX_BLUE(low));
    return GFX_ARGB(255, r, g, b);
}

static inline uint32_t gfx_selected_blend_(uint32_t low, uint32_t upr)
{
    return (upr & 0xffffff) == 0xff0000 ? low : upr;
}

uint32_t gfx_alpha_blend(uint32_t low, uint32_t upr)
{
    return gfx_alpha_blend_(low, upr);
}

uint32_t gfx_upper_alpha_blend(uint32_t low, uint32_t upr)
{
    return gfx_upper_alpha_blend_(low, upr);
}

uint32_t gfx_selected_blend(uint32_t low, uint32_t upr)
{
    return gfx_selected_blend_(low, upr);
}


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

static inline void gfx_blit_generic(uint8_t *pixels, uint8_t *src_pixels,
                                    int dpitch, int spitch, int minx, int maxx, int miny, int maxy, int dx, int dy,
                                    gfx_blend_proc_t blend)
{
    int i, j;
    for (i = miny; i < maxy; ++i) {
        int k0 = i * dpitch;
        int k1 = (i + dy) * spitch;
        for (j = minx; j < maxx; ++j) {
            uint32_t *s = (uint32_t *)&src_pixels[k1 + (j + dx) * 4];
            uint32_t *d = (uint32_t *)&pixels[k0 + j * 4];
            *d = blend(*d, *s);
        }
    }
}

static inline void gfx_fill_generic(uint8_t *pixels, uint32_t color,
                                    int dpitch, int minx, int maxx, int miny, int maxy, gfx_blend_proc_t blend)
{
    int i, j;
    for (i = miny; i < maxy; ++i) {
        int k0 = i * dpitch;
        for (j = minx; j < maxx; ++j) {
            uint32_t *d = (uint32_t *)&pixels[k0 + (minx + j) * 4];
            *d = blend(*d, color);
        }
    }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

void gfx_blit(gfx_t *dst, gfx_t *src, gfx_blend_t mode, gfx_clip_t *clip, gfx_clip_t *clip_src)
{
    int i;
    int minx = clip == NULL ? 0 : MAX(0, clip->left);
    int maxx = clip == NULL ? dst->width : MIN(dst->width, clip->right);
    int miny = clip == NULL ? 0 : MAX(0, clip->top);
    int maxy = clip == NULL ? dst->height : MIN(dst->height, clip->bottom);

    int dx = clip == NULL ? 0 : -clip->left;
    int dy = clip == NULL ? 0 : -clip->top;
    maxx = MIN(maxx, clip == NULL ? minx + src->width : minx + clip->left + dx + src->width);
    maxy = MIN(maxy, clip == NULL ? miny + src->height : miny + clip->top + dy + src->height);

    if (minx >= maxx || miny >= maxy)
        return;

    if (mode == GFX_NOBLEND) {
        for (i = miny; i < maxy; ++i) {
            int k0 = i * dst->pitch;
            int k1 = (i + dy) * src->pitch;
            memcpy32(&dst->pixels[k0 + minx * 4], &src->pixels[k1 + (minx + dx) * 4], (maxx - minx) * 4);
        }
    } else if (mode == GFX_ALPHA_BLEND) {
        gfx_blit_generic(dst->pixels, src->pixels, dst->pitch, src->pitch,
                         minx, maxx, miny, maxy, dx, dy, gfx_alpha_blend);
    } else if (mode == GFX_UPPER_BLEND) {
        gfx_blit_generic(dst->pixels, src->pixels, dst->pitch, src->pitch,
                         minx, maxx, miny, maxy, dx, dy, gfx_upper_alpha_blend);
    } else if (mode == GFX_CLRBLEND) {
        gfx_blit_generic(dst->pixels, src->pixels, dst->pitch, src->pitch,
                         minx, maxx, miny, maxy, dx, dy, gfx_selected_blend);
    }
}

void gfx_fill(gfx_t *dst, uint32_t color, gfx_blend_t mode, gfx_clip_t *clip)
{

    int i;
    int minx = clip == NULL ? 0 : MAX(0, clip->left);
    int maxx = clip == NULL ? dst->width : MIN(dst->width, clip->right);
    int miny = clip == NULL ? 0 : MAX(0, clip->top);
    int maxy = clip == NULL ? dst->height : MIN(dst->height, clip->bottom);
    if (minx >= maxx || miny >= maxy)
        return;

    if (mode == GFX_NOBLEND) {
        for (i = miny; i < maxy; ++i) {
            int k0 = i * dst->pitch;
            memset32(&dst->pixels[k0 + minx * 4], color, (maxx - minx) * 4);
        }
    } else if (mode == GFX_ALPHA_BLEND) {
        gfx_fill_generic(dst->pixels, color, dst->pitch,
                         minx, maxx, miny, maxy, gfx_alpha_blend);
    } else if (mode == GFX_UPPER_BLEND) {
        gfx_fill_generic(dst->pixels, color, dst->pitch,
                         minx, maxx, miny, maxy, gfx_upper_alpha_blend);
    }
}


#include <math.h>

void gfx_blit_scale(gfx_t *dst, gfx_t *src, gfx_blend_t blend, gfx_clip_t *clip_dst, gfx_clip_t *clip_src)
{
    gfx_clip_t dc;
    if (clip_dst)
        dc = *clip_dst;
    else {
        dc.left = 0;
        dc.right = dst->width;
        dc.top = 0;
        dc.bottom = dst->height;
    }
    gfx_clip_t sc;
    if (clip_src)
        sc = *clip_src;
    else {
        sc.left = 0;
        sc.right = src->width;
        sc.top = 0;
        sc.bottom = src->height;
    }

    int minx = dc.left;
    int maxx = dc.right;
    int miny = dc.top;
    int maxy = dc.bottom;
    int i, j, s, t;

    float rx = (float)(sc.right - sc.left) / (dc.right - dc.left);
    float ry = (float)(sc.bottom - sc.top) / (dc.bottom - dc.top);
    float dx = 0;
    float dy = 0;

    for (i = miny; i < maxy; ++i) {
        int k0 = i * dst->width;
        float ny = ry * i + dy;
        float my = ry * (i + 1) + dy;
        for (j = minx; j < maxx; ++j) {
            uint32_t *D = &dst->pixels4[k0 + (minx + j)];
            float R = 0;
            float G = 0;
            float B = 0;

            float sm = 0;
            float nx = rx * j + dx;
            float mx = rx * (j + 1) + dx;

            for (t = floor(ny); t < ceil(my); ++t) {
                for (s = floor(nx); s < ceil(mx); ++s) {
                    float r = 1;

                    if (t == floor(ny))
                        r *= 1.0f - (ny - floor(ny));
                    else if (t + 1.f == ceil(my))
                        r *= my - (float)t;

                    if (s == floor(nx))
                        r *= 1.0f - (nx - floor(nx));
                    else if (s + 1.f == ceil(mx))
                        r *= mx - (float)s;

                    uint32_t V = src->pixels4[t * src->width + s];
                    R += GFX_RED(V) * r;
                    G += GFX_GREEN(V) * r;
                    B += GFX_BLUE(V) * r;
                    sm += r;
                }
            }

            uint32_t S = GFX_RGB(0xff, (int)(R / sm), (int)(G / sm), (int)(B / sm));
            *D = S;
        }
    }
}

// void gfx_blit_transform(gfx_t *dst, gfx_t* src, gfx_blend_t blend, gfx_clip_t* clip, float *matrix);
