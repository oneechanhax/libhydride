/*
 * program_triangles_plain.c
 *
 *  Created on: Nov 10, 2017
 *      Author: nullifiedcat
 */

#include <GL/glew.h>

#include "programs.h"
#include "drawglx_internal.h"
#include "log.h"

DECL_PROGRAM_INIT(triangles_plain)
{

}

DECL_PROGRAM_LOAD(triangles_plain)
{
    ds_use_shader(programs[PROGRAM_TRIANGLES_PLAIN].shader);
    vertex_buffer_clear(programs[PROGRAM_TRIANGLES_PLAIN].vertex);
}

DECL_PROGRAM_RENDER(triangles_plain)
{
    if (programs[PROGRAM_TRIANGLES_PLAIN].vertex->indices->size == 0)
        return;
    vertex_buffer_render(programs[PROGRAM_TRIANGLES_PLAIN].vertex, GL_TRIANGLES);
}

DECL_PROGRAM_UNLOAD(triangles_plain)
{

}

DECL_PROGRAM_DESTROY(triangles_plain)
{

}
