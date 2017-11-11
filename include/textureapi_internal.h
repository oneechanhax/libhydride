/*
 * textureapi_internal.h
 *
 *  Created on: Nov 11, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "textureapi.h"

#include <GL/gl.h>
#include <libpng/png.h>

#define XOVERLAY_TEXTURE_COUNT 64

struct textureapi_texture_t
{
    char bound;
    char init;

    int width;
    int height;

    GLuint texture_id;
    char filename[256];
    GLubyte *data;
};


struct textureapi_texture_t loaded_textures[XOVERLAY_TEXTURE_COUNT];

int
textureapi_load_png_rgba(const char *name, struct textureapi_texture_t *out);

struct textureapi_texture_t *
textureapi_get(xoverlay_texture_handle handle);

void
textureapi_bind(xoverlay_texture_handle handle);

int
textureapi_init();

void
textureapi_destroy();

xoverlay_texture_handle
textureapi_add_texture(struct textureapi_texture_t font);


