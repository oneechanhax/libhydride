/*
 * drawglx.h
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "overlay.h"
#include <GL/glx.h>

struct xoverlay_glx_state
{
    int version_major;
    int version_minor;
};

int xoverlay_glx_init(struct xoverlay_library *library);
int xoverlay_glx_destroy(struct xoverlay_library *library);
