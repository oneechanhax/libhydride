/*
 * program_freetype.c
 *
 *  Created on: Nov 10, 2017
 *      Author: nullifiedcat
 */

#include <GL/glew.h>

#include "freetype-gl.h"
#include "programs.h"
#include "texture-atlas.h"
#include "drawglx_internal.h"

DECL_PROGRAM_INIT(freetype)
{
}

DECL_PROGRAM_LOAD(freetype)
{
    vertex_buffer_clear(programs[PROGRAM_FREETYPE].vertex);
    ds_use_shader(programs[PROGRAM_FREETYPE].shader);
    texture_font_t *fnt = fontapi_get(ds.font);
    if (fnt == NULL)
        return;
    ds_bind_texture(fnt->atlas->id);
}

DECL_PROGRAM_RENDER(freetype)
{
    if (programs[PROGRAM_FREETYPE].vertex->indices->size == 0)
        return;
    vertex_buffer_render(programs[PROGRAM_FREETYPE].vertex, GL_TRIANGLES);
}

DECL_PROGRAM_UNLOAD(freetype)
{

}

DECL_PROGRAM_DESTROY(freetype)
{
}

void
program_freetype_switch_font(xoverlay_font_handle_t font)
{
    texture_font_t *fnt = fontapi_get(font);
    if (fnt == NULL)
        return;
    printf("Switching font to %u\n", font);
    if (fnt->atlas->id == 0)
    {
        glGenTextures(1, &fnt->atlas->id);
    }
    ds_bind_texture(fnt->atlas->id);
    if (fnt->atlas->dirty) {
        printf("Refreshing atlas\n");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fnt->atlas->width, fnt->atlas->height, 0, GL_RED, GL_UNSIGNED_BYTE, fnt->atlas->data);
        fnt->atlas->dirty = 0;
    }
}

