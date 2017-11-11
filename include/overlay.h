/*
 * overlay.hpp
 *
 *  Created on: Nov 8, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "fontapi.h"
#include "textureapi.h"

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

typedef void(*xoverlay_callback_keypress)(unsigned int keycode, int action);
typedef void(*xoverlay_callback_click)(unsigned int buttoncode, int action);
typedef void(*xoverlay_callback_scroll)(int value);
typedef void(*xoverlay_callback_mousemove)(int dx, int dy, int x, int y);

struct xoverlay_library
{
    xoverlay_callback_keypress cb_keypress;
    xoverlay_callback_click cb_click;
    xoverlay_callback_scroll cb_scroll;
    xoverlay_callback_mousemove cb_mousemove;

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

    char init;
    char drawing;
};

struct xoverlay_library xoverlay_library;

typedef struct xoverlay_vec4_t
{
    union
    {
        float data[4];
        struct
        {
            float r;
            float g;
            float b;
            float a;
        };
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
    };
} xoverlay_vec4_t, xoverlay_rgba_t;

typedef struct xoverlay_vec2_t
{
    float x;
    float y;
} xoverlay_vec2_t;

int  xoverlay_init();
void xoverlay_destroy();

void xoverlay_install_keyboard_callback(xoverlay_callback_keypress callback);
void xoverlay_install_click_callback(xoverlay_callback_click callback);
void xoverlay_install_scroll_callback(xoverlay_callback_scroll callback);
void xoverlay_install_mouse_callback(xoverlay_callback_mousemove callback);

xoverlay_rgba_t xoverlay_rgba(int r, int g, int b, int a);

void
xoverlay_draw_line(xoverlay_vec2_t xy, xoverlay_vec2_t delta, xoverlay_rgba_t color, float thickness);

void
xoverlay_draw_rect(xoverlay_vec2_t xy, xoverlay_vec2_t hw, xoverlay_rgba_t color);

void
xoverlay_draw_rect_outline(xoverlay_vec2_t xy, xoverlay_vec2_t hw, xoverlay_rgba_t color, float thickness);

void
xoverlay_draw_rect_textured(xoverlay_vec2_t xy, xoverlay_vec2_t hw, xoverlay_rgba_t color, xoverlay_texture_handle texture, xoverlay_vec2_t t_xy, xoverlay_vec2_t t_hw);

void
xoverlay_draw_string(xoverlay_vec2_t xy, const char *string, xoverlay_font_handle_t font, xoverlay_vec4_t color, int *out_x, int *out_y);

void
xoverlay_draw_string_with_outline(xoverlay_vec2_t xy, const char *string, xoverlay_font_handle_t font, xoverlay_vec4_t color, xoverlay_vec4_t outline_color, float outline_width, int adjust_outline_alpha, int *out_x, int *out_y);

void xoverlay_poll_events();
void xoverlay_draw_begin();
void xoverlay_draw_end();
