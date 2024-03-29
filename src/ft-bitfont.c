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
#include <gfx.h>
#include <stdlib.h>
#include "mcrs.h"

typedef struct font_bmp font_bmp_t;

struct font_bmp {
    const uint8_t *glyphs;
    char glyph_size;
    char width;
    char height;
    char dispx;
    char dispy;
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
const uint8_t glyph_6x10[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ' '
    0x04, 0x41, 0x10, 0x04, 0x01, 0x10, 0x00, 0x00,  // '!'
    0x14, 0x45, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,  // '"'
    0x00, 0x45, 0xfd, 0xd4, 0xaf, 0x28, 0x00, 0x00,  // '#'
    0x08, 0xa7, 0x28, 0x1c, 0x4a, 0x7a, 0x04, 0x00,  // '$'
    0x67, 0xf5, 0x20, 0x84, 0xae, 0xe6, 0x00, 0x00,  // '%'
    0x8e, 0xa2, 0x18, 0x57, 0x16, 0xf9, 0x00, 0x00,  // '&'
    0x04, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '''
    0x10, 0x82, 0x10, 0x04, 0x41, 0x20, 0x08, 0x04,  // '('
    0x02, 0x41, 0x20, 0x08, 0x82, 0x10, 0x84, 0x00,  // ')'
    0x88, 0xca, 0xa9, 0x08, 0x00, 0x00, 0x00, 0x00,  // '*'
    0x00, 0x80, 0x20, 0x3e, 0x82, 0x00, 0x00, 0x00,  // '+'
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x30, 0x8c, 0x01,  // ','
    0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00,  // '-'
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x30, 0x00, 0x00,  // '.'
    0x20, 0x04, 0x21, 0x08, 0x42, 0x10, 0x02, 0x00,  // '/'
    0x80, 0x13, 0x65, 0xd5, 0x14, 0x39, 0x00, 0x00,  // '0'
    0x00, 0xa3, 0x20, 0x08, 0x82, 0xf8, 0x00, 0x00,  // '1'
    0x00, 0x27, 0x82, 0x10, 0x42, 0xf8, 0x00, 0x00,  // '2'
    0x80, 0x03, 0x41, 0x0c, 0x04, 0x39, 0x00, 0x00,  // '3'
    0x00, 0x46, 0x49, 0xd1, 0x0f, 0x41, 0x00, 0x00,  // '4'
    0x80, 0x27, 0x08, 0x0e, 0x04, 0x39, 0x00, 0x00,  // '5'
    0x00, 0x67, 0x08, 0x9e, 0x28, 0x72, 0x00, 0x00,  // '6'
    0x80, 0x0f, 0x42, 0x10, 0x82, 0x10, 0x00, 0x00,  // '7'
    0x00, 0x27, 0x9a, 0x9c, 0x2c, 0x72, 0x00, 0x00,  // '8'
    0x00, 0x27, 0x8a, 0x3c, 0x08, 0x39, 0x00, 0x00,  // '9'
    0x00, 0xc0, 0x30, 0x00, 0xc0, 0x30, 0x00, 0x00,  // ':'
    0x00, 0xc0, 0x30, 0x00, 0xc0, 0x30, 0x8c, 0x01,  // ';'
    0x00, 0x00, 0x21, 0x04, 0x81, 0x40, 0x00, 0x00,  // '<'
    0x00, 0x00, 0x00, 0x3e, 0xe0, 0x03, 0x00, 0x00,  // '='
    0x00, 0x20, 0x10, 0x08, 0x42, 0x08, 0x00, 0x00,  // '>'
    0x0c, 0x06, 0x41, 0x0c, 0x01, 0x10, 0x00, 0x00,  // '?'
    0x9c, 0x2c, 0xa6, 0x6d, 0xdb, 0x05, 0x82, 0x07,  // '@'
    0x00, 0xa1, 0x28, 0xca, 0x17, 0x45, 0x00, 0x00,  // 'A'
    0xc0, 0x13, 0x45, 0x4f, 0x14, 0x3d, 0x00, 0x00,  // 'B'
    0x80, 0x33, 0x04, 0x41, 0x30, 0x38, 0x00, 0x00,  // 'C'
    0xc0, 0x93, 0x45, 0x51, 0x94, 0x3d, 0x00, 0x00,  // 'D'
    0x80, 0x27, 0x08, 0x9e, 0x20, 0x78, 0x00, 0x00,  // 'E'
    0x80, 0x27, 0x08, 0x9e, 0x20, 0x08, 0x00, 0x00,  // 'F'
    0x80, 0x27, 0x04, 0x59, 0x34, 0x79, 0x00, 0x00,  // 'G'
    0x80, 0x24, 0x49, 0x9e, 0x24, 0x49, 0x00, 0x00,  // 'H'
    0x80, 0x8f, 0x20, 0x08, 0x82, 0xf8, 0x00, 0x00,  // 'I'
    0x80, 0x07, 0x41, 0x10, 0x04, 0x39, 0x00, 0x00,  // 'J'
    0x80, 0xa4, 0x28, 0x86, 0xa2, 0x48, 0x00, 0x00,  // 'K'
    0x80, 0x20, 0x08, 0x82, 0x20, 0x78, 0x00, 0x00,  // 'L'
    0x80, 0x24, 0xb5, 0x6d, 0x18, 0x86, 0x00, 0x00,  // 'M'
    0x80, 0x64, 0x59, 0x96, 0xa6, 0x49, 0x00, 0x00,  // 'N'
    0x80, 0x37, 0x87, 0x61, 0x38, 0x7b, 0x00, 0x00,  // 'O'
    0x80, 0x27, 0x8a, 0x9e, 0x20, 0x08, 0x00, 0x00,  // 'P'
    0x80, 0x33, 0x45, 0x51, 0x94, 0x39, 0x04, 0x06,  // 'Q'
    0x80, 0x23, 0x49, 0x8e, 0x22, 0x89, 0x00, 0x00,  // 'R'
    0x00, 0x2f, 0x08, 0x1c, 0x08, 0x7a, 0x00, 0x00,  // 'S'
    0x80, 0x8f, 0x20, 0x08, 0x82, 0x20, 0x00, 0x00,  // 'T'
    0x80, 0x28, 0x8a, 0xa2, 0x28, 0x72, 0x00, 0x00,  // 'U'
    0x80, 0x28, 0x8a, 0x14, 0x45, 0x21, 0x00, 0x00,  // 'V'
    0x40, 0x18, 0x86, 0xad, 0xa7, 0x49, 0x00, 0x00,  // 'W'
    0x40, 0x28, 0x31, 0x0c, 0x23, 0xcd, 0x00, 0x00,  // 'X'
    0x40, 0x20, 0x52, 0x08, 0x82, 0x20, 0x00, 0x00,  // 'Y'
    0x80, 0x0f, 0x41, 0x08, 0x41, 0x78, 0x00, 0x00,  // 'Z'
    0x1c, 0x41, 0x10, 0x04, 0x41, 0x10, 0x04, 0x07,  // '['
    0x20, 0x04, 0x21, 0x08, 0x42, 0x10, 0x02, 0x00,  // '\'
    0x0e, 0x82, 0x20, 0x08, 0x82, 0x20, 0x88, 0x03,  // ']'
    0x00, 0xa1, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00,  // '^'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x0f,  // '_'
    0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '`'
    0x00, 0xc0, 0x81, 0xbc, 0x28, 0xf3, 0x00, 0x00,  // 'a'
    0x82, 0xe0, 0x99, 0xa2, 0x28, 0x7a, 0x00, 0x00,  // 'b'
    0x00, 0xc0, 0x19, 0x82, 0x20, 0x70, 0x00, 0x00,  // 'c'
    0x20, 0xc8, 0x8b, 0xa2, 0x28, 0xb3, 0x00, 0x00,  // 'd'
    0x00, 0xc0, 0x89, 0xbe, 0x60, 0xf0, 0x00, 0x00,  // 'e'
    0x38, 0x41, 0x7c, 0x04, 0x41, 0x10, 0x00, 0x00,  // 'f'
    0x00, 0xc0, 0x8b, 0xa2, 0x27, 0xf8, 0xa2, 0x07,  // 'g'
    0x82, 0xf0, 0x49, 0x92, 0x24, 0x49, 0x00, 0x00,  // 'h'
    0x18, 0xe0, 0x20, 0x08, 0x82, 0x78, 0x00, 0x00,  // 'i'
    0x18, 0xe0, 0x41, 0x10, 0x04, 0x41, 0x90, 0x03,  // 'j'
    0x82, 0x20, 0x29, 0x86, 0xa1, 0x48, 0x00, 0x00,  // 'k'
    0x0e, 0x82, 0x20, 0x08, 0x82, 0x78, 0x00, 0x00,  // 'l'
    0x00, 0xe0, 0xab, 0xaa, 0xaa, 0xaa, 0x00, 0x00,  // 'm'
    0x00, 0xe0, 0x49, 0x92, 0x24, 0x49, 0x00, 0x00,  // 'n'
    0x00, 0xc0, 0x89, 0xa2, 0x28, 0x72, 0x00, 0x00,  // 'o'
    0x00, 0xe0, 0x99, 0xa2, 0x28, 0x7a, 0x82, 0x00,  // 'p'
    0x00, 0xc0, 0x8b, 0xa2, 0x28, 0xb3, 0x20, 0x08,  // 'q'
    0x00, 0xe0, 0x49, 0x82, 0x20, 0x08, 0x00, 0x00,  // 'r'
    0x00, 0xc0, 0x09, 0x06, 0x06, 0x39, 0x00, 0x00,  // 's'
    0x04, 0xf1, 0x11, 0x04, 0x41, 0x70, 0x00, 0x00,  // 't'
    0x00, 0x20, 0x49, 0x92, 0x24, 0x79, 0x00, 0x00,  // 'u'
    0x00, 0x20, 0x8a, 0x22, 0x45, 0x21, 0x00, 0x00,  // 'v'
    0x00, 0x10, 0x56, 0x55, 0xb7, 0x49, 0x00, 0x00,  // 'w'
    0x00, 0x30, 0x4b, 0x0c, 0x23, 0xcd, 0x00, 0x00,  // 'x'
    0x00, 0x20, 0x8a, 0x12, 0xc5, 0x20, 0xc4, 0x01,  // 'y'
    0x00, 0xe0, 0x21, 0x08, 0x41, 0x78, 0x00, 0x00,  // 'z'
    0x0c, 0x41, 0x10, 0x03, 0x41, 0x10, 0x04, 0x03,  // '{'
    0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x02,  // '|'
    0x0c, 0x82, 0x20, 0x30, 0x82, 0x20, 0x08, 0x03,  // '}'
    0x00, 0x00, 0x00, 0x57, 0x07, 0x00, 0x00, 0x00,  // '~'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ''
};

const font_bmp_t font_6x10 = {
    .glyphs = glyph_6x10,
    .glyph_size = 8,
    .width = 6,
    .height = 10,
    .dispx = 7,
    .dispy = 10,
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
const uint8_t glyph_8x15[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ' '
    0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00,  // '!'
    0x48, 0x24, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '"'
    0x00, 0x14, 0x14, 0x14, 0x7f, 0x14, 0x14, 0x7f, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00,  // '#'
    0x00, 0x00, 0x08, 0x08, 0x08, 0x3e, 0x49, 0x09, 0x3e, 0x48, 0x49, 0x3e, 0x08, 0x08, 0x08,  // '$'
    0x80, 0x46, 0x49, 0x29, 0x26, 0x10, 0x10, 0x08, 0x08, 0x64, 0x94, 0x92, 0x62, 0x01, 0x00,  // '%'
    0x00, 0x00, 0x00, 0x0c, 0x12, 0x12, 0x0c, 0x4e, 0x51, 0x31, 0x31, 0x4e, 0x00, 0x00, 0x00,  // '&'
    0x00, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '''
    0x10, 0x08, 0x08, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x08, 0x08, 0x10, 0x00, 0x00,  // '('
    0x04, 0x08, 0x08, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x08, 0x08, 0x04, 0x00, 0x00,  // ')'
    0x00, 0x00, 0x00, 0x00, 0x12, 0x0c, 0x3f, 0x0c, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '*'
    0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x7f, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,  // '+'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x04, 0x00,  // ','
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '-'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00,  // '.'
    0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x01, 0x00,  // '/'
    0x00, 0x1c, 0x22, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x22, 0x1c, 0x00, 0x00, 0x00,  // '0'
    0x00, 0x10, 0x18, 0x14, 0x12, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00,  // '1'
    0x00, 0x3e, 0x41, 0x41, 0x41, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x7f, 0x00, 0x00, 0x00,  // '2'
    0x00, 0x3e, 0x41, 0x40, 0x40, 0x40, 0x3e, 0x40, 0x40, 0x40, 0x41, 0x3e, 0x00, 0x00, 0x00,  // '3'
    0x00, 0x18, 0x14, 0x14, 0x12, 0x12, 0x11, 0x11, 0x7f, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00,  // '4'
    0x00, 0x7f, 0x01, 0x01, 0x01, 0x01, 0x3e, 0x40, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00, 0x00,  // '5'
    0x00, 0x3e, 0x41, 0x01, 0x01, 0x01, 0x3d, 0x43, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00, 0x00,  // '6'
    0x00, 0x7f, 0x41, 0x41, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04, 0x00, 0x00, 0x00,  // '7'
    0x00, 0x3e, 0x41, 0x41, 0x41, 0x41, 0x3e, 0x41, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00, 0x00,  // '8'
    0x00, 0x3e, 0x41, 0x41, 0x41, 0x41, 0x7e, 0x40, 0x40, 0x40, 0x40, 0x3e, 0x00, 0x00, 0x00,  // '9'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00,  // ':'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x04, 0x00,  // ';'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x10, 0x08, 0x04, 0x08, 0x10, 0x20, 0x00, 0x00, 0x00,  // '<'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '='
    0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00,  // '>'
    0x00, 0x1e, 0x21, 0x21, 0x21, 0x10, 0x08, 0x08, 0x08, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00,  // '?'
    0x00, 0x1c, 0x22, 0x41, 0x59, 0x65, 0x45, 0x65, 0x59, 0x01, 0x42, 0x3c, 0x00, 0x00, 0x00,  // '@'
    0x00, 0x08, 0x08, 0x14, 0x14, 0x22, 0x22, 0x22, 0x7f, 0x41, 0x41, 0x41, 0x00, 0x00, 0x00,  // 'A'
    0x00, 0x3f, 0x41, 0x41, 0x41, 0x41, 0x3f, 0x41, 0x41, 0x41, 0x41, 0x3f, 0x00, 0x00, 0x00,  // 'B'
    0x00, 0x3e, 0x41, 0x41, 0x01, 0x01, 0x01, 0x01, 0x01, 0x41, 0x41, 0x3e, 0x00, 0x00, 0x00,  // 'C'
    0x00, 0x3f, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3f, 0x00, 0x00, 0x00,  // 'D'
    0x00, 0x7f, 0x01, 0x01, 0x01, 0x01, 0x3f, 0x01, 0x01, 0x01, 0x01, 0x7f, 0x00, 0x00, 0x00,  // 'E'
    0x00, 0x7f, 0x01, 0x01, 0x01, 0x01, 0x1f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,  // 'F'
    0x00, 0x3e, 0x41, 0x41, 0x01, 0x01, 0x79, 0x41, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00, 0x00,  // 'G'
    0x00, 0x41, 0x41, 0x41, 0x41, 0x41, 0x7f, 0x41, 0x41, 0x41, 0x41, 0x41, 0x00, 0x00, 0x00,  // 'H'
    0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00,  // 'I'
    0x00, 0x7c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x0e, 0x00, 0x00, 0x00,  // 'J'
    0x00, 0x41, 0x21, 0x11, 0x09, 0x05, 0x03, 0x05, 0x09, 0x11, 0x21, 0x41, 0x00, 0x00, 0x00,  // 'K'
    0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x7f, 0x00, 0x00, 0x00,  // 'L'
    0x00, 0x41, 0x63, 0x63, 0x55, 0x55, 0x49, 0x49, 0x41, 0x41, 0x41, 0x41, 0x00, 0x00, 0x00,  // 'M'
    0x00, 0x41, 0x43, 0x45, 0x45, 0x49, 0x49, 0x51, 0x51, 0x61, 0x41, 0x41, 0x00, 0x00, 0x00,  // 'N'
    0x00, 0x3e, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00, 0x00,  // 'O'
    0x00, 0x3f, 0x41, 0x41, 0x41, 0x41, 0x3f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,  // 'P'
    0x00, 0x3e, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x51, 0x3e, 0x10, 0x60, 0x00,  // 'Q'
    0x00, 0x3f, 0x41, 0x41, 0x41, 0x41, 0x3f, 0x05, 0x09, 0x11, 0x21, 0x41, 0x00, 0x00, 0x00,  // 'R'
    0x00, 0x3e, 0x41, 0x41, 0x01, 0x01, 0x3e, 0x40, 0x40, 0x41, 0x41, 0x3e, 0x00, 0x00, 0x00,  // 'S'
    0x00, 0x7f, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00,  // 'T'
    0x00, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00, 0x00,  // 'U'
    0x00, 0x41, 0x41, 0x41, 0x41, 0x22, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00, 0x00,  // 'V'
    0x00, 0x41, 0x41, 0x41, 0x41, 0x49, 0x49, 0x49, 0x55, 0x55, 0x63, 0x41, 0x00, 0x00, 0x00,  // 'W'
    0x00, 0x41, 0x41, 0x41, 0x22, 0x14, 0x08, 0x14, 0x22, 0x41, 0x41, 0x41, 0x00, 0x00, 0x00,  // 'X'
    0x00, 0x41, 0x41, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00,  // 'Y'
    0x00, 0x7f, 0x40, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01, 0x7f, 0x00, 0x00, 0x00,  // 'Z'
    0x1c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1c, 0x00, 0x00,  // '['
    0x01, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0x00,  // '\'
    0x1c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1c, 0x00, 0x00,  // ']'
    0x00, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '^'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00,  // '_'
    0x00, 0x0c, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '`'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x5e, 0x61, 0x41, 0x41, 0x41, 0x61, 0x5e, 0x00, 0x00, 0x00,  // 'a'
    0x00, 0x01, 0x01, 0x01, 0x01, 0x3d, 0x43, 0x41, 0x41, 0x41, 0x43, 0x3d, 0x00, 0x00, 0x00,  // 'b'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x41, 0x01, 0x01, 0x01, 0x41, 0x3e, 0x00, 0x00, 0x00,  // 'c'
    0x00, 0x40, 0x40, 0x40, 0x40, 0x5e, 0x61, 0x41, 0x41, 0x41, 0x61, 0x5e, 0x00, 0x00, 0x00,  // 'd'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x41, 0x41, 0x7f, 0x01, 0x41, 0x3e, 0x00, 0x00, 0x00,  // 'e'
    0x00, 0x18, 0x24, 0x24, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00,  // 'f'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x5e, 0x61, 0x41, 0x41, 0x41, 0x61, 0x5e, 0x40, 0x41, 0x3e,  // 'g'
    0x00, 0x01, 0x01, 0x01, 0x01, 0x3d, 0x43, 0x41, 0x41, 0x41, 0x41, 0x41, 0x00, 0x00, 0x00,  // 'h'
    0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00,  // 'i'
    0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0c,  // 'j'
    0x00, 0x02, 0x02, 0x42, 0x22, 0x12, 0x0a, 0x06, 0x0a, 0x12, 0x22, 0x42, 0x00, 0x00, 0x00,  // 'k'
    0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x18, 0x00, 0x00, 0x00,  // 'l'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x4b, 0x49, 0x49, 0x49, 0x49, 0x49, 0x00, 0x00, 0x00,  // 'm'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x43, 0x41, 0x41, 0x41, 0x41, 0x41, 0x00, 0x00, 0x00,  // 'n'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3e, 0x00, 0x00, 0x00,  // 'o'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x43, 0x41, 0x41, 0x41, 0x43, 0x3d, 0x01, 0x01, 0x01,  // 'p'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x5e, 0x61, 0x41, 0x41, 0x41, 0x61, 0x5e, 0x40, 0x40, 0x40,  // 'q'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x43, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,  // 'r'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x41, 0x01, 0x3e, 0x40, 0x41, 0x3e, 0x00, 0x00, 0x00,  // 's'
    0x00, 0x04, 0x04, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x04, 0x04, 0x24, 0x18, 0x00, 0x00, 0x00,  // 't'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x41, 0x41, 0x41, 0x41, 0x43, 0x3d, 0x00, 0x00, 0x00,  // 'u'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x41, 0x22, 0x22, 0x14, 0x14, 0x08, 0x00, 0x00, 0x00,  // 'v'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x41, 0x49, 0x49, 0x49, 0x55, 0x22, 0x00, 0x00, 0x00,  // 'w'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x22, 0x14, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00,  // 'x'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x41, 0x41, 0x41, 0x61, 0x5e, 0x40, 0x40, 0x41, 0x3e,  // 'y'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x20, 0x10, 0x08, 0x04, 0x02, 0x7f, 0x00, 0x00, 0x00,  // 'z'
    0x00, 0x30, 0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x30, 0x00,  // '{'
    0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,  // '|'
    0x00, 0x0c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x20, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0c, 0x00,  // '}'
    0x00, 0x00, 0x00, 0x00, 0x46, 0x49, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '~'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ''
};

const font_bmp_t font_8x15 = {
    .glyphs = glyph_8x15,
    .glyph_size = 15,
    .width = 8,
    .height = 15,
    .dispx = 9,
    .dispy = 15,
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
const uint8_t glyph_7x13[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ' '
    0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x00, 0x08, 0x04, 0x00, 0x00, 0x00,  // '!'
    0x24, 0x12, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '"'
    0x00, 0x00, 0x0a, 0xe5, 0xa7, 0x50, 0x7e, 0x0a, 0x05, 0x00, 0x00, 0x00,  // '#'
    0x00, 0x04, 0x82, 0xa3, 0x52, 0xf0, 0x50, 0x2a, 0x0e, 0x02, 0x01, 0x00,  // '$'
    0xa6, 0x54, 0xc6, 0x82, 0x40, 0xd0, 0x98, 0x4a, 0x19, 0x00, 0x00, 0x00,  // '%'
    0x00, 0x86, 0x44, 0xc2, 0xb0, 0x86, 0x42, 0x21, 0x2f, 0x00, 0x00, 0x00,  // '&'
    0x08, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '''
    0x10, 0x04, 0x81, 0x40, 0x20, 0x10, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00,  // '('
    0x04, 0x04, 0x04, 0x02, 0x81, 0x40, 0x20, 0x08, 0x02, 0x00, 0x00, 0x00,  // ')'
    0x00, 0x00, 0x80, 0x84, 0xf1, 0x63, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00,  // '*'
    0x00, 0x00, 0x00, 0x81, 0xf0, 0x21, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,  // '+'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x04, 0x01, 0x00, 0x00,  // ','
    0x00, 0x00, 0x00, 0x00, 0xf0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '-'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x04, 0x00, 0x00, 0x00,  // '.'
    0x20, 0x10, 0x04, 0x82, 0x40, 0x10, 0x08, 0x02, 0x01, 0x00, 0x00, 0x00,  // '/'
    0x3c, 0xa1, 0x50, 0x2c, 0x55, 0x1a, 0x85, 0x42, 0x1e, 0x00, 0x00, 0x00,  // '0'
    0x08, 0x86, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x1f, 0x00, 0x00, 0x00,  // '1'
    0x3c, 0xa1, 0x10, 0x08, 0x82, 0x20, 0x08, 0x02, 0x3f, 0x00, 0x00, 0x00,  // '2'
    0x3c, 0xa1, 0x10, 0x88, 0x03, 0x02, 0x85, 0x42, 0x1e, 0x00, 0x00, 0x00,  // '3'
    0x40, 0x30, 0x14, 0x49, 0x14, 0xfa, 0x81, 0x40, 0x20, 0x00, 0x00, 0x00,  // '4'
    0x7e, 0x81, 0x40, 0xe0, 0x03, 0x02, 0x81, 0x42, 0x1e, 0x00, 0x00, 0x00,  // '5'
    0x38, 0x82, 0x40, 0xe0, 0x13, 0x0a, 0x85, 0x42, 0x1e, 0x00, 0x00, 0x00,  // '6'
    0x7e, 0x20, 0x10, 0x04, 0x82, 0x40, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00,  // '7'
    0x3c, 0xa1, 0x50, 0xc8, 0x13, 0x0a, 0x85, 0x42, 0x1e, 0x00, 0x00, 0x00,  // '8'
    0x3c, 0xa1, 0x50, 0x28, 0xe4, 0x03, 0x81, 0x20, 0x0e, 0x00, 0x00, 0x00,  // '9'
    0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x08, 0x04, 0x00, 0x00, 0x00,  // ';'
    0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x08, 0x04, 0x01, 0x00, 0x00,  // ':'
    0x00, 0x10, 0x04, 0x41, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00,  // '<'
    0x00, 0x00, 0x00, 0xe0, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '='
    0x00, 0x01, 0x01, 0x01, 0x01, 0x41, 0x10, 0x04, 0x01, 0x00, 0x00, 0x00,  // '>'
    0x3c, 0xa1, 0x50, 0x08, 0x82, 0x40, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00,  // '?'
    0xbe, 0x60, 0xbe, 0x58, 0x2c, 0x96, 0xb3, 0x01, 0x3f, 0x00, 0x00, 0x00,  // '@'
    0x3c, 0xa1, 0x50, 0x28, 0xf4, 0x0b, 0x85, 0x42, 0x21, 0x00, 0x00, 0x00,  // 'A'
    0x3e, 0xa1, 0x50, 0xe8, 0x13, 0x0a, 0x85, 0x42, 0x1f, 0x00, 0x00, 0x00,  // 'B'
    0x3c, 0xa1, 0x50, 0x20, 0x10, 0x08, 0x84, 0x42, 0x1e, 0x00, 0x00, 0x00,  // 'C'
    0x1e, 0x91, 0x50, 0x28, 0x14, 0x0a, 0x85, 0x22, 0x0f, 0x00, 0x00, 0x00,  // 'D'
    0x7e, 0x81, 0x40, 0xe0, 0x11, 0x08, 0x04, 0x02, 0x3f, 0x00, 0x00, 0x00,  // 'E'
    0x7e, 0x81, 0x40, 0xe0, 0x11, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00,  // 'F'
    0x3c, 0xa1, 0x40, 0x20, 0x10, 0x88, 0x85, 0x42, 0x1e, 0x00, 0x00, 0x00,  // 'G'
    0x42, 0xa1, 0x50, 0xe8, 0x17, 0x0a, 0x85, 0x42, 0x21, 0x00, 0x00, 0x00,  // 'H'
    0x1c, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x0e, 0x00, 0x00, 0x00,  // 'I'
    0x70, 0x10, 0x08, 0x04, 0x02, 0x81, 0x44, 0x22, 0x0e, 0x00, 0x00, 0x00,  // 'J'
    0x42, 0x91, 0x44, 0x61, 0x30, 0x28, 0x24, 0x22, 0x21, 0x00, 0x00, 0x00,  // 'K'
    0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x3f, 0x00, 0x00, 0x00,  // 'L'
    0xc1, 0x71, 0x35, 0x19, 0x0c, 0x06, 0x83, 0xc1, 0x20, 0x00, 0x00, 0x00,  // 'M'
    0x42, 0xa1, 0xd0, 0xa8, 0x94, 0x8a, 0x85, 0x42, 0x21, 0x00, 0x00, 0x00,  // 'N'
    0x3c, 0xa1, 0x50, 0x28, 0x14, 0x0a, 0x85, 0x42, 0x1e, 0x00, 0x00, 0x00,  // 'O'
    0x3e, 0xa1, 0x50, 0x28, 0xf4, 0x09, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00,  // 'P'
    0x3c, 0xa1, 0x50, 0x28, 0x14, 0x0a, 0x85, 0x52, 0x1e, 0x10, 0x00, 0x00,  // 'Q'
    0x3e, 0xa1, 0x50, 0x28, 0xf4, 0x29, 0x24, 0x22, 0x21, 0x00, 0x00, 0x00,  // 'R'
    0x3c, 0xa1, 0x40, 0xc0, 0x03, 0x02, 0x85, 0x42, 0x1e, 0x00, 0x00, 0x00,  // 'S'
    0x7f, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00,  // 'T'
    0x42, 0xa1, 0x50, 0x28, 0x14, 0x0a, 0x85, 0x42, 0x1e, 0x00, 0x00, 0x00,  // 'U'
    0x42, 0xa1, 0x50, 0x28, 0x24, 0x91, 0x48, 0x18, 0x0c, 0x00, 0x00, 0x00,  // 'V'
    0xc1, 0x60, 0x30, 0x18, 0x4c, 0x26, 0xab, 0xe3, 0x20, 0x00, 0x00, 0x00,  // 'W'
    0x42, 0x21, 0x89, 0x84, 0xc1, 0x90, 0x48, 0x42, 0x21, 0x00, 0x00, 0x00,  // 'X'
    0xc1, 0xa0, 0x48, 0x44, 0x41, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00,  // 'Y'
    0x7e, 0x20, 0x10, 0x04, 0x41, 0x10, 0x04, 0x02, 0x3f, 0x00, 0x00, 0x00,  // 'Z'
    0x1c, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x0e, 0x00, 0x00, 0x00,  // '['
    0x02, 0x01, 0x81, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x00, 0x00, 0x00,  // '\'
    0x1c, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x0e, 0x00, 0x00, 0x00,  // ']'
    0x08, 0x8a, 0x28, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '^'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00,  // '_'
    0x04, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '`'
    0x00, 0x00, 0x80, 0x07, 0xe4, 0x0b, 0x85, 0x42, 0x3e, 0x00, 0x00, 0x00,  // 'a'
    0x02, 0x81, 0xc0, 0x27, 0x14, 0x0a, 0x85, 0x42, 0x1f, 0x00, 0x00, 0x00,  // 'b'
    0x00, 0x00, 0x80, 0x27, 0x14, 0x08, 0x04, 0x42, 0x1e, 0x00, 0x00, 0x00,  // 'c'
    0x40, 0x20, 0x90, 0x2f, 0x14, 0x0a, 0x85, 0x42, 0x3e, 0x00, 0x00, 0x00,  // 'd'
    0x00, 0x00, 0x80, 0x27, 0x14, 0xfa, 0x05, 0x02, 0x1e, 0x00, 0x00, 0x00,  // 'e'
    0x70, 0x04, 0xc2, 0x87, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00,  // 'f'
    0x00, 0x00, 0x80, 0x2f, 0x14, 0x0a, 0x85, 0x42, 0x3e, 0x10, 0xc8, 0x03,  // 'g'
    0x02, 0x81, 0xc0, 0x27, 0x14, 0x0a, 0x85, 0x42, 0x21, 0x00, 0x00, 0x00,  // 'h'
    0x08, 0x04, 0x80, 0x81, 0x40, 0x20, 0x10, 0x08, 0x0e, 0x00, 0x00, 0x00,  // 'i'
    0x08, 0x04, 0x80, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0xe1, 0x00,  // 'j'
    0x02, 0x81, 0x40, 0x28, 0x92, 0x38, 0x24, 0x22, 0x21, 0x00, 0x00, 0x00,  // 'k'
    0x0c, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x0e, 0x00, 0x00, 0x00,  // 'l'
    0x00, 0x00, 0xe0, 0x97, 0x4c, 0x26, 0x93, 0xc9, 0x24, 0x00, 0x00, 0x00,  // 'm'
    0x00, 0x00, 0xc0, 0x27, 0x14, 0x0a, 0x85, 0x42, 0x21, 0x00, 0x00, 0x00,  // 'n'
    0x00, 0x00, 0x80, 0x27, 0x14, 0x0a, 0x85, 0x42, 0x1e, 0x00, 0x00, 0x00,  // 'o'
    0x00, 0x00, 0xc0, 0x27, 0x14, 0x0a, 0x85, 0x42, 0x9f, 0x40, 0x20, 0x00,  // 'p'
    0x00, 0x00, 0x80, 0x2f, 0x14, 0x0a, 0x85, 0x42, 0x3e, 0x10, 0x08, 0x04,  // 'q'
    0x00, 0x00, 0x40, 0x6f, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00,  // 'r'
    0x00, 0x00, 0x80, 0x2f, 0x10, 0xf0, 0x80, 0x40, 0x1f, 0x00, 0x00, 0x00,  // 's'
    0x08, 0x04, 0xc2, 0x87, 0x40, 0x20, 0x10, 0x08, 0x38, 0x00, 0x00, 0x00,  // 't'
    0x00, 0x00, 0x40, 0x28, 0x14, 0x0a, 0x85, 0x42, 0x3e, 0x00, 0x00, 0x00,  // 'u'
    0x00, 0x00, 0x40, 0x28, 0x14, 0x92, 0x48, 0x18, 0x0c, 0x00, 0x00, 0x00,  // 'v'
    0x00, 0x00, 0x20, 0x18, 0x4c, 0x26, 0x93, 0x49, 0x1f, 0x00, 0x00, 0x00,  // 'w'
    0x00, 0x00, 0x40, 0x28, 0x24, 0x61, 0x48, 0x42, 0x21, 0x00, 0x00, 0x00,  // 'x'
    0x00, 0x00, 0x40, 0x28, 0x14, 0x0a, 0x85, 0x42, 0x3e, 0x10, 0xc8, 0x03,  // 'y'
    0x00, 0x00, 0xc0, 0x0f, 0x82, 0x20, 0x08, 0x02, 0x3f, 0x00, 0x00, 0x00,  // 'z'
    0x30, 0x04, 0x02, 0x81, 0x20, 0x20, 0x10, 0x08, 0x04, 0x0c, 0x00, 0x00,  // '{'
    0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00,  // '|'
    0x06, 0x04, 0x02, 0x81, 0x80, 0x20, 0x10, 0x08, 0x84, 0x01, 0x00, 0x00,  // '}'
    0x00, 0x00, 0xc0, 0x98, 0x8c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '~'
    0x00, 0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ''
};

const font_bmp_t font_7x13 = {
    .glyphs = glyph_7x13,
    .glyph_size = 12,
    .width = 7,
    .height = 13,
    .dispx = 8,
    .dispy = 13,
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
const uint64_t glyph_6x9[] = {
    0x00000000000000, 0x00004004104104, 0x0000000000028a, 0x0000a7ca51e500, // 20-23
    0x0010f30f1453c4, 0x0004d3821072c8, 0x00016255085246, 0x00000000000104, // 24-27
    0x08104104104108, 0x04208208208204, 0x00000284384000, 0x0000411f104000, // 28-2b
    0x00084180000000, 0x0000000e000000, 0x00006180000000, 0x00082104208410, // 2c-2f

    0x0000f24924924f, 0x00004104104106, 0x0000f0413c820f, 0x0000f20838820f, // 30-33 - 0
    0x0000820f249249, 0x0000f2083c104f, 0x0000f24924f04f, 0x0000208210420f, // 34-37 - 4
    0x0000f2493c924f, 0x0000f20f24924f, 0x00006180186000, 0x00084180186000, // 38-3b - 8
    0x00018181198000, 0x00000380380000, 0x00003310303000, 0x0000400410844e, // 3c-3f

    0x1f05d55d45f000, 0x000114517d145f, 0x0001f4517c924f, 0x0000f04104104f, // 40-23
    0x0000f45145144f, 0x0000f0411c104f, 0x000010411c104f, 0x0001f45174104f, // 44-27 - D
    0x000114517d1451, 0x00002082082082, 0x00007208208208, 0x00009143043149, // 48-2b - H
    0x0000f041041041, 0x0001155555555f, 0x0001145145145f, 0x0001f45145145f, // 4c-2f - L

    0x000010417d145f, 0x0041f55145145f, 0x000114517c924f, 0x0000f2083c104f, // 50-23 - P
    0x0000410410411f, 0x0001f451451451, 0x0000410a291451, 0x0001b555555451, // 54-27 - T
    0x0001144a10a451, 0x000041047d1451, 0x0000f04108420f, 0x0c10410410410c, // 58-2b - X
    0x00410208104082, 0x06104104104106, 0x00000000011284, 0x0003f000000000, // 5c-2f

    0x00000000000204, 0x0000f24f20f000, 0x0000f24924f041, 0x0000f04104f000, // 60-23
    0x0000f24924f208, 0x0000f04f24f000, 0x00001041043047, 0x0f20f24924f000, // 64-27 - d
    0x0000924924f041, 0x00002082082002, 0x000c2082082002, 0x00009143149041, // 68-2b - h
    0x00007041041041, 0x0001555555f000, 0x0000924924f000, 0x0000f24924f000, // 6c-2f - l

    0x0104f24924f000, 0x0820f24924f000, 0x0000104124f000, 0x0000f20f04f000, // 70-23 - p
    0x00007041043041, 0x0000f249249000, 0x0000428a451000, 0x0001f555555000, // 74-27 - t
    0x00009246249000, 0x0f20f249249000, 0x0000f04210f000, 0x18104103104118, // 78-2b - x
    0x04104104104104, 0x03104118104103, 0x00000000352000, 0x0001f7df7df7df, // 7c-2f
};

const font_bmp_t font_6x9 = {
    .glyphs = (const uint8_t *)glyph_6x9,
    .glyph_size = 8,
    .width = 6,
    .height = 9,
    .dispx = 6,
    .dispy = 9,
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
const uint64_t glyph_8x8[] = {
    0x0000000000000000, 0x0000100010101010, 0x0000000000001414, 0x0000143E143E1400,
    0x00103C5038147810, 0x0000609468162906, 0x0000986498040438, 0x0000000000001010,
    0x0000100804040810, 0x0000081020201008, 0x00000054387C3854, 0x000010107C101000,
    0x0008100000000000, 0x000000003C000000, 0x0000100000000000, 0x0000020408102040,

    0x0000182442422418, 0x0000381010101810, 0x00007E041820423C, 0x00003C421820423C,
    0x0000103E12141810, 0x00003C42403E027E, 0x00003C42423E023C, 0x000002040810207E,
    0x00003C42423C423C, 0x00003C407C42423C, 0x0000100000100000, 0x0008100000100000,
    0x0000601806186000, 0x0000007E007E0000, 0x0000061860180600, 0x000008001820221C,

    0x38043A2939223C00, 0x000042423C241818, 0x00001E22221E221E, 0x00001C220101221C,
    0x00001E222222221E, 0x00003E02021E023E, 0x00000202021E023E, 0x00001C223901221C,
    0x00004242427E4242, 0x00007C101010107C, 0x00001C2220202038, 0x000022120E0A1222,
    0x00007E0202020202, 0x0000828292AAC682, 0x00004262524A4642, 0x0000384444444438,

    0x00000202021E221E, 0x0040384444444438, 0x000022120A1E221E, 0x0000384430084438,
    0x00001010101010FE, 0x00003C4242424242, 0x0000182424424281, 0x000082C6AA928282,
    0x0000422418182442, 0x0000101010284482, 0x00007E040810207E, 0x00001C040404041C,
    0x0000402010080402, 0x00001C101010101C, 0x0000000000001408, 0x00007E0000000000,

    0x0000000000001008, 0x00005C627C403C00, 0x00003A46423E0202, 0x0000380404380000,
    0x00005C62427C4040, 0x00003C027E423C00, 0x000008081C081800, 0x1C202C22322C0000,
    0x00002424241C0404, 0x0000101010100010, 0x0E10101010180010, 0x0000340C14240404,
    0x0000180808080808, 0x000042425A660000, 0x000044444C740000, 0x00003C42423C0000,

    0x02023E42463A0000, 0x40407C42625C0000, 0x000004044C340000, 0x00001C2018043800,
    0x00001808083C0800, 0x0000586444440000, 0x0000182442420000, 0x0000665A81810000,
    0x0000661824420000, 0x0608102844420000, 0x00003C08103C0000, 0x000038080C0C0838,
    0x0010101010101010, 0x00001C103030101C, 0x000000324C000000, 0x00007E7E7E7E7E7E,
};

const font_bmp_t font_8x8 = {
    .glyphs = (const uint8_t *)glyph_8x8,
    .glyph_size = 8,
    .width = 8,
    .height = 8,
    .dispx = 8,
    .dispy = 8,
};


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

const font_bmp_t *fonts[] = {
    &font_6x10,
    &font_8x15,
    &font_7x13,
    &font_6x9,
    &font_8x8,
};


int gfx_glyph_bitfont(gfx_t* gfx, const font_bmp_t* face, uint32_t unicode, uint32_t fg, uint32_t bg, int x, int y, const gfx_clip_t* clip)
{
    int l, px, ph;
    int py = y - face->height;
    int gx = MIN(gfx->width, x + face->width);
    int sx = MIN(gfx->width, x + face->dispx);
    int gy = MIN(gfx->height, py + face->height);
    int sy = MIN(gfx->height, py + face->dispy);
    const uint8_t* glyph = &face->glyphs[(unicode - 0x20) * face->glyph_size];

    l = 0;
    if (py < 0)
        l = face->width * -py, py = 0;

    for (; py < gy; ++py) {
        ph = py * gfx->width;
        for (px = x; px < gx; ++px, ++l) 
            if (glyph[l / 8] & (1 << l % 8))
                gfx->pixels4[ph + px] = fg;
        // TODO -- l is late if gx < x + face->width
    }

    return face->dispx;
}

int gfx_glyph_bitfont2(gfx_t* gfx, const font_bmp_t* face, uint32_t unicode, uint32_t fg, uint32_t bg, int x, int y, const gfx_clip_t* clip)
{
    int l, px, ph;
    int py = y - face->height;
    int gx = MIN(gfx->width, x + face->width);
    int sx = MIN(gfx->width, x + face->dispx);
    int gy = MIN(gfx->height, py + face->height);
    int sy = MIN(gfx->height, py + face->dispy);
    const uint8_t* glyph = &face->glyphs[(unicode - 0x20) * face->glyph_size];
    for (l = 0; py < gy; ++py) {
        ph = py * gfx->width;
        for (px = x; px < gx; ++px, ++l)
            gfx->pixels4[ph + px] = (glyph[l / 8] & (1 << l % 8)) ? fg : bg;
        for (; px < sx; ++px)
            gfx->pixels4[ph + px] = bg;
        // TODO -- l is late if gx < x + face->width
    }

    for (; py < sy; ++py) {
        ph = py * gfx->width;
        for (px = x; px < sx; ++px)
            gfx->pixels4[ph + px] = bg;
    }
    return face->dispx;
}

int gfx_mesure_bitfont(const font_bmp_t *face, const char *text, gfx_text_metrics_t *metrics)
{
    metrics->width = face->dispx;
    metrics->height = face->dispy;
    metrics->baseline = face->height;
    return 0;
}

gfx_font_t *gfx_load_bitfont(float size, int style)
{
    const font_bmp_t *face = NULL;
    if (size <= 8)
        face = &font_8x8;
    else if (size <= 9)
        face = &font_6x9;
    else if (size <= 10)
        face = &font_6x10;
    else if (size <= 13)
        face = &font_7x13;
    else if (size <= 15)
        face = &font_8x15;
    if (face == NULL)
        return NULL;

    gfx_font_t *font = malloc(sizeof(gfx_font_t));
    font->mode = GFX_FT_BITFONT;
    font->size = size;
    font->style = 0;
    font->family = NULL;
    font->face = (void *)face;
    return font;
}

void gfx_clear_bitfont(gfx_font_t *font)
{
    free(font);
}
