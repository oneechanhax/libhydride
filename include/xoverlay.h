/*
 * overlay.hpp
 *
 *  Created on: Nov 8, 2017
 *      Author: nullifiedcat
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <X11/XKBlib.h>
#include <X11/Xlib.h>

struct xoverlay_library {
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

extern struct xoverlay_library xoverlay_library;

int xoverlay_init();

void xoverlay_destroy();

void xoverlay_show();

void xoverlay_hide();

void xoverlay_draw_begin();

void xoverlay_draw_end();

#ifdef __cplusplus
}
#endif
