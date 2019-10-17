#include <assert.h>
#include <string.h>
#include <kora/gfx.h>
#include <threads.h>

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



int gfx_loop(gfx_t *gfx, void *arg, gfx_handlers_t *handlers)
{
    gfx_seat_t seat;
    gfx_msg_t msg;
    memset(&seat, 0, sizeof(seat));
    for (;;) {
        gfx_poll(gfx, &msg);
        switch (msg.message) {
        case EV_QUIT:
            return 0;
        case EV_MOUSEMOVE:
            seat.mouse_x = msg.param1 & 0x7fff;
            seat.mouse_y = msg.param1 >> 16;
            if (handlers->mse_move)
                handlers->mse_move(gfx, arg, &seat);
            break;
        case EV_BUTTONDOWN:
            seat.btn_status |= msg.param1;
            if (handlers->mse_down)
                handlers->mse_down(gfx, arg, &seat, msg.param1);
            break;
        case EV_BUTTONUP:
            seat.btn_status &= ~msg.param1;
            if (handlers->mse_up)
                handlers->mse_up(gfx, arg, &seat, msg.param1);
            break;
        case EV_MOUSEWHEEL:
            if (handlers->mse_wheel)
                handlers->mse_wheel(gfx, arg, &seat, msg.param1);
            break;
        case EV_KEYDOWN:
            if (handlers->key_down)
                handlers->key_down(gfx, arg, &seat, msg.param1);
            break;
        case EV_KEYUP:
            if (handlers->key_up)
                handlers->key_up(gfx, arg, &seat, msg.param1);
            break;
        case EV_TIMER:
            if (handlers->repaint == NULL || handlers->repaint(gfx, arg, &seat)) {
                if (handlers->expose) {
                    handlers->expose(gfx, arg, &seat);
                    gfx_flip(gfx);
                }
            }
            break;
        case EV_RESIZE:
            gfx_unmap(gfx);
            gfx->width = msg.param1 >> 16;
            gfx->height = msg.param1 & 0xffff;
            break;
        case EV_DELAY: {
            struct timespec ts;
            ts.tv_sec = msg.param1 / 1000000;
            ts.tv_sec = msg.param1 % 1000000;
            thrd_sleep(&ts, NULL);
        }
            break;
        default:
            break;
        }
    }
}

