/*
 * programs.h
 *
 *  Created on: Nov 10, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "freetype-gl.h"

struct draw_cmd;

#define PROGRAM_INIT(name) program_##name##_init
#define PROGRAM_LOAD(name) program_##name##_load
#define PROGRAM_RENDER(name) program_##name##_render
#define PROGRAM_UNLOAD(name) program_##name##_unload
#define PROGRAM_DESTROY(name) program_##name##_destroy
#define PROGRAM_CHECK(name) program_##name##_check

#define DECL_PROGRAM_INIT(name) void program_##name##_init()
#define DECL_PROGRAM_LOAD(name) void program_##name##_load()
#define DECL_PROGRAM_RENDER(name) void program_##name##_render(struct draw_cmd *cmd)
#define DECL_PROGRAM_UNLOAD(name) void program_##name##_unload()
#define DECL_PROGRAM_DESTROY(name) void program_##name##_destroy()
#define DECL_PROGRAM_CHECK(name) int program_##name##_check(struct draw_cmd *cmd, void *data)

#define DECL_PROGRAM(name) \
    DECL_PROGRAM_INIT(name); \
    DECL_PROGRAM_LOAD(name); \
    DECL_PROGRAM_RENDER(name); \
    DECL_PROGRAM_UNLOAD(name); \
    DECL_PROGRAM_DESTROY(name); \
    DECL_PROGRAM_CHECK(name)

enum
{
    PROGRAM_TRIANGLES_PLAIN,
    PROGRAM_TRIANGLES_TEXTURED,
    PROGRAM_FREETYPE,
    PROGRAM_COUNT
};

typedef void(*program_callback_fn)(struct draw_cmd *);
typedef void(*program_callback_empty_fn)();
typedef int(*program_callback_check_command_fn)(struct draw_cmd *cmd, void *data);

struct program_t
{
    vertex_buffer_t *vertex;
    GLuint shader;

    program_callback_empty_fn init;
    program_callback_empty_fn load;
    program_callback_fn render;
    program_callback_empty_fn unload;
    program_callback_empty_fn destroy;
    program_callback_check_command_fn check;
};

void
program_init_inplace(struct program_t *program, const char *vertex_format, const char *shader_frag, const char *shader_vert, program_callback_empty_fn init, program_callback_empty_fn load, program_callback_fn render, program_callback_empty_fn unload, program_callback_empty_fn destroy, program_callback_check_command_fn check);

struct program_t programs[PROGRAM_COUNT];

struct program_data_t
{
    union
    {
        struct
        {
            int pad;
        } triangles_plain;
        struct
        {
            GLuint texture;
        } triangles_textured;
        struct
        {
            texture_font_t *font;
        } freetype;
    };
};

DECL_PROGRAM(triangles_plain);
DECL_PROGRAM(triangles_textured);
DECL_PROGRAM(freetype);
