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
    strncpy(result.path, path, sizeof(result.path) - 1);
    result.size = size;
    xoverlay_font_handle_t handle = fontapi_add_font(&result);
    log_write("fontapi: init font %u: %d %d\n", handle, loaded_fonts[handle].init, result.init);
    fflush(stdout);
    return handle;
}

void
xoverlay_font_unload(xoverlay_font_handle_t handle)
{
    if (handle > XOVERLAY_FONT_COUNT)
        return;
    if (loaded_fonts[handle].init == 0)
        return;
    if (loaded_fonts[handle].loaded == 0)
        return;

    texture_atlas_delete(loaded_fonts[handle].atlas);
    texture_font_delete(loaded_fonts[handle].font);
}

xoverlay_font_handle_t
fontapi_add_font(struct fontapi_font_t *font)
{
    for (xoverlay_font_handle_t i = 0; i < XOVERLAY_FONT_COUNT; ++i)
    {
        if (loaded_fonts[i].init == 0)
        {
            font->init = 1;
            memcpy(&loaded_fonts[i], font, sizeof(struct fontapi_font_t));
            return i;
        }
    }
    return XOVERLAY_FONT_INVALID_HANDLE;
}

void
fontapi_texture_load(xoverlay_font_handle_t handle)
{
    log_write("fontapi: loading font %u\n", handle);
    struct fontapi_font_t *font = &loaded_fonts[handle];

    font->error = 1;

    font->atlas = texture_atlas_new(1024, 1024, 1);
    if (font->atlas != NULL)
    {
        font->font = texture_font_new_from_file(font->atlas, font->size, font->path);
        if (font->font != NULL)
        {
            font->error = 0;
        }
        else
        {
            log_write("fontapi: load: could not init font\n");
        }
    }
    else
    {
        log_write("fontapi: load: could not init atlas\n");
    }

    font->loaded = 1;
}

texture_font_t*
fontapi_get(xoverlay_font_handle_t handle)
{
    log_write("fontapi: finding font %u\n", handle);
    if (handle >= XOVERLAY_FONT_COUNT)
    {
        log_write("fontapi: invalid handle %u\n", handle);
        fflush(stdout);
        return NULL;
    }
    if (loaded_fonts[handle].init == 0)
    {
        log_write("fontapi: font '%s' %u not init\n", loaded_fonts[handle - 1].path, handle);
        fflush(stdout);
        return NULL;
    }
    if (loaded_fonts[handle].loaded == 0)
        fontapi_texture_load(handle);
    if (loaded_fonts[handle].error == 1)
    {
        log_write("fontapi: %u loading error\n", handle);
        fflush(stdout);
        return NULL;
    }
    return loaded_fonts[handle].font;
}
