/*
 * fontapi.c
 *
 *  Created on: Nov 11, 2017
 *      Author: nullifiedcat
 */

#include "fontapi_internal.h"
#include "log.h"

#include <string.h>
#include <memory.h>
#include <stdio.h>

struct fontapi_font_t *loaded_fonts = 0;

int
fontapi_init()
{
    loaded_fonts = calloc(XOVERLAY_FONT_COUNT, sizeof(struct fontapi_font_t));
    log_write("fontapi: init %u\n", XOVERLAY_FONT_COUNT * sizeof(struct fontapi_font_t));
    fflush(stdout);
    return 0;
}

void
fontapi_destroy()
{

}

xoverlay_font_handle_t
xoverlay_font_load(const char *path, float size)
{
    struct fontapi_font_t result;
    memset(&result, 0, sizeof(result));

    result.atlas = texture_atlas_new(1024, 1024, 1);
    if (result.atlas != NULL)
    {
        glGenTextures(1, &result.atlas->id);
        result.font = texture_font_new_from_file(result.atlas, size, path);
        if (result.font == NULL)
        {
            log_write("fontapi: load: could not init font\n");
        }
    }
    else
    {
        log_write("fontapi: load: could not init atlas\n");
    }
    return fontapi_add_font(&result);
}

void
xoverlay_font_unload(xoverlay_font_handle_t handle)
{
    if (handle > XOVERLAY_FONT_COUNT)
        return;
    if (loaded_fonts[handle].init == 0)
        return;

    texture_atlas_delete(loaded_fonts[handle].atlas);
    texture_font_delete(loaded_fonts[handle].font);
}

xoverlay_font_handle_t
fontapi_add_font(struct fontapi_font_t *font)
{
    for (xoverlay_font_handle_t i = 1; i < XOVERLAY_FONT_COUNT; ++i)
    {
        if (loaded_fonts[i].init == 0)
        {
            font->init = 1;
            memcpy(&loaded_fonts[i], font, sizeof(struct fontapi_font_t));
            log_write("init success: %u %p %p\n", i, font->atlas, font->font);
            return i;
        }
    }
    return XOVERLAY_FONT_INVALID_HANDLE;
}

texture_font_t*
fontapi_get(xoverlay_font_handle_t handle)
{
    if (handle >= XOVERLAY_FONT_COUNT)
    {
        log_write("fontapi: invalid handle %u\n", handle);
        return NULL;
    }
    if (loaded_fonts[handle].init == 0)
    {
        log_write("fontapi: font %u not init\n", handle);
        return NULL;
    }
    return loaded_fonts[handle].font;
}
