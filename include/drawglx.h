/*
 * drawglx.h
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

struct xoverlay_glx_state
{
    int version_major;
    int version_minor;
    GLXContext context;
} glx_state;

int xoverlay_glx_init();
int xoverlay_glx_create_window();
int xoverlay_glx_destroy();
