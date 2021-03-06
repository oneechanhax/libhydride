
/*
 * Libxoverlay: A transparent drawable GL layer for your desktop! 
 * Copyright (C) 2022 Rebekah Rowe
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <X11/XKBlib.h>
#include <X11/Xlib.h>

#include "hydride.h"

typedef struct hydride_library xoverlay_library_t; // only api break in rename

extern struct hydride_library xoverlay_library;

inline int xoverlay_init() { return hydride_init(); }

inline void xoverlay_destroy() { hydride_destroy(); }

inline void xoverlay_show() { hydride_show(); }

inline void xoverlay_hide() { hydride_hide(); }

inline void xoverlay_draw_begin() { hydride_draw_begin(); }

inline void xoverlay_draw_end() { hydride_draw_end(); }

#ifdef __cplusplus
}
#endif
