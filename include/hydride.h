
/*
 * Libhydride: A transparent drawable GL layer for your desktop! 
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

struct hydride_library {
    Display* display;
    Window window;
    Colormap colormap;
    GC gc;
    XGCValues gcvalues;
    XFontStruct font;
    int screen;

    int width;
    int height;

    struct
    {
        int x;
        int y;
    } mouse;

    char init;
    char drawing;
    char mapped;
};

extern struct hydride_library hydride_library;

int hydride_init();

void hydride_destroy();

void hydride_show();

void hydride_hide();

void hydride_draw_begin();

void hydride_draw_end();

#ifdef __cplusplus
}
#endif
