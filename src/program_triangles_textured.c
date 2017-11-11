/*
 * program_triangles_textured.c
 *
 *  Created on: Nov 10, 2017
 *      Author: nullifiedcat
 */

#include <GL/glew.h>

#include "programs.h"
#include "drawglx_internal.h"

DECL_PROGRAM_INIT(triangles_textured)
{

}

DECL_PROGRAM_LOAD(triangles_textured)
{
    ds_use_shader(programs[PROGRAM_TRIANGLES_TEXTURED].shader);
    vertex_buffer_clear(programs[PROGRAM_TRIANGLES_TEXTURED].vertex);
}

DECL_PROGRAM_RENDER(triangles_textured)
{
    if (programs[PROGRAM_TRIANGLES_TEXTURED].vertex->indices->size == 0)
        return;
    vertex_buffer_render(programs[PROGRAM_TRIANGLES_TEXTURED].vertex, GL_TRIANGLES);
}

DECL_PROGRAM_UNLOAD(triangles_textured)
{

}

DECL_PROGRAM_DESTROY(triangles_textured)
{

}
