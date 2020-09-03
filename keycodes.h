/*
 *      This file is part of the KoraOS project.
 *  Copyright (C) 2015  <Fabien Bavent>
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
#ifndef _KEYCODES_H
#define _KEYCODES_H  1

#define _FROM_SCANCODE(x)  (x|(1<<30))
#define KEY_UNICODE_MAX  (1<<30)

typedef enum {
    KEY_UNKNOWN = 0,

    /* Control characters */
    KEY_NUL = 0,
    KEY_SOH = 1,
    KEY_STX = 2,
    KEY_ETX = 3,
    KEY_EOT = 4,
    KEY_ENQ = 5,
    KEY_ACK = 6,
    KEY_BEL = 7,
    KEY_BS = 8, // BACKSPACE
    KEY_TAB = 9,
    KEY_LF = 10,
    KEY_VT = 11,
    KEY_FF = 12,
    KEY_CR = 13, // RETURN
    KEY_SO = 14,
    KEY_SI = 15,
    KEY_DLE = 16,
    KEY_DC1 = 17,
    KEY_DC2 = 18,
    KEY_DC3 = 19,
    KEY_DC4 = 20,
    KEY_NAK = 21,
    KEY_SYN = 22,
    KEY_ETB = 23,
    KEY_CAN = 24,
    KEY_EM = 25,
    KEY_SUB = 26,
    KEY_ESC = 27, // ESCAPE
    KEY_FS = 28,
    KEY_GS = 29,
    KEY_RS = 30,
    KEY_US = 31,


    KEY_RETURN = '\r',
    KEY_ESCAPE = '\033',
    KEY_BACKSPACE = '\b',
    // KEY_TAB = '\t',
    KEY_SPACE = ' ',

    KEY_EXCLAIM = '!',
    KEY_QUOTEDBL = '"',
    KEY_HASH = '#',
    KEY_PERCENT = '%',
    KEY_DOLLAR = '$',
    KEY_AMPERSAND = '&',
    KEY_QUOTE = '\'',
    KEY_LEFTPAREN = '(',
    KEY_RIGHTPAREN = ')',
    KEY_ASTERISK = '*',
    KEY_PLUS = '+',
    KEY_COMMA = ',',
    KEY_MINUS = '-',
    KEY_PERIOD = '.',
    KEY_SLASH = '/',

    KEY_COLON = ':',
    KEY_SEMICOLON = ';',
    KEY_LESS = '<',
    KEY_EQUALS = '=',
    KEY_GREATER = '>',
    KEY_QUESTION = '?',
    KEY_AT = '@',

    KEY_LEFTBRACKET = '[',
    KEY_BACKSLASH = '\\',
    KEY_RIGHTBRACKET = ']',
    KEY_CARET = '^',
    KEY_UNDERSCORE = '_',
    KEY_BACKQUOTE = '`',

    KEY_0 = '0',
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,

    KEY_A = 'a',
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    KEY_LSHIFT = _FROM_SCANCODE(1),
    KEY_RSHIFT,
    KEY_LCTRL,
    KEY_RCTRL,
    KEY_LALT,
    KEY_RALT,
    KEY_LGUI,
    KEY_RGUI,
    KEY_NUM,
    KEY_CAPS,

    KEY_PRINTSCREEN,
    KEY_SCROLLLOCK,
    KEY_PAUSE,
    KEY_INSERT,
    KEY_HOME,
    KEY_PAGEUP,
    KEY_DELETE,
    KEY_END,
    KEY_PAGEDOWN,
    KEY_RIGHT,
    KEY_LEFT,
    KEY_DOWN,
    KEY_UP,

    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,

    KEY_BREAK,

} keycode_t;

typedef enum {
    KMOD_NONE = 0x000,
    KMOD_LSHIFT = 0x001,
    KMOD_RSHIFT = 0x002,
    KMOD_LCTRL = 0x004,
    KMOD_RCTRL = 0x008,
    KMOD_LALT = 0x010,
    KMOD_RALT = 0x020,
    KMOD_ALTGR = KMOD_RALT,
    KMOD_LGUI = 0x040,
    KMOD_RGUI = 0x080,
    KMOD_NUM = 0x100,
    KMOD_CAPS = 0x200,
    KMOD_HOST = KMOD_LGUI,
    KMOD_SCROLL = 0x1000,
} keymod_t;

#define KMOD_CTRL   (KMOD_LCTRL|KMOD_RCTRL)
#define KMOD_SHIFT  (KMOD_LSHIFT|KMOD_RSHIFT)
#define KMOD_ALT    (KMOD_LALT|KMOD_RALT)
#define KMOD_GUI    (KMOD_LGUI|KMOD_RGUI)




#endif  /* _KEYCODES_H */

