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
#ifndef _GFX_H
#define _GFX_H 1

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define _16x10(n)  ((n)/10*16)
#define _16x9(n)  ((n)/9*16)

#ifndef PACK
#  ifndef _WIN32
#    define PACK(decl) decl __attribute__((__packed__))
#  else
#    define PACK(decl) __pragma(pack(push,1)) decl __pragma(pack(pop))
#  endif
#endif

#define GFX_ALPHA(c)  (((c) >> 24) & 0xff)
#define GFX_RED(c)  (((c) >> 16) & 0xff)
#define GFX_GREEN(c)  (((c) >> 8) & 0xff)
#define GFX_BLUE(c)  ((c) & 0xff)
#define GFX_ARGB(a,r,g,b)  ((b & 0xff) | ((g & 0xff) << 8) | ((r & 0xff) << 16) | ((a & 0xff) << 24))
#define GFX_RGB(a,r,g,b)  ((b & 0xff) | ((g & 0xff) << 8) | ((r & 0xff) << 16) | 0xff000000)

typedef enum gfx_blend gfx_blend_t;
typedef enum gfx_event gfx_event_t;
typedef struct gfx gfx_t;
typedef struct gfx_seat gfx_seat_t;
typedef struct gfx_msg gfx_msg_t;
typedef struct gfx_clip gfx_clip_t;


enum gfx_blend {
    GFX_NOBLEND, // Just copy the color
    GFX_ALPHA_BLEND, // Full transparency support
    GFX_UPPER_BLEND, // Source transparency only
    GFX_CLRBLEND,
};

enum gfx_event {
    GFX_EV_QUIT = 0,
    GFX_EV_MOUSEMOVE,
    GFX_EV_BTNUP,
    GFX_EV_BTNDOWN,
    GFX_EV_KEYUP,
    GFX_EV_KEYDOWN,
    GFX_EV_KEYPRESS,
    GFX_EV_MOUSEWHEEL,
    GFX_EV_TIMER,
    GFX_EV_RESIZE,
    GFX_EV_PAINT,
    GFX_EV_DELAY = 127,
};

enum gfx_flags {
    GFX_FL_INVALID = (1 << 0),
    GFX_FL_PAINTTICK = (1 << 1),
    GFX_FL_EXPOSED = (1 << 2),
};

struct gfx {
    int width;
    int height;
    int pitch;
    long fd;
    long fi;
    union {
        uint8_t *pixels;
        uint32_t *pixels4;
    };
    uint8_t *backup;
    int flags;
};

struct gfx_seat {
    int mouse_x;
    int mouse_y;
    int rel_x;
    int rel_y;
    int btn_status; // left, right, wheel, prev, next
    int kdb_status; // Shift, caps, ctrl, alt, home, num, defl !
    int kdb_altkey;
    int *kdb_layout;
    int *kdb_alt_layout;
};

PACK(struct gfx_msg {
    uint16_t message;
    uint16_t window;
    uint32_t param1;
    uint32_t param2;
});

struct gfx_clip {
    int left, right, top, bottom;
};

/*
Gfx create a pointer to a surface.
This surface can be a fix image, or a video stream (in/out) or a window (event), or even a frame buffer (like window)

        /dev/fb0  is a framebuffer
        /dev/seat0  is a window or desktop !

*/



/* Surface operations
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
gfx_t *gfx_create_window(void *ctx, int width, int height);
gfx_t *gfx_create_surface(int width, int height);
void gfx_destroy(gfx_t *gfx);
int gfx_resize(gfx_t *gfx, int width, int height);
void *gfx_map(gfx_t *gfx);
void gfx_unmap(gfx_t *gfx);
int gfx_width(gfx_t *gfx);
int gfx_height(gfx_t *gfx);


gfx_t *gfx_open(const char *name, int flags);
void gfx_close(gfx_t *gfx);


/* Drawing operations
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

void gfx_blit(gfx_t *dst, gfx_t *src, gfx_blend_t mode, gfx_clip_t *clip, gfx_clip_t *clip_src);
void gfx_fill(gfx_t *dst, uint32_t color, gfx_blend_t mode, gfx_clip_t *clip);
// void gfx_blit_scale(gfx_t *dst, gfx_t* src, gfx_blend_t mode, gfx_clip_t* clip_dst, gfx_clip_t* clip_src);
// void gfx_blit_transform(gfx_t *dst, gfx_t* src, gfx_blend_t mode, gfx_clip_t* clip, float *matrix);

uint32_t gfx_alpha_blend(uint32_t low, uint32_t upr);
uint32_t gfx_upper_alpha_blend(uint32_t low, uint32_t upr);


/* Event operations
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
int gfx_poll(gfx_t *gfx, gfx_msg_t *msg);
int gfx_push(gfx_t *gfx, int type, int param);
/* Go to next frame */
int gfx_flip(gfx_t *gfx);
int gfx_resize(gfx_t *gfx, int width, int height);


/* Helpers
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
void gfx_clipboard_copy(const char *buf, int len);
int gfx_clipboard_paste(char *buf, int len);
void gfx_keyboard_load(gfx_seat_t *seat);
int gfx_keyboard_down(int key, gfx_seat_t *seat, int *key2);
int gfx_keyboard_up(int key, gfx_seat_t *seat);

int gfx_handle(gfx_t *gfx, gfx_msg_t *msg, gfx_seat_t *seat);
gfx_t *gfx_opend(int fd, int fi);
int gfx_push_msg(gfx_t *gfx, int type, int param);
void gfx_invalid(gfx_t *gfx);

gfx_t *gfx_load_image(const char *name);







// Context

void *gfx_open_framebuffer(const char *path);
int gfx_fb_open_input(void *ctx, const char *path);



#endif  /* _GFX_H */
