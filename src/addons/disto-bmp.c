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

void clipboard_copy(const char *buf, int len)
{
    ((void)buf);
    ((void)len);
}

int clipboard_paste(char *buf, int len)
{
    ((void)buf);
    ((void)len);
    return 0;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

const char *pipe_name = NULL;

static void gfx_post(int fd, int no, uint32_t param1, uint32_t param2)
{
    gfx_msg_t msg;
    msg.window = 0;
    msg.message = no;
    msg.param1 = param1;
    msg.param2 = param2;
    write(fd, &msg, sizeof(msg));
    printf(" - [%d, %x, %x]\n", no, param1, param2);
}

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

static void gfx_parse_key(int fd, char *buf)
{
    int lm = buf[1];
    int md = buf[0];
    int ch = buf[0];

    if (lm == '+') {
        if (md == 'C')
            gfx_post(fd, GFX_EV_KEYDOWN, 0x1d, 0);
        else if (md == 'S')
            gfx_post(fd, GFX_EV_KEYDOWN, 0x2a, 0);
        ch = buf[2];
    } else if (lm == '!') {
        if (md == 'e')
            ch = 0x10;
        else
            ch = 0;
    }

    int key = rev[ch];
    if (key != 0) {
        gfx_post(fd, GFX_EV_KEYDOWN, key, 0);
        gfx_post(fd, GFX_EV_KEYUP, key, 0);
    }

    if (lm == '+') {
        if (md == 'C')
            gfx_post(fd, GFX_EV_KEYUP, 0x1d, 0);
        else if (md == 'S')
            gfx_post(fd, GFX_EV_KEYUP, 0x2a, 0);
    }
}

static int gfx_read_events(int *fds)
{
    int cn, idx = 0;
    char buf[16] = { 0 };
    for (;;) {
        int cap = 16;
        if (idx < cap)
            cn = read(fds[1], &buf[idx], cap - idx);
        cap = idx + cn;
        if (cap == 0)
            break;
        char *n = strchr(buf, '\n');
        if (n == NULL)
            exit(-5);
        *n = '\0';

        if (strncmp(buf, "KEY ", 4) == 0)
            gfx_parse_key(fds[0], &buf[4]);
        else if (strncmp(buf, "QUIT", 4) == 0 || strcmp(buf, "q") == 0)
            gfx_post(fds[0], GFX_EV_QUIT, 0, 0);
        else if (strncmp(buf, "TIMER", 5) == 0 || strcmp(buf, "t") == 0)
            gfx_post(fds[0], GFX_EV_TIMER, 0, 0);
        else if (strncmp(buf, "DELAY", 5) == 0)
            gfx_post(fds[0], GFX_EV_DELAY, 500000, 0);
        idx = (++n) - buf;
        memmove(buf, n, cap - idx);
        idx = cap - idx;
    }
    close(fds[1]);
    close(fds[0]);
    free(fds);
    thrd_detach(thrd_current());
    thrd_exit(0);
    return 0;
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


int gfx_open_window(gfx_t *gfx)
{
    int p2[2];
    int *p3 = malloc(2 * sizeof(int));
    pipe(p2);
    p3[0] = p2[1];
    p3[1] = pipe_name == NULL ? 0 : open(pipe_name, O_RDONLY);
    gfx->fd = p2[0];

    thrd_t thrd;
    thrd_create(&thrd, (thrd_start_t)gfx_read_events, p3);
    return 0;
}

int gfx_open_device(gfx_t *gfx, const char *path)
{
    int p2[2];
    int *p3 = malloc(2 * sizeof(int));
    pipe(p2);
    p3[0] = p2[1];
    p3[1] = open(path, O_RDONLY);
    gfx->fd = p2[0];

    thrd_t thrd;
    thrd_create(&thrd, (thrd_start_t)gfx_read_events, p3);
    return 0;
}

int gfx_close_window(gfx_t *gfx)
{
    gfx_unmap(gfx);
    close(gfx->fd);
    return 0;
}

void gfx_map_window(gfx_t *gfx)
{
    if (gfx->pixels != NULL)
        return;
    gfx->pitch = ALIGN_UP(gfx->width * 4, 4);
    gfx->pixels = calloc(gfx->pitch, gfx->height);
}

void gfx_unmap_window(gfx_t *gfx)
{
    if (gfx->pixels == NULL)
        return;
    free(gfx->pixels);
    gfx->pixels = NULL;
}

int gfx_flip(gfx_t *gfx)
{
    if (gfx->pixels == NULL)
        return -1;
    gfx_save_image(gfx, "win.bmp");
    return 0;
}

int gfx_poll(/*gfx_t *gfx, */gfx_msg_t *msg)
{
    gfx_t *gfx = NULL;
    for (;;) {
        int by = read(gfx->fd, msg, sizeof(*msg));
        if (by != 0)
            return by == sizeof(*msg) ? 0 : -1;
    }
}

int gfx_push(gfx_t *gfx, int type, int param)
{
    return -1;
}
