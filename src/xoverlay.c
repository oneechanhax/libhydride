
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

#include "drawglx.h"

#include <GL/glx.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>

#include <stdio.h>
#include <string.h>
#include <xoverlay.h>

int event_ShapeNotify;
int event_ShapeError;
struct xoverlay_library xoverlay_library;

int preinit_done = 0;

int xoverlay_init() {
    memset(&xoverlay_library, 0, sizeof(struct xoverlay_library));

    xoverlay_library.display = XOpenDisplay(NULL);
    if (xoverlay_library.display == NULL) {
        return -2;
    }

    xoverlay_library.screen = DefaultScreen(xoverlay_library.display);
    xoverlay_library.width = DisplayWidth(xoverlay_library.display, xoverlay_library.screen);
    xoverlay_library.height = DisplayHeight(xoverlay_library.display, xoverlay_library.screen);

    if (!XShapeQueryExtension(xoverlay_library.display, &event_ShapeNotify,
            &event_ShapeError)) {
        return -3;
    }

    if (xoverlay_glx_init() < 0) {
        return -4;
    }
    if (xoverlay_glx_create_window() < 0) {
        return -5;
    }

    xoverlay_library.init = 1;

    return 0;
}

void xoverlay_destroy() {
    XDestroyWindow(xoverlay_library.display, xoverlay_library.window);
    XCloseDisplay(xoverlay_library.display);
    xoverlay_glx_destroy();
    xoverlay_library.init = 0;
}

void xoverlay_show() {
    if (xoverlay_library.mapped == 1)
        return;

    XMapWindow(xoverlay_library.display, xoverlay_library.window);
    xoverlay_library.mapped = 1;
}

void xoverlay_hide() {
    if (xoverlay_library.mapped == 0)
        return;

    XUnmapWindow(xoverlay_library.display, xoverlay_library.window);
    xoverlay_library.mapped = 0;
}

void xoverlay_draw_begin() {
    if (!xoverlay_library.init)
        return;
    xoverlay_library.drawing = 1;
    glXMakeCurrent(xoverlay_library.display, xoverlay_library.window,
        glx_state.context);
    glClear(GL_COLOR_BUFFER_BIT);
}

void xoverlay_draw_end() {
    if (!xoverlay_library.init)
        return;
    glXSwapBuffers(xoverlay_library.display, xoverlay_library.window);
    xoverlay_library.drawing = 0;
}
