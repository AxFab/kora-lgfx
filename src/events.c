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
#include "gfx.h"
#include "keycodes.h"

int gfx_keyboard_down(int key, gfx_seat_t *seat, int *key2);
int gfx_keyboard_up(int key, gfx_seat_t *seat);

LIBAPI void gfx_handle(gfx_msg_t *msg)
{
    int key, key2, x, y;
    gfx_t* gfx = msg->gfx;
    gfx_seat_t* seat = gfx ? gfx->seat : NULL;
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
        if (key2 != 0)
            gfx_push(gfx, GFX_EV_KEYPRESS, key2);
        if (key != 0 && key < KEY_UNICODE_MAX)
            gfx_push(gfx, GFX_EV_KEYPRESS, key);
        break;
    case GFX_EV_KEYUP:
        key = gfx_keyboard_up(msg->param1, seat);
        break;
    /*case GFX_EV_TIMER:
        if (gfx->flags & GFX_FL_INVALID && gfx->flags & GFX_FL_PAINTTICK)
            gfx_push(gfx, GFX_EV_PAINT, 0);
        break;*/
    /*case GFX_EV_PAINT:
        gfx->flags &= ~GFX_FL_INVALID;
        break;*/
    case GFX_EV_RESIZE:
        gfx_unmap(gfx);
        gfx->width = msg->param1 >> 16;
        gfx->height = msg->param1 & 0xffff;
        break;
    default:
        break;
    }
}


