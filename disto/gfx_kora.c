#include <kora/gfx.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

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

gfx_t *gfx_create_window(void *ctx, int width, int height, int flag)
{
    gfx_t *gfx = calloc(sizeof(gfx_t), 1);
    gfx->width = width;
    gfx->height = height;
    gfx->fd = window(0, width, height, 0);
    if (gfx->fd == -1) {
        free(gfx);
        return NULL;
    }

    ((void)ctx);
    ((void)flag);
    return gfx;
}

void gfx_destroy(gfx_t *gfx)
{
    gfx_unmap(gfx);
    close(gfx->fd);
    free(gfx);
}


int gfx_map(gfx_t *gfx)
{
    (void)gfx->fi;
    if (gfx->pixels != NULL)
        return 0;
    gfx->pitch = ALIGN_UP(gfx->width * 4, 4);
    gfx->pixels = mmap(NULL, 8 * _Mib_, 0x10002, 0, gfx->fd, 0);
    gfx->backup = ADDR_OFF(gfx->pixels, gfx->pitch * gfx->height);
    return 0;
}

int gfx_unmap(gfx_t *gfx)
{
    if (gfx->pixels == NULL)
        return 0;
    munmap(MIN(gfx->pixels, gfx->backup), 8 * _Mib_);
    gfx->pixels = NULL;
    gfx->backup = NULL;
    return 0;
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

int gfx_poll(gfx_t *gfx, gfx_msg_t *msg)
{
    char tmp[120];
    for (;;) {
        if (read(gfx->fi, (char *)msg, sizeof(*msg)) != 0) {
            if (msg->message != GFX_EV_TIMER) {
                // snprintf(tmp, 120, "Event recv <%d:%x.%x>", msg->message, msg->param1, msg->param2);
                write(1, tmp, strlen(tmp));
            }
            return 0;
        }
    }
}

