/*
 * drawglx_internal.h
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "drawglx.h"
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
        int             program;
        /* DI_PUSH_VERTICES / DI_PUSH_INDICES */
        size_t          count;
        /* DI_PROGRAM_SWITCH_TEXTURE */
        GLuint          texture;
        /* DI_PROGRAM_SWITCH_FONT */
        texture_font_t *font;
    };
};

struct draw_instruction_stream_t
{
    void  *memory;
    size_t capacity;
    size_t write_ptr;
    size_t read_ptr;
    size_t last_draw_instruction_offset;
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

void
dis_switch_program(int program);

void
dis_push_vertices(size_t count, size_t vertex_size, void *vertex_data);

void
dis_push_indices(size_t count, GLuint *index_data);

void
dis_program_switch_texture(GLuint texture);

void
dis_program_switch_font(texture_font_t *font);

void
dis_finish();

struct draw_instruction_t*
dis_fetch_instruction();

struct draw_cmd
{
    vector_t *vertices;
    vector_t *indices;

    int       program;
    struct program_data_t data;
};

struct draw_cmd*
draw_cmd_new(int program);

void
draw_cmd_delete(struct draw_cmd *cmd);

struct draw_state
{
    mat4   model, view, projection;

    int    program;

    GLuint texture;
    GLuint shader;

    struct vector_t *commands;
    struct size_t    current_command;
};

struct draw_state ds;

void
ds_init();

void
ds_destroy();

void
ds_alloc_next_command(int program);

void
ds_next_frame();

void
ds_render();

void
ds_prepare_command(int program);

/* Primitive Internal Drawing API */

void
draw_line(vec2 xy, vec2 delta, vec4 color);

void
draw_rect(vec2 xy, vec2 hw, vec4 color);

void
draw_rect_outline(vec2 xy, vec2 hw, vec4 color);

void
draw_rect_textured(vec2 xy, vec2 hw, vec4 color, int texture, vec2 uv, vec2 st);

void
draw_string(vec2 xy, const char *string, texture_font_t *font, vec4 color);

void
draw_string_outline(vec2 xy, const char *string, texture_font_t *font, vec4 color);

void
draw_string_with_outline(vec2 xy, const char *string, texture_font_t *font, vec4 color, vec4 outline_color);

void
get_string_size(const char *string, texture_font_t *font, vec2 *out);
