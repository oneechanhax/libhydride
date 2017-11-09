/*
 * overlay.c
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#include "overlay.h"
#include "input.h"

#include <GL/glx.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

#include <string.h>

int event_ShapeNotify;
int event_ShapeError;

int xoverlay_init(struct xoverlay_library *library)
{
    memset(library, 0, sizeof(struct xoverlay_library));

    if (init_input() < 0)
    {
        return -1;
    }
    library->display = XOpenDisplay(NULL);
    if (library->display == NULL)
    {
        return -1;
    }

    library->screen = DefaultScreen(library->display);
    library->width = DisplayWidth(library->display, library->screen);
    library->height = DisplayHeight(library->display, library->screen);

    if (!XShapeQueryExtension(library->display, &event_ShapeNotify, &event_ShapeError))
    {
        return -1;
    }

    Window root = DefaultRootWindow(library->display);
    XVisualInfo info;

    XMatchVisualInfo(library->display, library->screen, 32, TrueColor, &info);
    library->colormap = XCreateColormap(library->display, root, info.visual, AllocNone);

    XSetWindowAttributes attr;
    attr.background_pixmap = None;
    attr.background_pixel = 0x0;
    attr.border_pixel = 0;
    attr.win_gravity = NorthWestGravity;
    attr.bit_gravity = ForgetGravity;
    attr.save_under = 1;
    attr.event_mask = 0;
    attr.do_not_propagate_mask = 0;
    attr.override_redirect = 1;
    attr.colormap = library->colormap;

    unsigned long mask = CWBackPixmap | CWBackPixel | CWBorderPixel | CWWinGravity | CWBitGravity | CWSaveUnder | CWEventMask | CWDontPropagate | CWOverrideRedirect | CWColormap;
    library->window = XCreateWindow(library->display, root, 0, 0, library->width, library->height, 0, info.depth, InputOutput, info.visual, mask, &attr);
    XShapeCombineMask(library->display, library->window, ShapeInput, 0, 0, None, ShapeSet);
    XShapeSelectInput(library->display, library->window, ShapeNotifyMask);

    XserverRegion region = XFixesCreateRegion(library->display, NULL, 0);
    XFixesSetWindowShapeRegion(library->display, library->window, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(library->display, region);
    XMapWindow(library->display, library->window);

    library->gc = XCreateGC(library->display, library->window, 0, 0);
    XSetBackground(library->display, library->gc, 0xFFFFFFFF);
    XSetForeground(library->display, library->gc, 0xFFFF0000);

    library->init = 1;
    return 0;
}

void xoverlay_destroy(struct xoverlay_library *library)
{
    XDestroyWindow(library->display, library->window);
    XCloseDisplay(library->display);
    library->init = 0;
}

unsigned xoverlay_rgb(int r, int g, int b)
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
}

void xoverlay_clear_rectangle(struct xoverlay_library *library, int x, int y, int w, int h)
{
    if (!library->init) return;
    if (!library->drawing) return;
}

void xoverlay_clear_screen(struct xoverlay_library *library)
{
    if (!library->init) return;
    if (!library->drawing) return;
}

void xoverlay_draw_string(struct xoverlay_library *library, const char *string, int x, int y, unsigned fg, unsigned bg)
{
    if (!library->init) return;
    if (!library->drawing) return;
}

void xoverlay_draw_line(struct xoverlay_library *library, int x, int y, int w, int h, unsigned fg)
{
    if (!library->init) return;
    if (!library->drawing) return;
    XSetForeground(library->display, library->gc, fg);
    XDrawLine(library->display, library->window, library->gc, x, y, x + w, x + h);
}

void xoverlay_draw_rectangle(struct xoverlay_library *library, int x, int y, int w, int h, unsigned fg)
{
    if (!library->init) return;
    if (!library->drawing) return;
    XSetForeground(library->display, library->gc, fg);
    XFillRectangle(library->display, library->window, library->gc, x, y, w, h);
}

void xoverlay_install_keyboard_callback(struct xoverlay_library *library, xoverlay_callback_keypress callback)
{
    library->cb_keypress = callback;
}

void xoverlay_install_click_callback(struct xoverlay_library *library, xoverlay_callback_click callback)
{
    library->cb_click = callback;
}

void xoverlay_install_scroll_callback(struct xoverlay_library *library, xoverlay_callback_scroll callback)
{
    library->cb_scroll = callback;
}

void xoverlay_install_mouse_callback(struct xoverlay_library *library, xoverlay_callback_mousemove callback)
{
    library->cb_mousemove = callback;
}

/*void xoverlay_install_draw_callback(struct xoverlay_library *library, xoverlay_callback_draw callback)
{
    library->cb_draw = callback;
}*/

void xoverlay_poll_events(struct xoverlay_library *library)
{
    if (!library->init) return;

    struct input_event_parsed ev;

    int accum_x = 0;
    int accum_y = 0;

    while (poll_event(INPUT_DEVICE_KEYBOARD, &ev))
    {
        switch (ev.type)
        {
        case INPUT_EVENT_KEY:
            if (library->cb_keypress)
                library->cb_keypress(ev.evt_key.key, ev.evt_key.state);
            break;
        }
    }
    while (poll_event(INPUT_DEVICE_MOUSE, &ev))
    {
        switch (ev.type)
        {
            case INPUT_EVENT_KEY:
                if (library->cb_click)
                    library->cb_click(ev.evt_key.key, ev.evt_key.state);
                break;
            case INPUT_EVENT_MOVE:
                accum_x += ev.evt_move.x;
                accum_y += ev.evt_move.y;
                break;
            case INPUT_EVENT_SCROLL:
                if (library->cb_scroll)
                    library->cb_scroll(ev.evt_scroll.value);
                break;
        }
    }

    if (accum_x > 0 || accum_y > 0)
    {
        int mx, my;

        Window a, b;
        int c, d;
        unsigned e;

        XQueryPointer(library->display, library->window, &a, &b, &mx, &my, &c, &d, &e);

        if (mx != library->mouse.x || my != library->mouse.y)
        {
            if (library->cb_mousemove)
                library->cb_mousemove(mx - library->mouse.x, my - library->mouse.y, mx, my);

            library->mouse.x = mx;
            library->mouse.y = my;
        }

    }

    /*char keymap_new[32];
    XQueryKeymap(library->display, keymap_new);
    for (int i = 0; i < 32; ++i)
    {
        if (keymap_new[i] != library->keyboard.keymap[i])
        {
            for (int j = 0; j < 8; ++j)
            {
                if ((keymap_new[i] & (1 << j)) != (library->keyboard.keymap[i] & (1 << j)))
                {
                    if (library->cb_keypress)
                        library->cb_keypress(i * 8 + j, !!(keymap_new[i] & (1 << j)));
                }
            }
            library->keyboard.keymap[i] = keymap_new[i];
        }
    }

    int mx, my;

    Window a, b;
    int c, d;
    unsigned e;

    XQueryPointer(library->display, library->window, &a, &b, &mx, &my, &c, &d, &e);
    if (library->cb_mousemove && (library->mouse.x != mx || library->mouse.y != my))
    {
        library->cb_mousemove(mx, my);
    }

    library->mouse.x = mx;
    library->mouse.y = my;*/

    /*XEvent xevt;
    if (XCheckWindowEvent(library->display, library->window, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask, &xevt))
    {
        printf("event %d\n", xevt.type);
        switch (xevt.type)
        {
        case KeyPress:
        case KeyRelease:
            library->keyboard[xevt.xkey.keycode] = (xevt.xkey.type == KeyPress);
            if (library->cb_click)
                library->cb_click(xevt.xkey.keycode, xevt.xkey.type == KeyPress);
            break;
        case ButtonPress:
        case ButtonRelease:
            library->mouse.buttons[xevt.xbutton.button - 1] = (xevt.xbutton.type == ButtonPress);
            if (library->cb_click)
                library->cb_click(xevt.xbutton.button - 1, xevt.xbutton.type == ButtonPress);
            break;
        case MotionNotify:
            library->mouse.x = xevt.xmotion.x;
            library->mouse.y = xevt.xmotion.y;
            if (library->cb_click)
                library->cb_click(xevt.xbutton.button - 1, xevt.xbutton.type == ButtonPress);
            break;
        case Expose:
            if (library->cb_draw)
                library->cb_draw();
        }
    }*/
}

void xoverlay_draw_begin(struct xoverlay_library *library)
{
    if (!library->init) return;

    library->drawing = 1;
}

void xoverlay_draw_end(struct xoverlay_library *library)
{
    if (!library->init) return;

    XFlush(library->display);
    library->drawing = 0;
}
