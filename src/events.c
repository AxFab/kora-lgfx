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
#include "gfx.h"
#include "disto.h"
#include "keycodes.h"
#include <string.h>

int gfx_keyboard_down(int key, gfx_seat_t *seat, int *key2);
int gfx_keyboard_up(int key, gfx_seat_t *seat);



LIBAPI void gfx_handle(gfx_msg_t *msg)
{
    int key, key2, stt, x, y;
    gfx_t *gfx = msg->gfx;
    gfx_seat_t *seat = gfx ? gfx->seat : NULL;
    if (seat)
        seat->rel_x = seat->rel_y = 0;

    switch (msg->message) {
    // TODO - Drag&Drop, Exposed/Hidden, MouseOver/out,
    case GFX_EV_MOUSEMOVE:
        x = seat->mouse_x;
        y = seat->mouse_y;
        seat->mouse_x = msg->param1 & 0x7fff;
        seat->mouse_y = msg->param1 >> 16;
        seat->rel_x = seat->mouse_x - x;
        seat->rel_y = seat->mouse_y - y;
        break;
    case GFX_EV_BTNDOWN:
        seat->btn_status |= msg->param1;
        break;
    case GFX_EV_BTNUP:
        seat->btn_status &= ~msg->param1;
        break;
    case GFX_EV_KEYDOWN:
        key = gfx_keyboard_down(msg->param1, seat, &key2);
        stt = 0;
        if (seat->kdb_status & (KMOD_LCTRL | KMOD_RCTRL))
            stt |= 1;
        else if (seat->kdb_status & (KMOD_LALT))
            stt |= 2;
        else if (seat->kdb_status & (KMOD_LGUI | KMOD_RGUI))
            stt |= 4;
        if (key2 != 0 && stt == 0)
            gfx_push(gfx, GFX_EV_KEYPRESS, key2, stt);
        if (key != 0 && key < KEY_UNICODE_MAX)
            gfx_push(gfx, GFX_EV_KEYPRESS, key, stt);
        break;
    case GFX_EV_KEYUP:
        key = gfx_keyboard_up(msg->param1, seat);
        break;
    case GFX_EV_RESIZE:
        gfx_resize(gfx, msg->param1 & 0xffff, msg->param1 >> 16);
        break;
    default:
        break;
    }
}


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#define MSG_POOL_SIZE 16
gfx_msg_t msg_pool[MSG_POOL_SIZE];
int msg_ptr = 0;

int gfx_poll(gfx_msg_t *msg)
{
    for (;;) {
        if (msg_ptr > 0) {
            msg_ptr--;
            memcpy(msg, &msg_pool[msg_ptr], sizeof(*msg));
            return 0;
        }

        gfx_ctx_t *ctx = gfx_context(NULL);
        int ret = ctx->poll(msg);
        if (ret == 0)
            return ret;
    }
}


int gfx_push(gfx_t *gfx, int type, int param1, int param2)
{
    if (msg_ptr >= MSG_POOL_SIZE)
        return -1;
    gfx_msg_t *msg = &msg_pool[msg_ptr++];
    msg->message = type;
    msg->param1 = param1;
    msg->param2 = param2;
    msg->gfx = gfx;
    return 0;
}



int gfx_timer(int delay, int interval)
{
    gfx_ctx_t *ctx = gfx_context(NULL);
    if (ctx->timer != NULL)
        return ctx->timer(delay, interval);
    return -1;
}
