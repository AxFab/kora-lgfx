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
#include "gfx.h"
#include <stdlib.h>

typedef struct font_bmp font_bmp_t;
int gfx_glyph_bitfont(gfx_t *gfx, const font_bmp_t *face, uint32_t unicode, uint32_t fg, uint32_t bg, int x, int y, const gfx_clip_t *clip);
int gfx_mesure_bitfont(const font_bmp_t *face, const char *text, gfx_text_metrics_t *metrics);
gfx_font_t * gfx_load_bitfont(float size, int style);
void gfx_clear_bitfont(gfx_font_t *font);

#if defined __USE_FREETYPE
typedef void * FT_Face;
void gfx_write_prepare_freetype(gfx_font_t *font);
int gfx_glyph_freetype(gfx_t *gfx, FT_Face face, uint32_t unicode, uint32_t fg, uint32_t bg, int x, int y, const gfx_clip_t *clip);
int gfx_mesure_freetype(FT_Face face, const char *text, gfx_text_metrics_t *metrics);
gfx_font_t * gfx_load_freetype(const char *family, float size, int style);
void gfx_clear_freetype(gfx_font_t *font);
#endif


LIBAPI int gfx_glyph(gfx_t *gfx, gfx_font_t *font, uint32_t unicode, uint32_t fg, uint32_t bg, int x, int y, const gfx_clip_t *clip)
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

LIBAPI int gfx_write(gfx_t *gfx, gfx_font_t *font, const char *text, uint32_t fg, int x, int y, const gfx_clip_t *clip)
{
#if defined __USE_FREETYPE
    if (font->mode == GFX_FT_FREETYPE)
        gfx_write_prepare_freetype(font);
#endif

    wchar_t unicode, len, w;
    while (*text) {
        len = mbtowc(&unicode, text, 6);
        if (len == 0)
            return -1;
        text += len;
        if (font->mode == GFX_FT_BITFONT)
            w = gfx_glyph_bitfont(gfx, font->face, unicode, fg, 0, x, y, clip);
#if defined __USE_FREETYPE
        else if (font->mode == GFX_FT_FREETYPE)
            w = gfx_glyph_freetype(gfx, font->face, unicode, fg, 0, x, y, clip);
#endif
        x += w;
    }
    return 0;
}

LIBAPI gfx_font_t * gfx_font(const char *family, float size, int style)
{
    gfx_font_t* font = NULL;
    if (family == NULL || family[0] == '\0')
        font = gfx_load_bitfont(size, style);
#if defined __USE_FREETYPE
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

LIBAPI int gfx_mesure_text(gfx_font_t *font, const char *text, gfx_text_metrics_t *metrics)
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

