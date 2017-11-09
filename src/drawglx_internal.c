/*
 * drawglx_internal.c
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#include "drawglx_internal.h"

struct draw_state*
draw_state_init(size_t vertex_size)
{
    struct draw_state *result = calloc(1, sizeof(struct draw_state));
    result->commands = vector_new(sizeof(struct draw_cmd));
    result->current_command = -1;
    return result;
}

void
draw_state_next_command(struct draw_state *state)
{

}

void
draw_state_free(struct draw_state *state)
{

}

void
draw_state_clear(struct draw_state *state)
{
    for (int i = 0; i < vector_size(state->commands); ++i)
    {
        struct draw_cmd *current = vector_get(state->commands, i);
    }
}

void
draw_state_render(struct draw_state *state)
{
    for (int i = 0; i < vector_size(state->commands); ++i)
    {
        struct draw_cmd *current = vector_get(state->commands, i);
        if (current->texture != state->texture)
        {
            glBindTexture(GL_TEXTURE_2D, current->texture);
        }
        if (current->program != state->program)
        {
            if (state->program >= 0)
            {
                programs[state->program].unload(current);
            }
            glUseProgram(programs[current->program].shader);
            programs[current->program].load(current);
        }
        programs[current->program].render(current);
    }
}

void
draw_set_texture(struct draw_state *state, GLuint texture)
{

}

void
draw_line(struct draw_state *state, float x, float y, float x2, float y2)
{

}

void
draw_rect(struct draw_state *state, float x, float y, float w, float h)
{

}
