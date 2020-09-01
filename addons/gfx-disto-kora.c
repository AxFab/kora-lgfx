#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include "../gfx.h"
#include "../mcrs.h"
#include <stdio.h>

int window(int service, int width, int height, int flags);

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

const char *pipe_name = "";

int gfx_open_window(gfx_t* gfx)
{
    gfx->fd = window(0, gfx->width, gfx->height, 0);
    gfx->fi = gfx->fd;
    if (gfx->fd == -1)
        return -1;
    return 0;
}

int gfx_open_device(gfx_t* gfx, const char *path)
{
    gfx->fd = open(path, O_RDWR | O_DIRECT);
    gfx->fi = gfx->fd;
    if (gfx->fd == -1)
        return -1;
    return 0;
}

int gfx_close_window(gfx_t* gfx)
{
    close(gfx->fd);
    if (gfx->fd != gfx->fi)
        close(gfx->fi);
    return 0;
}

void gfx_map_window(gfx_t *gfx)
{
    size_t lg = gfx->pitch * gfx->height * 2;
    gfx->pixels = mmap(NULL, lg, 0x10002, 0, gfx->fd, 0);
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
    fcntl(gfx->fd, 800/* FD_WFLIP */, 5, gfx->pixels, gfx->backup);
    uint8_t *tmp = gfx->backup;
    gfx->backup = gfx->pixels;
    gfx->pixels = tmp;
    return 0;
}

gfx_msg_t msg_pool[8];
int msg_ptr = 0;

int gfx_poll(gfx_t *gfx, gfx_msg_t *msg)
{
    char tmp[120];
    if (msg_ptr > 0) {
        msg_ptr--;
        memcpy(msg, &msg_pool[msg_ptr], sizeof(*msg));
        snprintf(tmp, 120, "Event recv <%d:%x.%x>", msg->message, msg->param1, msg->param2);
        return 0;
    }
    for (;;) {
        if (read(gfx->fi, (char *)msg, sizeof(*msg)) != 0) {
            if (msg->message != GFX_EV_TIMER) {
                snprintf(tmp, 120, "Event recv <%d:%x.%x>", msg->message, msg->param1, msg->param2);
                write(1, tmp, strlen(tmp));
            }
            return 0;
        }
    }
}


int gfx_push_msg(gfx_t* gfx, int type, int param)
{
    (void)gfx; // TODO -- WRITE ON PIPE!!
    gfx_msg_t msg;
    msg.message = type;
    msg.param1 = param;
    memcpy(&msg_pool[msg_ptr], &msg, sizeof(msg));
    msg_ptr++;
    // write(gfx->fi, (char*)&msg, sizeof(msg));
    return 0;
}
