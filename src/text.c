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
#include "gfx.h"
#include <stdlib.h>

typedef struct font_bmp font_bmp_t;
int gfx_glyph_bitfont(gfx_t *gfx, const font_bmp_t *face, uint32_t unicode, uint32_t fg, uint32_t bg, int x, int y, const gfx_clip_t *clip);
int gfx_mesure_bitfont(const font_bmp_t *face, const char *text, gfx_text_metrics_t *metrics);
gfx_font_t *gfx_load_bitfont(float size, int style);
void gfx_clear_bitfont(gfx_font_t *font);

#if defined __USE_FREETYPE
typedef void *FT_Face;
void gfx_write_prepare_freetype(const gfx_font_t *font);
int gfx_glyph_freetype(gfx_t *gfx, const FT_Face face, uint32_t unicode, uint32_t fg, uint32_t bg, int x, int y, const gfx_clip_t *clip);
int gfx_mesure_freetype(FT_Face face, const char *text, gfx_text_metrics_t *metrics);
gfx_font_t *gfx_load_freetype(const char *family, float size, int style);
void gfx_clear_freetype(gfx_font_t *font);
#endif


LIBAPI int unichar(const char **txt)
{
    const char *str = *txt;
    int charcode = ((unsigned char *)str)[0];
    str++;

    if (charcode & 0x80) { // mbstring !
        int lg = 1;
        if (charcode >= 0xFF) {
            charcode = 0; // 8
            lg = 0; // 7 x 6 => 42bits
        } else if (charcode >= 0xFE) {
            charcode = 0; // 7
            lg = 0; // 6 x 6 => 36bits
        } else if (charcode >= 0xFC) {
            charcode &= 0x1; // 6
            lg = 5; // 5 x 6 + 1 => 31bits
        } else if (charcode >= 0xF8) {
            charcode &= 0x3; // 5
            lg = 4; // 4 x 6 + 2 => 26bits
        } else if (charcode >= 0xF0) {
            charcode &= 0x07; // 4
            lg = 3; // 3 x 6 + 3 => 21bits
        } else if (charcode >= 0xE0) {
            charcode &= 0x0F; // 3
            lg = 2; // 2 x 6 + 4 => 16bits
        } else if (charcode >= 0xC0) {
            charcode &= 0x1F; // 2
            lg = 1; // 6 + 5 => 11bits
        }

        while (lg-- > 0) {
            charcode = (charcode << 6) | (((unsigned char *)str)[0] & 0x3f);
            str++;
        }
    }

    *txt = str;
    return charcode;
}

LIBAPI int utf8char(int uni, char *buf)
{
    if (uni < 0)
        return -1;
    if (uni < 0x80) {
        buf[0] = uni & 0x7f;
        buf[1] = 0;
        return 1;
    }
    if (uni < 0x1000) {
        buf[0] = 0xc0 | ((uni >> 6) & 0x1f);
        buf[1] = 0x80 | (uni & 0x3f);
        buf[2] = 0;
        return 2;
    }
    if (uni < 0x10000) {
        buf[0] = 0xe0 | ((uni >> 12) & 0x0f);
        buf[1] = 0x80 | ((uni >> 6) & 0x3f);
        buf[2] = 0x80 | (uni & 0x3f);
        buf[3] = 0;
        return 3;
    }
    if (uni < 0x200000) {
        buf[0] = 0xf0 | ((uni >> 18) & 0x07);
        buf[1] = 0x80 | ((uni >> 12) & 0x3f);
        buf[2] = 0x80 | ((uni >> 6) & 0x3f);
        buf[2] = 0x80 | (uni & 0x3f);
        buf[3] = 0;
        return 4;
    }
    return -1;
}


LIBAPI int gfx_glyph(gfx_t *gfx, const gfx_font_t *font, uint32_t unicode, uint32_t fg, uint32_t bg, int x, int y, const gfx_clip_t *clip)
{
    if (font->mode == GFX_FT_BITFONT)
        return gfx_glyph_bitfont(gfx, font->face, unicode, fg, bg, x, y, clip);
#if defined __USE_FREETYPE
    if (font->mode == GFX_FT_FREETYPE) {
        gfx_write_prepare_freetype(font);
        return gfx_glyph_freetype(gfx, font->face, unicode, fg, bg, x, y, clip);
    }
#endif
    return -1;
}

LIBAPI int gfx_write(gfx_t *gfx, const gfx_font_t *font, const char *text, uint32_t fg, int x, int y, const gfx_clip_t *clip)
{
#if defined __USE_FREETYPE
    if (font->mode == GFX_FT_FREETYPE)
        gfx_write_prepare_freetype(font);
#endif

    int w;
    while (*text) {
        int unicode = unichar(&text);
        if (unicode < 0)
            return -1;
        if (font->mode == GFX_FT_BITFONT)
            w = gfx_glyph_bitfont(gfx, font->face, unicode, fg, 0, x, y, clip);
#if defined __USE_FREETYPE
        else if (font->mode == GFX_FT_FREETYPE)
            w = gfx_glyph_freetype(gfx, font->face, unicode, fg, 0, x, y, clip);
#endif
        else
            w = 0;
        x += w;
    }
    return 0;
}

LIBAPI gfx_font_t *gfx_font(const char *family, float size, int style)
{
    gfx_font_t *font = NULL;
    if (family == NULL || family[0] == '\0')
        font = gfx_load_bitfont(size, style);
#if defined __USE_FREETYPE
    else
        font = gfx_load_freetype(family, size, style);
#endif
    return font;
}

LIBAPI void gfx_clear_font(gfx_font_t *font)
{
    if (font->mode == GFX_FT_BITFONT)
        gfx_clear_bitfont(font);
#if defined __USE_FREETYPE
    else if (font->mode == GFX_FT_FREETYPE)
        gfx_clear_freetype(font);
#endif
}

LIBAPI int gfx_measure_text(const gfx_font_t *font, const char *text, gfx_text_metrics_t *metrics)
{
    if (font->mode == GFX_FT_BITFONT)
        return gfx_mesure_bitfont(font->face, text, metrics);
#if defined __USE_FREETYPE
    if (font->mode == GFX_FT_FREETYPE) {
        gfx_write_prepare_freetype(font);
        return gfx_mesure_freetype(font->face, text, metrics);
    }
#endif
    return -1;
}
