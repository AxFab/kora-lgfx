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

#ifndef LIBAPI
# if defined(WIN32) || defined(_WIN32)
#  define LIBAPI __declspec(dllexport)
# else
#  define LIBAPI
# endif
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
typedef struct gfx_font gfx_font_t;
typedef struct gfx_text_metrics gfx_text_metrics_t;

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

enum gfx_font_mode {
    GFX_FT_BITFONT = 1,
    GFX_FT_FREETYPE = 2,
};

enum gfx_font_style {
    GFX_FONT_ITALIC = 0x10,
    GFX_FONT_WTHIN = 1,
    GFX_FONT_WEXTRALIGHT = 2,
    GFX_FONT_WLIGHT = 3,
    GFX_FONT_WREGULAR = 4,
    GFX_FONT_WMEDIUM = 5,
    GFX_FONT_WSEMIBOLD = 6,
    GFX_FONT_WBOLD = 7,
    GFX_FONT_WEXTRABOLD = 8,
    GFX_FONT_WBLACK = 9,
    GFX_FONT_WEIGHT_MASK = 0xF,
};

struct gfx {
    int width;
    int height;
    int pitch;
    uint32_t uid;
    // int flags;
    union {
        uint8_t* pixels;
        uint32_t* pixels4;
    };
    union {
        uint8_t* backup;
        uint32_t* backup4;
    };
    long fd;
    gfx_seat_t *seat;
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

struct gfx_msg {
    uint16_t message;
    uint16_t window;
    uint32_t param1;
    uint32_t param2;
    gfx_t* gfx;
};

struct gfx_clip {
    int left, right, top, bottom;
};


struct gfx_font {
    int mode;
    float size;
    int style;
    char *family;
    void *face;
};


struct gfx_text_metrics {
    int width;
    int height;
    int baseline;
};

/*
Gfx create a pointer to a surface.
This surface can be a fix image, or a video stream (in/out) or a window (event), or even a frame buffer (like window)

        /dev/fb0  is a framebuffer
        /dev/seat0  is a window or desktop !
*/
typedef struct gfx_placement gfx_placement_t;
struct gfx_placement {
    int screen;
    int type;
    int left;
    int top;
};


/* Surface creation
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
LIBAPI gfx_t *gfx_create_window(void *ctx, int width, int height);
LIBAPI gfx_t* gfx_create_surface(int width, int height);
LIBAPI gfx_t* gfx_open_surface(const char* path);
LIBAPI void gfx_destroy(gfx_t *gfx);
LIBAPI gfx_t *gfx_load_image(const char * path);
LIBAPI int gfx_save_image(gfx_t *gfx, const char* path);

/* Windows operations
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
int gfx_set_caption(gfx_t *win, const char *title, unsigned icon);
int gfx_title(gfx_t *win, char *buf, int len);
unsigned gfx_icon(gfx_t *win);
int gfx_set_position(gfx_t *win, const gfx_placement_t *place);
int gfx_position(gfx_t *win, gfx_placement_t *place);

/* Mapping operations
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
LIBAPI void *gfx_map(gfx_t *gfx);
LIBAPI void gfx_unmap(gfx_t *gfx);
int gfx_width(gfx_t* gfx);
int gfx_height(gfx_t* gfx);
LIBAPI int gfx_resize(gfx_t* gfx, int width, int height);
LIBAPI int gfx_flip(gfx_t *gfx);


/* Event operations
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
LIBAPI int gfx_poll(gfx_msg_t *msg);
LIBAPI int gfx_push(gfx_t *gfx, int type, int param);
LIBAPI void gfx_handle(gfx_msg_t* msg);
LIBAPI unsigned gfx_timer(int delay, int interval);


/* Drawing operations
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
LIBAPI void gfx_blit(gfx_t *dst, gfx_t *src, gfx_blend_t mode, const gfx_clip_t *clip, const gfx_clip_t *clip_src);
LIBAPI void gfx_fill(gfx_t *dst, uint32_t color, gfx_blend_t mode, const gfx_clip_t *clip);
LIBAPI void gfx_blit_scale(gfx_t *dst, gfx_t *src, gfx_blend_t mode, const gfx_clip_t *clip_dst, const gfx_clip_t *clip_src);

LIBAPI uint32_t gfx_alpha_blend(uint32_t low, uint32_t upr);
LIBAPI uint32_t gfx_upper_alpha_blend(uint32_t low, uint32_t upr);
LIBAPI uint32_t gfx_selected_blend(uint32_t low, uint32_t upr);
LIBAPI uint32_t gfx_select_color(uint32_t color);


/* Font operations
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
LIBAPI int gfx_glyph(gfx_t *gfx, gfx_font_t *font, uint32_t unicode, uint32_t fg, uint32_t bg, int x, int y, const gfx_clip_t *clip);
LIBAPI int gfx_write(gfx_t *gfx, gfx_font_t *font, const char *text, uint32_t fg, int x, int y, const gfx_clip_t *clip);
LIBAPI gfx_font_t * gfx_font(const char *family, float size, int style);
LIBAPI void gfx_clear_font(gfx_font_t *font);
LIBAPI int gfx_mesure_text(gfx_font_t *font, const char *text, gfx_text_metrics_t *metrics);


/* Helpers
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
LIBAPI void gfx_clipboard_copy(const char *buf, int len);
LIBAPI int gfx_clipboard_paste(char *buf, int len);

// void gfx_keyboard_load(gfx_seat_t *seat);
// int gfx_keyboard_down(int key, gfx_seat_t *seat, int *key2);
// int gfx_keyboard_up(int key, gfx_seat_t *seat);

// void gfx_invalid(gfx_t *gfx);

// Context



#endif  /* _GFX_H */
