#include "gfx.h"
#include "keycodes.h"

int gfx_handle(gfx_t* gfx, gfx_msg_t* msg, gfx_seat_t* seat)
{
    int key, key2, x, y;
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
    case GFX_EV_TIMER:
        if (gfx->flags & GFX_FL_INVALID && gfx->flags & GFX_FL_PAINTTICK)
            gfx_push(gfx, GFX_EV_PAINT, 0);
        break;
    case GFX_EV_PAINT:
        gfx->flags &= ~GFX_FL_INVALID;
        break;
    case GFX_EV_RESIZE:
        gfx_unmap(gfx);
        gfx->width = msg->param1 >> 16;
        gfx->height = msg->param1 & 0xffff;
        break;
    default:
        break;
    }
    return msg->message;
}


void gfx_invalid(gfx_t* gfx)
{
    gfx->flags |= GFX_FL_INVALID;
}


