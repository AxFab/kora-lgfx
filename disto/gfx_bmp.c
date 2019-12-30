#include <kora/gfx.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <threads.h>

void clipboard_copy(const char *buf, int len)
{
}

int clipboard_paste(char *buf, int len)
{
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

static void gfx_read_events(int *fds)
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
}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

gfx_t *gfx_create_window(void *ctx, int width, int height, int flag)
{
    gfx_t *gfx = calloc(sizeof(gfx_t), 1);
    gfx->width = width;
    gfx->height = height;
    gfx->fd = 0;

    int p2[2];
    int *p3 = malloc(2 * sizeof(int));
    pipe(p2);
    p3[0] = p2[1];
    p3[1] = pipe_name == NULL ? 0 : open(pipe_name, O_RDONLY);
    gfx->fi = p2[0];

    thrd_t thrd;
    thrd_create(&thrd, (thrd_start_t)gfx_read_events, p3);
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
    if (gfx->pixels != NULL)
        return 0;
    gfx->pitch = ALIGN_UP(gfx->width * 4, 4);
    gfx->pixels = calloc(gfx->pitch, gfx->height);
    return 0;
}

int gfx_unmap(gfx_t *gfx)
{
    if (gfx->pixels == NULL)
        return 0;
    free(gfx->pixels);
    gfx->pixels = NULL;
    return 0;
}

PACK(struct bmp_header {
    uint16_t magic;
    uint32_t fsize;
    uint32_t reserved;
    uint32_t offset;
    uint32_t hsize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitsperpx;
    uint32_t compression;
    uint32_t isize;
    uint32_t xresol;
    uint32_t yresol;
    uint32_t a;
    uint32_t b;
});

int gfx_export_bmp24(const char *name, gfx_t *gfx)
{
    struct bmp_header head;
    int fd = open(name, O_WRONLY | O_CREAT, 0644);
    if (fd == -1)
        return -1;
    memset(&head, 0, sizeof(head));
    head.magic = 0x4d42;
    head.fsize = sizeof(head) + gfx->width * 3 * gfx->height;
    head.offset = sizeof(head);
    head.hsize = sizeof(head) - 0xE;
    head.width = gfx->width;
    head.height = gfx->height;
    head.planes = 1;
    head.bitsperpx = 24;
    head.isize = gfx->width * 3 * gfx->height;
    if (write(fd, &head, sizeof(head)) != sizeof(head)) {
        fprintf(stderr, "Error persisting image: %d\n", errno);
        return -1;
    }

    int x, y = gfx->height;
    while (y-- > 0) {
        for (x = 0; x < gfx->width; ++x)
            write(fd, &gfx->pixels4[y * gfx->width + x], 3);
    }
    close(fd);
    return 0;
}


void gfx_flip(gfx_t *gfx)
{
    if (gfx->pixels == NULL)
        return;
    gfx_export_bmp24("win.bmp", gfx);
}

int gfx_poll(gfx_t *gfx, gfx_msg_t *msg)
{
    for (;;) {
        int by = read(gfx->fi, msg, sizeof(*msg));
        if (by != 0)
            return by == sizeof(*msg) ? 0 : -1;
    }
}

