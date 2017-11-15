/*
 * draw_buffers.c
 *
 *  Created on: Nov 15, 2017
 *      Author: nullifiedcat
 */

#include "draw_buffers.h"

#include <GL/glew.h>
#include <GL/gl.h>

void
draw_buffers_init()
{
    memset(&buffer_pc, 0, sizeof(buffer_pc));
    memset(&buffer_pc, 0, sizeof(buffer_ptc));

    glGenBuffers(1, &buffer_pc.vbo);
    glGenVertexArrays(1, &buffer_pc.vao);

    glBindVertexArray(buffer_pc.vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_pc.vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_v2fc4f), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct vertex_v2fc4f), 2 * sizeof(float));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    buffer_pc.index_capacity = 800;
    buffer_pc.index = malloc(buffer_pc.index_capacity);
    buffer_pc.vertex_capacity = 800;
    buffer_pc.vertex = malloc(buffer_pc.vertex_capacity);
    buffer_pc.vertex_size = sizeof(struct vertex_v2fc4f);
    buffer_pc.attributes = 2;

    glGenBuffers(1, &buffer_ptc.vbo);
    glGenVertexArrays(1, &buffer_ptc.vao);

    glBindVertexArray(buffer_ptc.vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_ptc.vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_v2ft2fc4f), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_v2ft2fc4f), 2 * sizeof(float));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(struct vertex_v2ft2fc4f), 4 * sizeof(float));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    buffer_ptc.index_capacity = 800;
    buffer_ptc.index = malloc(buffer_ptc.index_capacity);
    buffer_ptc.vertex_capacity = 800;
    buffer_ptc.vertex = malloc(buffer_ptc.vertex_capacity);
    buffer_ptc.vertex_size = sizeof(struct vertex_v2ft2fc4f);
    buffer_ptc.attributes = 3;
}

void
draw_buffers_refresh()
{
    buffer_pc.index_count = 0;
    buffer_ptc.vertex_count = 0;
}

void
draw_buffer_push(struct draw_buffer_t *buffer, void *vertices, uint32_t vertices_count, unsigned *indices, uint32_t indices_count)
{
    while (buffer->index_capacity <= buffer->index_count + indices_count)
    {
        buffer->index_capacity *= 2;
        buffer->index = realloc(buffer->index, buffer->index_capacity);
    }
    while (buffer->vertex_capacity <= buffer->vertex_count + vertices_count)
    {
        buffer->vertex_capacity *= 2;
        buffer->vertex = realloc(buffer->vertex, buffer->vertex_capacity);
    }

    memcpy(buffer->vertex + buffer->vertex_count * buffer->vertex_size, vertices, vertices_count * buffer->vertex_size);
    memcpy(buffer->index + buffer->index_count * sizeof(unsigned), indices, indices_count * sizeof(unsigned));
    buffer->vertex_count += vertices_count;
    buffer->index_count += indices_count;
}

void
draw_buffer_render(struct draw_buffer_t *buffer, uint32_t start, uint32_t count)
{
    glBindVertexArray(buffer->vao);
    for (uint32_t i = 0; i < buffer->attributes; ++i)
        glEnableVertexAttribArray(i);

    glDrawArrays(GL_TRIANGLES, start, count);

    for (uint32_t i = 0; i < buffer->attributes; ++i)
        glDisableVertexAttribArray(i);

    glBindVertexArray(0);
}
