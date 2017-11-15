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
dis_push_arrays(void *vertices, uint32_t vertices_count, uint32_t *indices, uint32_t indices_count)
{
    struct draw_instruction_t *last = dis_last_pushed_instruction();

    if (last && (last->type == DI_DRAW_ARRAYS) && last->arr_start + last->arr_count == programs[ds.program].buffer->index_count)
    {
        last->arr_count += indices_count;
        return;
    }
    else
    {
        struct draw_instruction_t instr;

        instr.type = DI_DRAW_ARRAYS;
        instr.arr_start = programs[ds.program].buffer->index_count;
        instr.arr_count += indices_count;
        ds_push_instruction(instr);
    }

    draw_buffer_push(programs[ds.program].buffer, vertices, vertices_count, indices, indices_count);
}

struct draw_instruction_t terminate = { .type = DI_TERMINATE };

struct draw_instruction_t*
ds_fetch_instruction()
{
    if (ds.next_fetch_instruction >= ds.instructions.size)
        return NULL;
    return ds.instructions[ds.next_fetch_instruction++];
}


void
ds_init()
{
    memset(&ds, 0, sizeof(struct draw_state));
    dis_init();
    ds.program = -1;
    ds.font = 0;
    ds.instructions = vector_new(sizeof(struct draw_instruction_t));
    mat4_set_identity(&ds.projection);
    mat4_set_identity(&ds.view);
    mat4_set_identity(&ds.model);
    mat4_set_orthographic(&ds.projection, 0, xoverlay_library.width, xoverlay_library.height, 0, -1, 1);
}

void
ds_destroy()
{
    dis_destroy();
}

void
ds_mark_dirty()
{
    ds.dirty = 1;
}

void
ds_render_if_needed()
{
    if (ds.dirty && ds.program >= 0)
    {
        programs[ds.program].render();
    }
    ds.dirty = 0;
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
}

void
ds_post_render()
{
    glPopClientAttrib();
    glPopAttrib();
    glFlush();
    glXSwapBuffers(xoverlay_library.display, xoverlay_library.window);
    glXMakeContextCurrent(xoverlay_library.display, None, None, None);
}

void
ds_start_next_frame()
{
    ds.program = -1;
    ds.thandle = 0;
    ds.font = 0;
    ds.texture = 0;
    ds.shader = 0;
    ds.next_fetch_instruction = 0;
    vector_clear(ds.instructions);

    draw_buffers_refresh();
}

void
ds_render_next_frame()
{
    ds_pre_render();
    ds_push_instruction(terminate);

    struct draw_instruction_t *instr;

    instr = ds_fetch_instruction();
    char valid = 1;

    while (valid && instr)
    {
        switch (instr->type)
        {
            case DI_SWITCH_PROGRAM:
                if (ds.program != instr->program)
                {
                    ds_render_if_needed();
                    if (ds.program >= 0 && programs[ds.program].unload)
                        programs[ds.program].unload();
                    ds.program = instr->program;
                    if (programs[ds.program].load)
                        programs[ds.program].load();
                }
                break;
            case DI_DRAW_ARRAYS:
                break;
            case DI_PROGRAM_SWITCH_FONT:
                program_freetype_switch_font(instr->font);
                break;
            case DI_TEXTUREAPI_BIND_TEXTURE:
                textureapi_bind(instr->texture);
                break;
            case DI_INVALID_INSTRUCTION:
            case DI_TERMINATE:
            default:
                valid = 0;
                break;

        }
        instr = ds_fetch_instruction();
    }
    ds_render_if_needed();
    if (ds.program >= 0 && programs[ds.program].unload)
    {
        programs[ds.program].unload();
        ds.program = -1;
    }
    ds_post_render();
}

void
ds_use_texture(GLuint texture)
{
    if (ds.texture != texture)
    {
        ds.texture = texture;
        glBindTexture(GL_TEXTURE_2D, texture);
    }
}

void
ds_use_shader(GLuint shader)
{
    if (ds.shader != shader)
    {
        ds.shader = shader;
        glUseProgram(shader);
    }
}

void
ds_use_font(xoverlay_font_handle_t font)
{
    if (ds.font != font)
    {
        ds.font = font;
    }
}

void
ds_prepare_texture(xoverlay_texture_handle_t texture)
{
    if (texture != ds.texture)
    {
        if (ds.instructions.size)
        {
            struct draw_instruction_t *last = ds.instructions[ds.instructions.size - 1];

            if (last && last->type == DI_TEXTUREAPI_BIND_TEXTURE)
            {
                last->texture = texture;
                return;
            }
        }
        struct draw_instruction_t instr;

        instr.type = DI_TEXTUREAPI_BIND_TEXTURE;
        instr.texture = texture;
        ds_push_instruction(instr);
        ds.texture = texture;
    }
}

void
ds_prepare_program(int program)
{
    if (program != ds.program)
    {
        if (ds.instructions.size)
        {
            struct draw_instruction_t *last = ds.instructions[ds.instructions.size - 1];

            if (last && last->type == DI_SWITCH_PROGRAM)
            {
                last->program = program;
                return;
            }
        }
        struct draw_instruction_t instr;

        instr.type = DI_SWITCH_PROGRAM;
        instr.program = program;
        ds_push_instruction(instr);
        ds.program = program;
    }
}

void
ds_prepare_font(xoverlay_font_handle_t font)
{
    if (font != ds.font)
    {
        if (ds.instructions.size)
        {
            struct draw_instruction_t *last = ds.instructions[ds.instructions.size - 1];

            if (last && last->type == DI_PROGRAM_SWITCH_FONT)
            {
                last->font = font;
                return;
            }
        }
        struct draw_instruction_t instr;

        instr.type = DI_PROGRAM_SWITCH_FONT;
        instr.font = font;
        ds_push_instruction(instr);
        ds.font = font;
    }
}
