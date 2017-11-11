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

#include "vector.h"
#include "vec234.h"
#include "mat4.h"

int
drawglx_internal_init();

enum
{
    DI_INVALID_INSTRUCTION,
    DI_SWITCH_PROGRAM,
    DI_PUSH_VERTICES,
    DI_PUSH_INDICES,
    DI_PROGRAM_SWITCH_TEXTURE,
    DI_PROGRAM_SWITCH_FONT,
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
        size_t count;
        /* DI_PROGRAM_SWITCH_TEXTURE */
        GLuint texture;
        /* DI_PROGRAM_SWITCH_FONT */
        xoverlay_font_handle_t font;
    };
};

struct draw_instruction_stream_t
{
    void  *memory;
    size_t capacity;
    size_t write_ptr;
    size_t read_ptr;
    size_t last_draw_instruction_offset;
    GLuint next_index;
};

struct draw_instruction_stream_t dstream;

void
dis_init();

void
dis_destroy();

void
dis_reset();

void
dis_reserve(size_t bytes);

struct draw_instruction_t*
dis_last_pushed_instruction();

void
dis_push_data(size_t bytes, void *data);

void
dis_push_instruction(struct draw_instruction_t instr);

size_t
dis_fetch_data(size_t bytes, void *data);

void*
dis_read_data(size_t bytes);

void
dis_switch_program(int program);

void
dis_push_vertices(size_t count, size_t vertex_size, void *vertex_data);

void
dis_push_indices(size_t count, GLuint *index_data);

void
dis_program_switch_texture(GLuint texture);

void
dis_program_switch_font(xoverlay_font_handle_t font);

void
dis_finish();

struct draw_instruction_t*
dis_fetch_instruction();

struct draw_state
{
    mat4   model, view, projection;

    int    program;
    int    dirty;

    GLuint                 texture;
    xoverlay_font_handle_t font;

    GLuint shader;
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
ds_render_next_frame();

/* To be called by draw functions */

void
ds_prepare_texture(GLuint texture);

void
ds_prepare_font(xoverlay_font_handle_t font);

/* To be called from programs */

void
ds_bind_texture(GLuint texture);

void
ds_use_shader(GLuint shader);

void
ds_use_font(xoverlay_font_handle_t font);

/* Primitive Internal Drawing API */

void
draw_line(vec2 xy, vec2 delta, vec4 color, float thickness);

void
draw_rect(vec2 xy, vec2 hw, vec4 color);

void
draw_rect_outline(vec2 xy, vec2 hw, vec4 color, float thickness);

void
draw_rect_textured(vec2 xy, vec2 hw, vec4 color, int texture, vec2 uv, vec2 st);

void
draw_string_internal(vec2 xy, const char *string, xoverlay_font_handle_t font, vec4 color, int *out_x, int *out_y);

void
draw_string(vec2 xy, const char *string, xoverlay_font_handle_t font, vec4 color, int *out_x, int *out_y);

void
draw_string_with_outline(vec2 xy, const char *string, xoverlay_font_handle_t font, vec4 color, vec4 outline_color, float outline_width, int adjust_outline_alpha, int *out_x, int *out_y);
