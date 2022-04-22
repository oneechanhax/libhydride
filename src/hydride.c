
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

#include "drawglx.h"

#include <GL/glx.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>

#include <hydride.h>
#include <stdio.h>
#include <string.h>

int event_ShapeNotify;
int event_ShapeError;
struct hydride_library hydride_library;

int preinit_done = 0;

int hydride_init() {
    memset(&hydride_library, 0, sizeof(struct hydride_library));

    hydride_library.display = XOpenDisplay(NULL);
    if (hydride_library.display == NULL) {
        return -2;
    }

    hydride_library.screen = DefaultScreen(hydride_library.display);
    hydride_library.width = DisplayWidth(hydride_library.display, hydride_library.screen);
    hydride_library.height = DisplayHeight(hydride_library.display, hydride_library.screen);

    if (!XShapeQueryExtension(hydride_library.display, &event_ShapeNotify,
            &event_ShapeError)) {
        return -3;
    }

    if (hydride_glx_init() < 0) {
        return -4;
    }
    if (hydride_glx_create_window() < 0) {
        return -5;
    }

    hydride_library.init = 1;

    return 0;
}

void hydride_destroy() {
    XDestroyWindow(hydride_library.display, hydride_library.window);
    XCloseDisplay(hydride_library.display);
    hydride_glx_destroy();
    hydride_library.init = 0;
}

void hydride_show() {
    if (hydride_library.mapped == 1)
        return;

    XMapWindow(hydride_library.display, hydride_library.window);
    hydride_library.mapped = 1;
}

void hydride_hide() {
    if (hydride_library.mapped == 0)
        return;

    XUnmapWindow(hydride_library.display, hydride_library.window);
    hydride_library.mapped = 0;
}

void hydride_draw_begin() {
    if (!hydride_library.init)
        return;
    hydride_library.drawing = 1;
    glXMakeCurrent(hydride_library.display, hydride_library.window,
        glx_state.context);
    glClear(GL_COLOR_BUFFER_BIT);
}

void hydride_draw_end() {
    if (!hydride_library.init)
        return;
    glXSwapBuffers(hydride_library.display, hydride_library.window);
    hydride_library.drawing = 0;
}
