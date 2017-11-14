/*
 * fontapi_internal.h
 *
 *  Created on: Nov 11, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "fontapi.h"

#include "freetype-gl.h"

struct fontapi_font_t
{
    int init;

    texture_font_t  *font;
    texture_atlas_t *atlas;
};

texture_font_t*
fontapi_get(xoverlay_font_handle_t handle);

int
fontapi_init();

void
fontapi_destroy();

xoverlay_font_handle_t
fontapi_add_font(struct fontapi_font_t *font);
