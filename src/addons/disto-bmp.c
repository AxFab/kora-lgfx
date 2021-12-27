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
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <threads.h>
#include <gfx.h>
#include "../mcrs.h"
#include "../disto.h"

int gfx_clipboard_copy(const char *buf, int len)
{
    ((void)buf);
    ((void)len);
    return 0;
}

int gfx_clipboard_paste(char *buf, int len)
{
    ((void)buf);
    ((void)len);
    return 0;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

uint8_t rev[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0x1c, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0x39, 0, 0, 0, 0, 0, 0, 0, // 0x20
    0, 0, 0, 0, 0x33, 0x0c, 0x34, 0x35,
    0x0b, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, // 0x30
    0x09, 0x0a, 0, 0x27, 0, 0x0d, 0, 0,
    0, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, // 0x40
    0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18,
    0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, // 0x50
    0x2d, 0x15, 0x2c, 0x1a, 0x56, 0x1b, 0, 0,
    0x29, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, // 0x60
    0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18,
    0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, // 0x70
    0x2d, 0x15, 0x2c, 0, 0, 0, 0, 0,

};

gfx_msg_t mq_events[8];
int mq_pointer = 0;
int mq_reader = 0;


static void gfx_post(int message, int param1, int param2)
{
    gfx_msg_t *msg = &mq_events[mq_pointer++];
    msg->message = message;
    msg->window = 0;
    msg->param1 = param1;
    msg->param2 = param2;
}

static void gfx_read(gfx_msg_t *msg)
{
    memcpy(msg, &mq_events[mq_reader++], sizeof(gfx_msg_t));
}

static void gfx_parse_key(char *buf)
{
    int lm = buf[1];
    int md = buf[0];
    int ch = buf[0];

    if (lm == '+') {
        if (md == 'C')
            gfx_post(GFX_EV_KEYDOWN, 0x1d, 0);
        else if (md == 'S')
            gfx_post(GFX_EV_KEYDOWN, 0x2a, 0);
        ch = buf[2];
    } else if (lm == '!') {
        if (md == 'e')
            ch = 0x10;
        else
            ch = 0;
    }

    int key = rev[ch];
    if (key != 0) {
        gfx_post(GFX_EV_KEYDOWN, key, 0);
        gfx_post(GFX_EV_KEYUP, key, 0);
    }

    if (lm == '+') {
        if (md == 'C')
            gfx_post(GFX_EV_KEYUP, 0x1d, 0);
        else if (md == 'S')
            gfx_post(GFX_EV_KEYUP, 0x2a, 0);
    }
}

static void gfx_parse_event(FILE *fp)
{
    char buf[32];
    int cn = fgets(buf, 32, fp);

    if (strncmp(buf, "KEY ", 4) == 0)
        gfx_parse_key(buf);
    else if (strncmp(buf, "QUIT", 4) == 0 || strcmp(buf, "q") == 0)
        gfx_post(GFX_EV_QUIT, 0, 0);
    else if (strncmp(buf, "TIMER", 5) == 0 || strcmp(buf, "t") == 0)
        gfx_post(GFX_EV_TIMER, 0, 0);
    else if (strncmp(buf, "DELAY", 5) == 0)
        gfx_post(GFX_EV_DELAY, 500000, 0);
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

int gfx_close_bmp(gfx_t *gfx)
{
    gfx_unmap(gfx);
    close(gfx->fd);
    return 0;
}

int gfx_map_bmp(gfx_t *gfx)
{
    gfx->pitch = ALIGN_UP(gfx->width * 4, 4);
    gfx->pixels = calloc(gfx->pitch, gfx->height);
    return 0;
}

int gfx_unmap_bmp(gfx_t *gfx)
{
    free(gfx->pixels);
    gfx->pixels = NULL;
    return 0;
}

int gfx_flip_bmp(gfx_t *gfx)
{
    if (gfx->pixels == NULL)
        return -1;
    gfx_save_image(gfx, "win.bmp");
    return 0;
}

int gfx_open_bmp(gfx_t *gfx)
{
    gfx->fd = 1;
    gfx->map = gfx_map_bmp;
    gfx->unmap = gfx_unmap_bmp;
    gfx->flip = gfx_flip_bmp;
    gfx->close = gfx_close_bmp;
    return 0;
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

int gfx_open_device(gfx_t *gfx, const char *path)
{
    return -1;
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

int gfx_poll_bmp(gfx_msg_t *msg)
{
    if (mq_reader >= mq_pointer && mq_reader != 0) {
        mq_pointer = 0;
        mq_reader = 0;
    }
    if (mq_pointer == 0)
        gfx_parse_event(stdin);

    gfx_read(msg);
    return 0;
}

int gfx_timer_bmp(int delay, int interval)
{
    return 1;
}


gfx_ctx_t gfx_ctx_bmp = {
    .open = gfx_open_bmp,
    .poll = gfx_poll_bmp,
    .timer = gfx_timer_bmp,
};
