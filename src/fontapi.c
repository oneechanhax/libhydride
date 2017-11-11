/*
 * fontapi.c
 *
 *  Created on: Nov 11, 2017
 *      Author: nullifiedcat
 */

#include "fontapi_internal.h"

#include <string.h>
#include <memory.h>

int
fontapi_init()
{
    memset(loaded_fonts, 0, sizeof(loaded_fonts));
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
    result.font = texture_font_new_from_file(result.atlas, size, path);

    return fontapi_add_font(result);
}

void
xoverlay_font_unload(xoverlay_font_handle_t handle)
{
    if (handle == 0)
        return;
    if (handle > XOVERLAY_FONT_COUNT)
        return;
    if (loaded_fonts[handle - 1].init == 0)
        return;

    texture_atlas_delete(loaded_fonts[handle - 1].atlas);
    texture_font_delete(loaded_fonts[handle - 1].font);
}

xoverlay_font_handle_t
fontapi_add_font(struct fontapi_font_t font)
{
    for (xoverlay_font_handle_t i = 0; i < XOVERLAY_FONT_COUNT; ++i)
    {
        if (loaded_fonts[i].init == 0)
        {
            memcpy(&loaded_fonts[i], &font, sizeof(font));
            loaded_fonts[i].init = 1;
            return i + 1;
        }
    }
    return 0;
}

texture_font_t*
fontapi_get(xoverlay_font_handle_t handle)
{
    if (handle == 0)
        return NULL;
    if (handle >= XOVERLAY_FONT_COUNT)
        return NULL;
    if (loaded_fonts[handle - 1].init == 0)
        return NULL;
    return loaded_fonts[handle - 1].font;
}

void
xoverlay_string_size(xoverlay_font_handle_t handle, const char *string, int *x, int *y)
{

}
