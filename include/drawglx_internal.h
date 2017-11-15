/*
 * drawglx_internal.h
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "drawglx.h"
#include "fontapi_internal.h"
#include "programs.h"
#include "textureapi_internal.h"

#include "vector.h"
#include "vec234.h"
#include "mat4.h"

int
drawglx_internal_init();

enum
{
    DI_INVALID_INSTRUCTION,
    DI_SWITCH_PROGRAM,
    DI_DRAW_ARRAYS,
    DI_PROGRAM_SWITCH_FONT,
    DI_TEXTUREAPI_BIND_TEXTURE,
    DI_TERMINATE
};

struct draw_instruction_t
{
    int type;
    union
    {
        /* DI_SWITCH_PROGRAM */
        int    program;
        /* DI_PUSH_VERTICES / DI_PUSH_INDICES */
        struct
        {
            uint32_t arr_start;
            uint32_t arr_count;
        };
        /* DI_PROGRAM_SWITCH_FONT */
        xoverlay_font_handle_t font;
        /* DI_TEXTUREAPI_BIND_TEXTURE */
        xoverlay_texture_handle_t texture;
    };
};

void
ds_push_instruction(struct draw_instruction_t instr);

void
ds_push_vertices(size_t count, size_t vertex_size, void *vertex_data);

struct draw_instruction_t*
ds_fetch_instruction();

struct draw_state
{
    vector_t instructions;
    uint32_t next_fetch_instruction;
    mat4     model, view, projection;

    int    program;
    int    dirty;

    xoverlay_font_handle_t    font;
    xoverlay_texture_handle_t thandle;

    GLuint shader;
    GLuint texture;
};

struct draw_state ds;

void
ds_init();

void
ds_destroy();

void
ds_mark_dirty();

void
ds_prepare_program(int program);

void
ds_render_if_needed();

void
ds_pre_render();

void
ds_post_render();

void
ds_start_next_frame();

void
ds_render_next_frame();

/* To be called by draw functions */

void
ds_prepare_texture(xoverlay_texture_handle_t texture);

void
ds_prepare_font(xoverlay_font_handle_t font);

/* To be called from programs */

void
ds_use_texture(GLuint texture);

void
ds_use_shader(GLuint shader);

void
ds_use_font(xoverlay_font_handle_t font);
