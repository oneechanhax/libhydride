/*
 * programs.h
 *
 *  Created on: Nov 10, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "freetype-gl.h"
#include "vertex-buffer.h"

struct draw_cmd;

#define PROGRAM_INIT(name) program_##name##_init
#define PROGRAM_LOAD(name) program_##name##_load
#define PROGRAM_RENDER(name) program_##name##_render
#define PROGRAM_UNLOAD(name) program_##name##_unload
#define PROGRAM_DESTROY(name) program_##name##_destroy

#define DECL_PROGRAM_INIT(name) void program_##name##_init()
#define DECL_PROGRAM_LOAD(name) void program_##name##_load()
#define DECL_PROGRAM_RENDER(name) void program_##name##_render()
#define DECL_PROGRAM_UNLOAD(name) void program_##name##_unload()
#define DECL_PROGRAM_DESTROY(name) void program_##name##_destroy()

#define DECL_PROGRAM(name) \
    DECL_PROGRAM_INIT(name); \
    DECL_PROGRAM_LOAD(name); \
    DECL_PROGRAM_RENDER(name); \
    DECL_PROGRAM_UNLOAD(name); \
    DECL_PROGRAM_DESTROY(name)

enum
{
    PROGRAM_TRIANGLES_PLAIN,
    PROGRAM_TRIANGLES_TEXTURED,
    PROGRAM_FREETYPE,
    PROGRAM_COUNT
};

typedef void(*program_callback_empty_fn)();

struct program_t
{
    vertex_buffer_t *vertex;
    size_t vertex_size;
    GLuint shader;

    program_callback_empty_fn init;
    program_callback_empty_fn load;
    program_callback_empty_fn render;
    program_callback_empty_fn unload;
    program_callback_empty_fn destroy;
};

void
program_init_inplace(struct program_t *program, const char *vertex_format, const char *shader_frag, const char *shader_vert, program_callback_empty_fn init, program_callback_empty_fn load, program_callback_empty_fn render, program_callback_empty_fn unload, program_callback_empty_fn destroy);

void
program_init_everything();

struct program_t programs[PROGRAM_COUNT];

DECL_PROGRAM(triangles_plain);
DECL_PROGRAM(triangles_textured);
DECL_PROGRAM(freetype);

void
program_freetype_switch_font(texture_font_t *font);
