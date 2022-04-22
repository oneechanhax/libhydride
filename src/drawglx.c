
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

#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>
#include <hydride.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
typedef GLXContext (*glXCreateContextAttribsARBfn)(Display*, GLXFBConfig,
    GLXContext, Bool,
    const int*);

hydride_glx_state glx_state;

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
int glx_is_extension_supported(const char* list, const char* extension) {
    const char* start;
    const char *where, *terminator;

    where = strchr(extension, ' ');
    if (where || *extension == '\0')
        return 0;

    start = list;
    while (1) {
        where = strstr(start, extension);

        if (!where)
            break;

        terminator = where + strlen(extension);

        if (where == start || *(where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return 1;

        start = terminator;
    }

    return 0;
}

int hydride_glx_init() {
    glXQueryVersion(hydride_library.display, &glx_state.version_major,
        &glx_state.version_minor);
    return 0;
}

int hydride_glx_create_window() {
    GLint attribs[] = { GLX_X_RENDERABLE,
        GL_TRUE,
        GLX_DRAWABLE_TYPE,
        GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,
        GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE,
        GLX_TRUE_COLOR,
        GLX_DEPTH_SIZE,
        24,
        GLX_STENCIL_SIZE,
        8,
        GLX_RED_SIZE,
        8,
        GLX_GREEN_SIZE,
        8,
        GLX_BLUE_SIZE,
        8,
        GLX_ALPHA_SIZE,
        8,
        GLX_DOUBLEBUFFER,
        GL_TRUE,
        None };

    int fbc_count;
    GLXFBConfig* fbc = glXChooseFBConfig(
        hydride_library.display, hydride_library.screen, attribs, &fbc_count);
    if (fbc == NULL) {
        return -1;
    }
    int fbc_best = -1;
    int fbc_best_samples = -1;
    for (int i = 0; i < fbc_count; ++i) {
        XVisualInfo* info = glXGetVisualFromFBConfig(hydride_library.display, fbc[i]);
        if (info->depth != 32)
            continue;
        int samples;
        glXGetFBConfigAttrib(hydride_library.display, fbc[i], GLX_SAMPLES,
            &samples);
        if (fbc_best < 0 || samples > fbc_best_samples) {
            fbc_best = i;
            fbc_best_samples = samples;
        }
        XFree(info);
    }
    if (fbc_best == -1) {
        return -1;
    }
    GLXFBConfig fbconfig = fbc[fbc_best];
    XFree(fbc);

    XVisualInfo* info = glXGetVisualFromFBConfig(hydride_library.display, fbconfig);
    if (info == NULL) {
        return -1;
    }
    Window root = DefaultRootWindow(hydride_library.display);
    hydride_library.colormap = XCreateColormap(hydride_library.display, root,
        info->visual, AllocNone);
    XSetWindowAttributes attr;
    attr.background_pixel = 0x0;
    attr.border_pixel = 0;
    attr.save_under = 1;
    attr.override_redirect = 1;
    attr.colormap = hydride_library.colormap;
    attr.event_mask = 0x0;
    attr.do_not_propagate_mask = (KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask);

    unsigned long mask = CWBackPixel | CWBorderPixel | CWSaveUnder | CWOverrideRedirect | CWColormap | CWEventMask | CWDontPropagate;
    hydride_library.window = XCreateWindow(hydride_library.display, root, 0, 0,
        hydride_library.width, hydride_library.height, 0,
        info->depth, InputOutput, info->visual, mask, &attr);
    if (hydride_library.window == 0) {
        return -1;
    }

    XShapeCombineMask(hydride_library.display, hydride_library.window,
        ShapeInput, 0, 0, None, ShapeSet);
    XShapeSelectInput(hydride_library.display, hydride_library.window,
        ShapeNotifyMask);

    XserverRegion region = XFixesCreateRegion(hydride_library.display, NULL, 0);
    XFixesSetWindowShapeRegion(hydride_library.display,
        hydride_library.window, ShapeInput, 0, 0,
        region);
    XFixesDestroyRegion(hydride_library.display, region);

    XFree(info);
    XStoreName(hydride_library.display, hydride_library.window,
        "OverlayWindow");

    hydride_show();

    const char* extensions = glXQueryExtensionsString(hydride_library.display,
        hydride_library.screen);
    glXCreateContextAttribsARBfn glXCreateContextAttribsARB = (glXCreateContextAttribsARBfn)glXGetProcAddressARB(
        (const GLubyte*)"glXCreateContextAttribsARB");

    if (!glx_is_extension_supported(extensions, "GLX_ARB_create_context")) {
        glx_state.context = glXCreateNewContext(
            hydride_library.display, fbconfig, GLX_RGBA_TYPE, NULL, GL_TRUE);
    } else {
        int ctx_attribs[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0, None };
        glx_state.context = glXCreateContextAttribsARB(
            hydride_library.display, fbconfig, NULL, GL_TRUE, ctx_attribs);
        XSync(hydride_library.display, GL_FALSE);
    }
    if (glx_state.context == NULL) {
        return -1;
    }
    if (!glXIsDirect(hydride_library.display, glx_state.context))
        ;
    glXMakeCurrent(hydride_library.display, hydride_library.window,
        glx_state.context);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return -1;
    }
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddressARB(
        (const GLubyte*)"glXSwapIntervalEXT");
    if (glXSwapIntervalEXT)
        glXSwapIntervalEXT(hydride_library.display, hydride_library.window,
            0);
    glXSwapBuffers(hydride_library.display, hydride_library.window);

    return 0;
}

int hydride_glx_destroy() {
    return 0;
}
