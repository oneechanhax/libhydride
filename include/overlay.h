/*
 * overlay.hpp
 *
 *  Created on: Nov 8, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

typedef void(*xoverlay_callback_keypress)(unsigned int keycode, int action);
typedef void(*xoverlay_callback_click)(unsigned int buttoncode, int action);
typedef void(*xoverlay_callback_scroll)(int value);
typedef void(*xoverlay_callback_mousemove)(int dx, int dy, int x, int y);
/* typedef void(*xoverlay_callback_draw)(); */

struct xoverlay_library
{
    xoverlay_callback_keypress cb_keypress;
    xoverlay_callback_click cb_click;
    xoverlay_callback_scroll cb_scroll;
    xoverlay_callback_mousemove cb_mousemove;
    /* xoverlay_callback_draw cb_draw; */

    Display *display;
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

    /*struct
    {
        int x;
        int y;
        char buttons[5];
    } mouse;

    struct
    {
        char keymap[32];
    } keyboard;*/

    struct xoverlay_glx_state glx;

    char init;
    char drawing;
};

int xoverlay_init(struct xoverlay_library *library);
void xoverlay_destroy(struct xoverlay_library *library);

void xoverlay_install_keyboard_callback(struct xoverlay_library *library, xoverlay_callback_keypress callback);
void xoverlay_install_click_callback(struct xoverlay_library *library, xoverlay_callback_click callback);
void xoverlay_install_scroll_callback(struct xoverlay_library *library, xoverlay_callback_scroll callback);
void xoverlay_install_mouse_callback(struct xoverlay_library *library, xoverlay_callback_mousemove callback);
/* void xoverlay_install_draw_callback(struct xoverlay_library *library, xoverlay_callback_draw callback); */

int xoverlay_is_button_down(struct xoverlay_library *library, KeyCode code);

unsigned xoverlay_rgb(int r, int g, int b);
unsigned xoverlay_rgba(int r, int g, int b, int a);

void xoverlay_clear_rectangle(struct xoverlay_library *library, int x, int y, int w, int h);
void xoverlay_clear_screen(struct xoverlay_library *library);

void xoverlay_draw_string(struct xoverlay_library *library, const char *string, int x, int y, unsigned fg, unsigned bg);
void xoverlay_draw_line(struct xoverlay_library *library, int x, int y, int w, int h, unsigned fg);
void xoverlay_draw_rectangle(struct xoverlay_library *library, int x, int y, int w, int h, unsigned fg);

void xoverlay_poll_events(struct xoverlay_library *library);
void xoverlay_draw_begin(struct xoverlay_library *library);
void xoverlay_draw_end(struct xoverlay_library *library);
