#include <gfx.h>
#include <wns.h>
#include "disto.h"
#include "llist.h"
#include <sys/mman.h>
#include <unistd.h>

wns_cnx_t __wnscnx;
int __wnssct = 0;
int __wnsqry = 0;
int __wnsano = 0;
llhead_t __wnsgfx = INIT_LLHEAD;

struct lnwin {
    gfx_t* gfx;
    llnode_t node;
};

gfx_t* __find_win(int uid)
{
    struct lnwin* lnw;
    for ll_each(&__wnsgfx, lnw, struct lnwin, node) {
        if (lnw->gfx->uid == uid)
            return lnw->gfx;
    }
    return NULL;
}

void __store_win(gfx_t* gfx)
{
    struct lnwin* lnw = malloc(sizeof(struct lnwin));
    lnw->gfx = gfx;
    ll_append(&__wnsgfx, &lnw->node);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int gfx_close_wns(gfx_t* gfx)
{
    close(gfx->fd);
    return 0;
}

int gfx_map_wns(gfx_t* gfx)
{
    gfx->pixels = mmap(NULL, gfx->pitch * gfx->height * 2, PROT_WRITE, MMAP_SHARED, gfx->fd, 0);
    gfx->backup = &gfx->pixels[gfx->pitch * gfx->height];
    return 0;
}

int gfx_unmap_wns(gfx_t* gfx)
{
    munmap(gfx->pixels, gfx->pitch * gfx->height * 2);
    return 0;
}

int gfx_flip2_wns(gfx_t* gfx, gfx_clip_t* clip)
{
    void* top = gfx->pixels > gfx->backup ? gfx->pixels : gfx->backup;
    void* btm = gfx->pixels > gfx->backup ? gfx->backup : gfx->pixels;
    gfx->pixels = clip ? top : btm;
    gfx->backup = clip ? btm : top;
    return 0;
}

int gfx_flip_wns(gfx_t* gfx, gfx_clip_t *clip)
{
    wns_msg_t msg;
    gfx_clip_t rect;
    rect.left = clip ? clip->left : 0;
    rect.top = clip ? clip->top : 0;
    rect.right = clip ? clip->right : gfx->width;
    rect.bottom = clip ? clip->bottom : gfx->height;
    int off = gfx->pixels > gfx->backup ? 1 : 0;
    WNS_MSG(msg, WNS_FLIP, __wnssct, gfx->uid, GFX_POINT(rect.left, rect.top), GFX_POINT(rect.right, rect.bottom), off);
    wns_send(&__wnscnx, &msg);
    void* tmp = gfx->pixels;
    gfx->pixels = gfx->backup;
    gfx->backup = tmp;
    return 0;
}

int gfx_resize_wns(gfx_t *gfx)
{
    wns_msg_t msg;
    WNS_MSG(msg, WNS_RESIZE, __wnssct, gfx->uid, gfx->width, gfx->height, 0);
    wns_send(&__wnscnx, &msg);
    return 0;
}

int __gfx_wns_response(wns_msg_t *msg, int expect)
{
    for (;;) {
        wns_recv(&__wnscnx, msg);
        if (msg->request == WNS_NACK)
            return -1;
        if (msg->request == expect)
            return 0;
        // TODO -- Are we loosing events ?
    }
}

int gfx_open_wns(gfx_t* gfx)
{
    wns_msg_t msg;
    if (__wnssct == 0) {
        wns_connect(&__wnscnx);
        WNS_MSG(msg, WNS_HELLO, 0, 0, 0, 0, 0);
        wns_send(&__wnscnx, &msg);

        if (__gfx_wns_response(&msg, WNS_ACK) != 0)
            return -1;
        __wnssct = msg.secret;
    }

    WNS_MSG(msg, WNS_OPEN, __wnssct, 0, gfx->width, gfx->height, 0);
    wns_send(&__wnscnx, &msg);

    if (__gfx_wns_response(&msg, WNS_WINDOW) != 0)
        return -1;

    char tmp[16];
    snprintf(tmp, 16, "wns%08x", msg.winhdl);
    gfx->fd = shm_open(tmp, O_WRONLY, 0);
    if (gfx->fd == -1)
        return -1;
    gfx->width = msg.param;
    gfx->height = msg.param2;
    gfx->pitch = gfx->width * 4;
    gfx->uid = msg.winhdl;
    __store_win(gfx);
    gfx->map = gfx_map_wns;
    gfx->unmap = gfx_unmap_wns;
    gfx->flip = gfx_flip_wns;
    gfx->resize = gfx_resize_wns;
    gfx->close = gfx_close_wns;
    return 0;
}

LIBAPI gfx_t *gfx_create_wns(int width, int height, int uid)
{
    char tmp[16];
    gfx_t* gfx = malloc(sizeof(gfx_t));
    if (gfx == NULL)
        return NULL;
    memset(gfx, 0, sizeof(gfx_t));
    snprintf(tmp, 16, "wns%08x", uid);
    gfx->fd = shm_open(tmp, O_WRONLY | O_CREAT, 0);
    if (gfx->fd == -1)
        return NULL;
    gfx->seat = calloc(sizeof(gfx_seat_t), 1);
    gfx->width = width;
    gfx->height = height;
    gfx->pitch = gfx->width * 4;
    gfx->uid = uid;
    gfx->map = gfx_map_wns;
    gfx->unmap = gfx_unmap_wns;
    gfx->flip = gfx_flip2_wns;
    gfx->close = gfx_close_wns;
    return gfx;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int gfx_poll_wns(gfx_msg_t* msg)
{
    if (__wnssct == 0)
        return -1;

    for (;;) {
        wns_msg_t wmsg;
        wns_recv(&__wnscnx, &wmsg);
        memset(msg, 0, sizeof(gfx_msg_t));
        switch (wmsg.request) {
        case WNS_TICK:
            msg->message = GFX_EV_TIMER;
            msg->gfx = __find_win(wmsg.winhdl);
            msg->param1 = wmsg.param;
            return 0;
        case WNS_MOUSE:
            msg->message = GFX_EV_MOUSEMOVE;
            msg->gfx = __find_win(wmsg.winhdl);
            msg->param1 = wmsg.param;
            return 0;
        case WNS_EVENT:
            msg->message = wmsg.param;
            msg->gfx = __find_win(wmsg.winhdl);
            msg->param1 = wmsg.param2;
            msg->param2 = wmsg.param3;
            return 0;
        default:
            break;
        }
    }
}

int gfx_timer_wns(int delay, int interval)
{
    if (__wnssct == 0)
        return -1;

    wns_msg_t msg;
    WNS_MSG(msg, WNS_TIMER, __wnssct, delay, interval, 0, 0);
    wns_send(&__wnscnx, &msg);
    if (__gfx_wns_response(&msg, WNS_ACK) != 0)
        return -1;
    return msg.param;
}

gfx_ctx_t gfx_ctx_wns = {
    .open = gfx_open_wns,
    .poll = gfx_poll_wns,
    .timer = gfx_timer_wns,
};
