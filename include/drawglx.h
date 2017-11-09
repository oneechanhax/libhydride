/*
 * drawglx.h
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include <GL/glew.h>
#include <GL/glx.h>

struct xoverlay_library;
struct xoverlay_glx_state
{
    int version_major;
    int version_minor;
    GLXContext context;
};

int xoverlay_glx_init(struct xoverlay_library *library);
int xoverlay_glx_create_window(struct xoverlay_library *library);
int xoverlay_glx_destroy(struct xoverlay_library *library);
