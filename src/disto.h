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
#ifndef _GFX_DISTO_H
#define _GFX_DISTO_H 1

#include "gfx.h"


int gfx_open_device(gfx_t* gfx, const char* path);

int gfx_open_window(gfx_t *gfx);
int gfx_close_window(gfx_t *gfx);

void gfx_map_window(gfx_t *gfx);
void gfx_unmap_window(gfx_t *gfx);



#endif  /* _GFX_DISTO_H */
