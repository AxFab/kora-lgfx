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
#include <gfx.h>
#include <stdlib.h>
#include <dirent.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftsnames.h>
#include "../mcrs.h"

FT_Library library;
bool __freetype_initialized = false;

void gfx_write_prepare_freetype(gfx_font_t* font)
{
    int dpi = 96;
    float sizeInPx = (font->size * dpi) / 96;
    FT_Error fterror = FT_Set_Char_Size((FT_Face)font->face, 0, sizeInPx * 64.0f/* 1/64th pts*/, dpi, dpi);
}

int gfx_glyph_freetype(gfx_t* gfx, FT_Face face, uint32_t unicode, uint32_t fg, uint32_t bg, int x, int y, const gfx_clip_t* clip)
{
    FT_GlyphSlot slot = face->glyph;
    FT_UInt glyph_index = FT_Get_Char_Index(face, unicode);
    FT_Error fterror = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    fterror = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

    // Draw
    fg = fg & 0xffffff;
    for (int i = 0; i < slot->bitmap.rows; ++i) {
        int py = y - slot->bitmap_top + i;
        for (int j = 0; j < slot->bitmap.width; ++j) {
            if (py < 0 || py >= gfx->height)
                continue;
            int px = x + slot->bitmap_left + j;
            if (px < 0 || px > gfx->width)
                continue;
            int a = slot->bitmap.buffer[i * slot->bitmap.pitch + j];
            // a = MIN(a * 2, 0xff);
            if (a > 0)
                gfx->pixels4[py * gfx->width + px] = gfx_upper_alpha_blend(gfx->pixels4[py * gfx->width + px], fg | (a << 24));
        }
    }

    return slot->advance.x >> 6;
}

int gfx_mesure_freetype(FT_Face face, const char* text, gfx_text_metrics_t* metrics)
{
    int width = 0, height = 0, yBearing = 0;

    FT_GlyphSlot slot = face->glyph;
    while (*text) {
        wchar_t unicode;
        int len = mbtowc(&unicode, text, 6);
        text += len;

        FT_UInt glyph_index = FT_Get_Char_Index(face, unicode);
        FT_Error fterror = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

        yBearing = MAX(yBearing, slot->metrics.horiBearingY);
        width += slot->advance.x;
        height = MAX(height, slot->metrics.vertAdvance);
    }

    metrics->width = width >> 6;
    metrics->baseline = yBearing >> 6;
    metrics->height = height >> 6;
    return 0;
}

gfx_font_t* gfx_load_freetype(const char* family, float size, int style)
{
    static const char* system_dir = "C:/Windows/Fonts";
    FT_Error fterror;
    FT_Face face;
    char buf[BUFSIZ];
    if (!__freetype_initialized) {
        fterror = FT_Init_FreeType(&library);
        __freetype_initialized = true;

        struct dirent de;
        void* p;
        DIR* dir = opendir(system_dir);
        for (;;) {
            readdir_r(dir, &de, &p);
            if (p == NULL)
                break;

            FT_Face face;
            snprintf(buf, BUFSIZ, "%s/%s", system_dir, de.d_name);
            fterror = FT_New_Face(library, buf, 0, &face);
            if (fterror != 0)
                continue;
            // printf("- %s : %s (%s)\n", de.d_name, face->family_name, face->style_name);
            int num = 0; // face->num_faces;
            FT_Done_Face(face);
            for (int i = 1; i < num; ++i) {
                fterror = FT_New_Face(library, buf, i, &face);
                if (fterror != 0)
                    continue;
                // printf("- %s : %s (%s)\n", de.d_name, face->family_name, face->style_name);
                FT_Done_Face(face);
            }
        }
        closedir(dir);
    }
    snprintf(buf, BUFSIZ, "%s/%s", system_dir, "consola.ttf");
    fterror = FT_New_Face(library, buf, 0, &face);
    if (fterror != 0)
        return NULL;
    
    gfx_font_t* font = malloc(sizeof(gfx_font_t));
    font->mode = GFX_FT_FREETYPE;
    font->size = size;
    font->style = 0;
    font->family = NULL; // strdup(family);
    font->face = (void*)face;
    return font;
}

void gfx_clear_freetype(gfx_font_t* font)
{
    FT_Done_Face((FT_Face)font->face);
}

