/*
 * overlay.c
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#include "overlay.h"
#include "input.h"
#include "drawglx_internal.h"
#include "log.h"

#include <GL/glx.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

#include <string.h>
#include <stdio.h>

int event_ShapeNotify;
int event_ShapeError;
struct xoverlay_library xoverlay_library;

int preinit_done = 0;

void xoverlay_preinit()
{
    preinit_done = 1;
    if (init_input() < 0)
    {
        perror("XOVERLAY: init_input failed");
        return;
    }
    fontapi_init();
    textureapi_init();
}

int xoverlay_init()
{
    memset(&xoverlay_library, 0, sizeof(struct xoverlay_library));

    xoverlay_library.display = XOpenDisplay(NULL);
    if (xoverlay_library.display == NULL)
    {
        log_write("XOVERLAY: XOpenDisplay failed\n");
        return -2;
    }

    if (preinit_done == 0)
        xoverlay_preinit();
    else
        log_write("Preinit already done\n");

    xoverlay_library.screen = DefaultScreen(xoverlay_library.display);
    xoverlay_library.width = DisplayWidth(xoverlay_library.display, xoverlay_library.screen);
    xoverlay_library.height = DisplayHeight(xoverlay_library.display, xoverlay_library.screen);

    if (!XShapeQueryExtension(xoverlay_library.display, &event_ShapeNotify, &event_ShapeError))
    {
        log_write("XOVERLAY: no shape extension\n");
        return -3;
    }

    if (xoverlay_glx_init() < 0)
    {
        log_write("XOVERLAY: xoverlay_glx_init failed\n");
        return -4;
    }
    if (xoverlay_glx_create_window() < 0)
    {
        log_write("XOVERLAY: xoverlay_glx_create_window failed\n");
        return -5;
    }

    xoverlay_library.init = 1;

    return 0;
}

void xoverlay_destroy()
{
    XDestroyWindow(xoverlay_library.display, xoverlay_library.window);
    XCloseDisplay(xoverlay_library.display);
    xoverlay_glx_destroy();
    fontapi_destroy();
    textureapi_destroy();
    xoverlay_library.init = 0;
}

/*unsigned xoverlay_rgb(int r, int g, int b)
{
    return xoverlay_rgba(r, g, b, 255);
}

unsigned xoverlay_rgba(int r, int g, int b, int a)
{
    unsigned result = 0x00000000;
    if (a == 0)
    {
        return 0;
    }
    r = r * a / 255;
    g = g * a / 255;
    b = b * a / 255;
    result = (b & 0xFF) | (g & 0xFF) << 8 | (r & 0xFF) << 16 | (a & 0xFF) << 24;
    return result;
}*/

xoverlay_rgba_t xoverlay_rgba(int r, int g, int b, int a)
{
    xoverlay_rgba_t result;
    result.r = (float)r / 255.0f;
    result.g = (float)g / 255.0f;
    result.b = (float)b / 255.0f;
    result.a = (float)a / 255.0f;
    return result;
}

void xoverlay_install_keyboard_callback(xoverlay_callback_keypress callback)
{
    xoverlay_library.cb_keypress = callback;
}

void xoverlay_install_click_callback(xoverlay_callback_click callback)
{
    xoverlay_library.cb_click = callback;
}

void xoverlay_install_scroll_callback(xoverlay_callback_scroll callback)
{
    xoverlay_library.cb_scroll = callback;
}

void xoverlay_install_mouse_callback(xoverlay_callback_mousemove callback)
{
    xoverlay_library.cb_mousemove = callback;
}

/*void xoverlay_install_draw_callback(struct xoverlay_library *library, xoverlay_callback_draw callback)
{
    xoverlay_library.cb_draw = callback;
}*/

void
xoverlay_show()
{
    if (xoverlay_library.mapped == 1)
        return;

    XMapWindow(xoverlay_library.display, xoverlay_library.window);
    xoverlay_library.mapped = 1;
}

void
xoverlay_hide()
{
    if (xoverlay_library.mapped == 0)
        return;

    XUnmapWindow(xoverlay_library.display, xoverlay_library.window);
    xoverlay_library.mapped = 0;
}

void xoverlay_poll_events()
{
    if (!xoverlay_library.init) return;

    struct input_event_parsed ev;

    int accum_x = 0;
    int accum_y = 0;

    while (poll_event(INPUT_DEVICE_KEYBOARD, &ev))
    {
        switch (ev.type)
        {
        case INPUT_EVENT_KEY:
            if (xoverlay_library.cb_keypress)
                xoverlay_library.cb_keypress(ev.evt_key.key, ev.evt_key.state);
            break;
        }
    }
    while (poll_event(INPUT_DEVICE_MOUSE, &ev))
    {
        switch (ev.type)
        {
            case INPUT_EVENT_KEY:
                if (xoverlay_library.cb_click)
                    xoverlay_library.cb_click(ev.evt_key.key, ev.evt_key.state);
                break;
            case INPUT_EVENT_MOVE:
                accum_x += ev.evt_move.x;
                accum_y += ev.evt_move.y;
                break;
            case INPUT_EVENT_SCROLL:
                if (xoverlay_library.cb_scroll)
                    xoverlay_library.cb_scroll(ev.evt_scroll.value);
                break;
        }
    }

    if (accum_x > 0 || accum_y > 0)
    {
        int mx, my;

        Window a, b;
        int c, d;
        unsigned e;

        XQueryPointer(xoverlay_library.display, xoverlay_library.window, &a, &b, &mx, &my, &c, &d, &e);

        if (mx != xoverlay_library.mouse.x || my != xoverlay_library.mouse.y)
        {
            if (xoverlay_library.cb_mousemove)
                xoverlay_library.cb_mousemove(mx - xoverlay_library.mouse.x, my - xoverlay_library.mouse.y, mx, my);

            xoverlay_library.mouse.x = mx;
            xoverlay_library.mouse.y = my;
        }

    }

    /*char keymap_new[32];
    XQueryKeymap(xoverlay_library.display, keymap_new);
    for (int i = 0; i < 32; ++i)
    {
        if (keymap_new[i] != xoverlay_library.keyboard.keymap[i])
        {
            for (int j = 0; j < 8; ++j)
            {
                if ((keymap_new[i] & (1 << j)) != (xoverlay_library.keyboard.keymap[i] & (1 << j)))
                {
                    if (xoverlay_library.cb_keypress)
                        xoverlay_library.cb_keypress(i * 8 + j, !!(keymap_new[i] & (1 << j)));
                }
            }
            xoverlay_library.keyboard.keymap[i] = keymap_new[i];
        }
    }

    int mx, my;

    Window a, b;
    int c, d;
    unsigned e;

    XQueryPointer(xoverlay_library.display, xoverlay_library.window, &a, &b, &mx, &my, &c, &d, &e);
    if (xoverlay_library.cb_mousemove && (xoverlay_library.mouse.x != mx || xoverlay_library.mouse.y != my))
    {
        xoverlay_library.cb_mousemove(mx, my);
    }

    xoverlay_library.mouse.x = mx;
    xoverlay_library.mouse.y = my;*/

    /*XEvent xevt;
    if (XCheckWindowEvent(xoverlay_library.display, xoverlay_library.window, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask, &xevt))
    {
        log_write("event %d\n", xevt.type);
        switch (xevt.type)
        {
        case KeyPress:
        case KeyRelease:
            xoverlay_library.keyboard[xevt.xkey.keycode] = (xevt.xkey.type == KeyPress);
            if (xoverlay_library.cb_click)
                xoverlay_library.cb_click(xevt.xkey.keycode, xevt.xkey.type == KeyPress);
            break;
        case ButtonPress:
        case ButtonRelease:
            xoverlay_library.mouse.buttons[xevt.xbutton.button - 1] = (xevt.xbutton.type == ButtonPress);
            if (xoverlay_library.cb_click)
                xoverlay_library.cb_click(xevt.xbutton.button - 1, xevt.xbutton.type == ButtonPress);
            break;
        case MotionNotify:
            xoverlay_library.mouse.x = xevt.xmotion.x;
            xoverlay_library.mouse.y = xevt.xmotion.y;
            if (xoverlay_library.cb_click)
                xoverlay_library.cb_click(xevt.xbutton.button - 1, xevt.xbutton.type == ButtonPress);
            break;
        case Expose:
            if (xoverlay_library.cb_draw)
                xoverlay_library.cb_draw();
        }
    }*/
}

void xoverlay_draw_begin()
{
    if (!xoverlay_library.init) return;
    ds_pre_render();
    xoverlay_library.drawing = 1;
}

void xoverlay_draw_end()
{
    if (!xoverlay_library.init) return;
    ds_post_render();
    xoverlay_library.drawing = 0;
}
