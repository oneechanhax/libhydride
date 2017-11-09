/*
 * drawglx_internal.h
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "drawglx.h"

#include "vector.h"
#include "vec234.h"

enum
{
    PROGRAM_TRIANGLES_PLAIN,
    PROGRAM_TRIANGLES_TEXTURED,
    PROGRAM_FREETYPE,
    PROGRAM_COUNT
};

enum
{
    CMD_VERTEX_BUFFER
};

typedef void(*program_callback_fn)(struct draw_cmd *);

struct program_t
{
    vertex_buffer_t vertex;
    GLuint shader;

    program_callback_fn load;
    program_callback_fn render;
    program_callback_fn unload;
};

void
program_init_inplace(struct program_t *program, const char *vertex_format, const char *shader_frag, const char *shader_vert, program_callback_fn load, program_callback_fn unload);

struct program_t programs[PROGRAM_COUNT];

int
drawglx_internal_init();

struct draw_cmd
{
    vertex_buffer_t vertex;
    int    program;
    GLuint texture;
};

struct draw_cmd*
draw_cmd_new();

struct draw_state
{
    int    program;
    GLuint texture;
    struct vector_t *commands;
    struct size_t    current_command;
};

struct draw_state*
draw_state_init(size_t vertex_size);

void
draw_state_next_command(struct draw_state *state);

void
draw_state_free(struct draw_state *state);

void
draw_state_clear(struct draw_state *state);

void
draw_state_render(struct draw_state *state);

void
draw_set_texture(struct draw_state *state, GLuint texture);

void
draw_line(struct draw_state *state, float x, float y, float x2, float y2);

void
draw_rect(struct draw_state *state, float x, float y, float w, float h);
