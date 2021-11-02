# Library Graphics `lgfx.so`

 The `lgfx` library aims to support the most basic way to create a surface and
 receive basic UI events. The goal is to offer the most basic yet reliable way
 to access graphics on multiple platform.

 The library can be build for several distributions. As it serves as the
 primary access method for Kora OS, it also work on Win32 and X11 API.

 The library runs fine by itself but can really benefits from a couple of
 optional third-parties:

  - `freetype2`
  - `libpng`

## Application Sample

 Let's see the most simple way of using lgfx library to create a graphic
 application.

```c
#include <gfx.h>

void app_paint(gfx_t *win)
{
    gfx_map(win);
    // ...
    gfx_flip(win);
}

void app_keypress(unsigned key, int mode, int *ptr_redraw)
{
    // ...
}

int main(int argc, char **argv)
{
    int need_redraw = 0;
    gfx_t* win = gfx_create_window(_16x10(480), 480);
    gfx_msg_t msg;
    gfx_timer(0, 20);
    while (gfx_poll(&msg) == 0 && msg.message != GFX_EV_QUIT) {
        gfx_handle(&msg);
        switch (msg.message) {
        case GFX_EV_TIMER: // Do painting
            if (need_redraw != 0) {
                app_paint(win);
                need_redraw = 1;
            }
            break;
        case GFX_EV_KEYPRESS: // Handle keyboard events
            app_keypress(msg.param1, msg.param2, &need_redraw);
            break;
        case GFX_EV_RESIZE: // Invalid painting
            need_redraw = 1;
            break;
        }
    }

    gfx_destroy(win);
    return 0;
}
```


## Surface

 - `gfx_create_surface`
 - `gfx_create_window`
 - `gfx_load_image`
 - `gfx_destroy`
 - `gfx_save_image`
 - `gfx_open_surface` (only on Kora!)

## Mapping operations

 - `gfx_map`
 - `gfx_unmap`
 - `gfx_width`
 - `gfx_height`
 - `gfx_resize`
 - `gfx_flip`

## Events

 - `gfx_poll`
 - `gfx_handle`
 - `gfx_push`
 - `gfx_timer`

## Drawing operations

 - `gfx_fill`
 - `gfx_blit`
 - `gfx_blit_scale`

## Font operations

 - `gfx_font`
 - `gfx_clear_font`
 - `gfx_mesure_text`
 - `gfx_write`


## Display and Desktop

 - `gfx_context`
 - `gfx_keyboard`

 > _To define_


## Note

 > The library is not unlike `SDL2`. To reach the same capability as the SDL, you will need to combine c11 thread support, `lgfx`, `lsnd`, OpenGl support, Joystick support and Haptic devices and power managment.

 - Handle change of keyboard
 - Configure to use GFX_EV_KEYPRESS from the system if available
 - Create a small tool to create a keyboard mapping !!
 - Add support for ico/jpeg/gif..
 - Add support to set window caption !

