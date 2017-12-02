/*
 * drawglx_internal.c
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#include "drawglx_internal.h"
#include "vertex_structs.h"
#include "overlay.h"
#include "log.h"

#include <memory.h>
#include <string.h>
#include <math.h>

int
drawglx_internal_init()
{
    return 0;
}

void
ds_init()
{
    memset(&ds, 0, sizeof(struct draw_state));
    ds.font = 0;
    mat4_set_identity(&ds.projection);
    mat4_set_identity(&ds.view);
    mat4_set_identity(&ds.model);
    mat4_set_orthographic(&ds.projection, 0, xoverlay_library.width, xoverlay_library.height, 0, -1, 1);
}

void
ds_destroy()
{
}

void
ds_pre_render()
{
    glXMakeContextCurrent(xoverlay_library.display, xoverlay_library.window, xoverlay_library.window, glx_state.context);

    glClear(GL_COLOR_BUFFER_BIT);
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_STENCIL_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_EDGE_FLAG_ARRAY);
    glDisableClientState(GL_FOG_COORD_ARRAY);
    glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);

    ds.texture = 0;
    ds.font = 0;
}

void
ds_post_render()
{
    program_draw();
    program_reset();
    glPopClientAttrib();
    glPopAttrib();
//    glFlush();
    glXSwapBuffers(xoverlay_library.display, xoverlay_library.window);
    glXMakeContextCurrent(xoverlay_library.display, None, None, None);
}

void
ds_bind_texture(GLuint texture)
{
    if (ds.texture != texture)
    {
        program_draw();
        program_reset();
        ds.texture = texture;
        glBindTexture(GL_TEXTURE_2D, texture);
    }
}
