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
#include "mat4.h"

enum
{
    PROGRAM_TRIANGLES_PLAIN,
    PROGRAM_TRIANGLES_TEXTURED,
    PROGRAM_FREETYPE,
    PROGRAM_COUNT
};

typedef void(*program_callback_fn)(struct draw_cmd *);
typedef void(*program_callback_empty_fn)();

struct program_t
{
    vertex_buffer_t *vertex;
    GLuint shader;

    program_callback_empty_fn init;
    program_callback_empty_fn load;
    program_callback_fn render;
    program_callback_empty_fn unload;
    program_callback_empty_fn destroy;
};

void
program_init_inplace(struct program_t *program, const char *vertex_format, const char *shader_frag, const char *shader_vert, program_callback_empty_fn init, program_callback_empty_fn load, program_callback_fn render, program_callback_empty_fn unload, program_callback_empty_fn destroy);

struct program_t programs[PROGRAM_COUNT];

int
drawglx_internal_init();

struct draw_cmd
{
    vector_t *vertices;
    int       program;
    GLuint    texture;
};

struct draw_cmd*
draw_cmd_new();

struct draw_state
{
    mat4   model, view, projection;

    int    program;

    GLuint texture;
    GLuint shader;

    struct vector_t *commands;
    struct size_t    current_command;
};

struct draw_state drawstate;

void
draw_state_init();

void
draw_state_next_command();

void
draw_state_free();

void
draw_state_clear();

void
draw_state_render();

void
draw_set_texture(GLuint texture);

void
draw_line(float x, float y, float x2, float y2);

void
draw_rect(float x, float y, float w, float h);
