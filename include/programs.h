/*
 * programs.h
 *
 *  Created on: Nov 10, 2017
 *      Author: nullifiedcat
 */

#pragma once

#define PROGRAM_INIT(name) program_##name##_init
#define PROGRAM_LOAD(name) program_##name##_load
#define PROGRAM_RENDER(name) program_##name##_render
#define PROGRAM_UNLOAD(name) program_##name##_unload
#define PROGRAM_DESTROY(name) program_##name##_destroy

#define DECL_PROGRAM_INIT(name) void program_##name##_init()
#define DECL_PROGRAM_LOAD(name) void program_##name##_load()
#define DECL_PROGRAM_RENDER(name) void program_##name##_render(struct draw_cmd *cmd)
#define DECL_PROGRAM_UNLOAD(name) void program_##name##_unload()
#define DECL_PROGRAM_DESTROY(name) void program_##name##_destroy()

#define DECL_PROGRAM(name) \
    DECL_PROGRAM_INIT(name); \
    DECL_PROGRAM_LOAD(name); \
    DECL_PROGRAM_RENDER(name); \
    DECL_PROGRAM_UNLOAD(name); \
    DECL_PROGRAM_DESTROY(name)

DECL_PROGRAM(triangles_plain);
DECL_PROGRAM(triangles_textured);
DECL_PROGRAM(freetype);

void program_noop() {};
