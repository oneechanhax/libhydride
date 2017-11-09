/*
 * drawglx.c
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#include "drawglx.h"
#include "overlay.h"

#include <GL/gl.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB           0x2092
typedef GLXContext (*glXCreateContextAttribsARBfn)(Display *, GLXFBConfig, GLXContext, Bool, const int *);

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
int glx_is_extension_supported(const char *list, const char *extension)
{
    const char *start;
    const char *where, *terminator;

    where = strchr(extension, ' ');
    if (where || *extension == '\0')
        return 0;

    start = list;
    while (1)
    {
        where = strstr(start, extension);

        if (!where)
        break;

        terminator = where + strlen(extension);

        if ( where == start || *(where - 1) == ' ' )
            if ( *terminator == ' ' || *terminator == '\0' )
                return 1;

        start = terminator;
    }

    return 0;
}

int xoverlay_glx_init(struct xoverlay_library *library)
{
    glXQueryVersion(library->display, &library->glx.version_major, &library->glx.version_minor);
    if (library->glx.version_major < 2)
    {
        return -1;
    }
    printf("GL Version: %s\n", glGetString(GL_VERSION));
    return 0;
}

int xoverlay_glx_create_window(struct xoverlay_library *library)
{
    GLint attribs[] = {
            GLX_X_RENDERABLE, GL_TRUE,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
            GLX_DEPTH_SIZE, 24,
            GLX_STENCIL_SIZE, 8,
            GLX_RED_SIZE, 8,
            GLX_GREEN_SIZE, 8,
            GLX_BLUE_SIZE, 8,
            GLX_ALPHA_SIZE, 8,
            GLX_DOUBLEBUFFER, GL_TRUE,
            None
    };

    int fbc_count;
    GLXFBConfig *fbc = glXChooseFBConfig(library->display, library->screen, attribs, &fbc_count);
    if (fbc == NULL)
    {
        return -1;
    }
    int fbc_best = -1;
    int fbc_best_samples = -1;
    for (int i = 0; i < fbc_count; ++i)
    {
        XVisualInfo *info = glXGetVisualFromFBConfig(library->display, fbc[i]);
        if (info->depth != 32)
            continue;
        int samples;
        glXGetFBConfigAttrib(library->display, fbc[i], GLX_SAMPLES, &samples);
        if (fbc_best < 0 || samples > fbc_best_samples)
        {
            fbc_best = i;
            fbc_best_samples = samples;
        }
        XFree(info);
    }
    GLXFBConfig fbconfig = fbc[fbc_best];
    XFree(fbc);

    XVisualInfo *info = glXGetVisualFromFBConfig(library->display, fbconfig);
    if (info == NULL)
    {
        return -1;
    }
    Window root = DefaultRootWindow(library->display);
    library->colormap = XCreateColormap(library->display, root, info->visual, AllocNone);
    XSetWindowAttributes attr;
    attr.background_pixel = 0x0;
    attr.border_pixel = 0;
    attr.save_under = 1;
    attr.override_redirect = 1;
    attr.colormap = library->colormap;

    unsigned long mask = CWBackPixel | CWBorderPixel | CWSaveUnder | CWOverrideRedirect | CWColormap;
    printf("depth %d\n", info->depth);
    library->window = XCreateWindow(library->display, root, 0, 0, library->width, library->height, 0, info->depth, InputOutput, info->visual, mask, &attr);
    if (library->window == NULL)
    {
        return -1;
    }
    XFree(info);
    XStoreName(library->display, library->window, "OverlayWindow");
    XMapWindow(library->display, library->window);

    const char *extensions = glXQueryExtensionsString(library->display, library->screen);
    glXCreateContextAttribsARBfn glXCreateContextAttribsARB = (glXCreateContextAttribsARBfn)
            glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");

    if (!glx_is_extension_supported(extensions, "GLX_ARB_create_context"))
    {
        printf("Falling back to glXCreateNewContext\n");
        library->glx.context = glXCreateNewContext(library->display, fbconfig, GLX_RGBA_TYPE, NULL, GL_TRUE);
    }
    else
    {
        int ctx_attribs[] = {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                None
        };
        library->glx.context = glXCreateContextAttribsARB(library->display, fbconfig, NULL, GL_TRUE, ctx_attribs);
        XSync(library->display, GL_FALSE);
    }
    if (library->glx.context == NULL)
    {
        return -1;
    }
    if (!glXIsDirect(library->display, library->glx.context))
    {
        printf("Context is indirect\n");
    }
    else
    {
        printf("Context is direct\n");
    }
    glXMakeCurrent(library->display, library->window, library->glx.context);
    glClearColor(0.0f, 0.0f, 0.3f, 0.1f);
    glClear(GL_COLOR_BUFFER_BIT);
    glXSwapBuffers(library->display, library->window);

    return 0;
}

int xoverlay_glx_destroy(struct xoverlay_library *library)
{
    return 0;
}
