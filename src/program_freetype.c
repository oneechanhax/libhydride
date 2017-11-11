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

texture_atlas_t *atlas;
texture_font_t *font;
vec2 pen;

DECL_PROGRAM_INIT(freetype)
{
    atlas = texture_atlas_new(1024, 1024, 1);
    font = texture_font_new_from_file(atlas, 14, "/usr/share/fonts/truetype/freefont/FreeMono.ttf");
    glGenTextures(1, &atlas->id);
}

DECL_PROGRAM_LOAD(freetype)
{
    vertex_buffer_clear(programs[PROGRAM_FREETYPE].vertex);
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
    texture_atlas_delete(atlas);
    texture_font_delete(font);
}

void
program_freetype_switch_font(texture_font_t *font)
{

}

