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
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include "gfx.h"
#include "../mcrs.h"
#include <stdio.h>

typedef struct gfx_eventmsg gfx_eventmsg_t;

PACK(struct gfx_eventmsg {
    uint16_t message;
    uint16_t window;
    uint32_t param1;
    uint32_t param2;
});

#define FB_FLIP 0x8002
#define FB_SIZE 0x8003

int window(int service, int width, int height, int flags);

void gfx_clipboard_copy(const char *buf, int len)
{
    ((void)buf);
    ((void)len);
}

int gfx_clipboard_paste(char *buf, int len)
{
    ((void)buf);
    ((void)len);
    return 0;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


typedef struct gfxhandle gfxhandle_t;
struct gfxhandle {
    gfx_t* gfx;
    uint32_t uid;
    gfxhandle_t* next;
};

gfxhandle_t* __handle = NULL;
gfx_t* __build_win = NULL;

uint32_t __auto_handle = 0;
int input_fd = 0;

const char *pipe_name = "";

gfx_t* __gfx_from_handle(uint32_t uid)
{
    if (__handle == NULL)
        return __build_win;
    gfxhandle_t* handle = __handle;
    while (handle->uid != uid) {
        handle = handle->next;
        if (handle == NULL)
            return __build_win;
    }
    return handle->gfx;
}

void gfx_initialize()
{
    if (input_fd == 0) {
        input_fd = open("/dev/kbd", O_RDONLY | O_DIRECT);
    }
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

int gfx_open_window(gfx_t *gfx)
{
    if (input_fd == 0)
        gfx_initialize();
    gfx->fd = window(0, gfx->width, gfx->height, 0);
    gfx->uid = ++__auto_handle;
    if (gfx->fd == -1)
        return -1;

    gfxhandle_t* handle = calloc(sizeof(gfxhandle_t), 1);
    handle->gfx = gfx;
    handle->uid = gfx->uid;
    handle->next = __handle;
    __handle = handle;
    return 0;
}

int gfx_open_device(gfx_t *gfx, const char *path)
{
    if (input_fd == 0)
        gfx_initialize();
    gfx->fd = open(path, O_WRONLY | O_DIRECT);
    if (gfx->fd == -1)
        return -1;
    // READ SIZE
    uint32_t size = fcntl(gfx->fd, FB_SIZE);
    gfx->width = size & 0x7FFF;
    gfx->height = (size >> 16) & 0x7FFF;
    return 0;
}

int gfx_close_window(gfx_t *gfx)
{
    close(gfx->fd);
    return 0;
}

void gfx_map_window(gfx_t *gfx)
{
    size_t lg = gfx->pitch * gfx->height * 2;
    gfx->pixels = mmap(NULL, lg, PROT_WRITE, MAP_PRIVATE | MAP_POPULATE, gfx->fd, 0);
    gfx->backup = ADDR_OFF(gfx->pixels, gfx->pitch * gfx->height);
}

void gfx_unmap_window(gfx_t *gfx)
{
    size_t lg = gfx->pitch * gfx->height * 2;
    munmap(MIN(gfx->pixels, gfx->backup), lg);
    gfx->pixels = NULL;
    gfx->backup = NULL;
}


int gfx_flip(gfx_t *gfx)
{
    if (gfx->pixels == NULL)
        return -1;
    fcntl(gfx->fd, FB_FLIP, gfx->pixels > gfx->backup ? gfx->pitch * gfx->height: 0);
    uint8_t *tmp = gfx->backup;
    gfx->backup = gfx->pixels;
    gfx->pixels = tmp;
    return 0;
}

gfx_msg_t msg_pool[8];
int msg_ptr = 0;

int gfx_poll_win(gfx_msg_t *msg)
{
    for (;;) {
        gfx_eventmsg_t emsg;
        if (read(input_fd, (char *)&emsg, sizeof(emsg)) != 0) {
            if (msg->message != GFX_EV_TIMER) {
                snprintf(tmp, 120, "Event recv <%d:%x.%x>", msg->message, msg->param1, msg->param2);
                write(1, tmp, strlen(tmp));
            }
            msg->message = emsg.message;
            msg->param1 = emsg.param1;
            msg->param2 = emsg.param2;
            msg->gfx = __gfx_from_handle(emsg.window);
            return 0;
        }
    }
}

unsigned gfx_timer(int delay, int interval)
{
    return 0;
}
