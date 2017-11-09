/*
 * drawglx.c
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#include "drawglx.h"

int glx_version_major;
int glx_version_minor;

int xoverlay_glx_init(struct xoverlay_library *library)
{
    glXQueryVersion(library->display, &library->glx.version_major, &library->glx.version_minor);
    return 0;
}

int xoverlay_glx_destroy(struct xoverlay_library *library)
{
    return 0;
}
