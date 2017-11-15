/*
 * drawglx_internal.h
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "drawglx.h"
#include "fontapi_internal.h"
#include "textureapi_internal.h"

#include "vector.h"
#include "vec234.h"
#include "mat4.h"

int
drawglx_internal_init();

struct draw_state
{
    mat4   model, view, projection;

    int    dirty;

    GLuint texture;
    xoverlay_font_handle_t font;
};

struct draw_state ds;

void
ds_init();

void
ds_destroy();

void
ds_mark_dirty();

void
ds_render_if_needed();

void
ds_pre_render();

void
ds_post_render();

void
ds_bind_texture(GLuint texture);
