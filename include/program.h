#pragma once

#include <vertex-buffer.h>

struct program_t
{
    unsigned shader;
    vertex_buffer_t *buffer;
};

struct program_t program;

void
program_init();

void
program_draw();