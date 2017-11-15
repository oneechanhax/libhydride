/*
 * draw_buffers.h
 *
 *  Created on: Nov 15, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include <stdint.h>

struct draw_buffer_t
{
    uint32_t vao;
    uint32_t vbo;
    uint32_t attributes;

    void *vertex;
    uint32_t vertex_capacity;
    uint32_t vertex_count;
    uint32_t vertex_size;

    uint32_t *index;
    uint32_t index_capacity;
    uint32_t index_count;
};

struct draw_buffer_t buffer_pc;
struct draw_buffer_t buffer_ptc;

void
draw_buffers_init();

void
draw_buffers_refresh();

void
draw_buffer_push(struct draw_buffer_t *buffer, void *vertices, uint32_t vertices_count, uint32_t *indices, uint32_t indices_count);

void
draw_buffer_render(struct draw_buffer_t *buffer, uint32_t start, uint32_t count);
