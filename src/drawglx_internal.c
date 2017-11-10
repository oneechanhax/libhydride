/*
 * drawglx_internal.c
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#include "drawglx_internal.h"

#include <memory.h>
#include <string.h>

int
drawglx_internal_init()
{
    return 0;
}

void
dis_init()
{
    dstream.capacity = 128;
    dstream.memory = malloc(dstream.capacity);
    struct draw_instruction_t *instr = dstream.memory;
    instr->type = DI_INVALID_INSTRUCTION;
}

void
dis_destroy()
{
    free(dstream.memory);
}

void
dis_reset()
{
    dstream.write_ptr = 0;
    dstream.read_ptr = 0;
}

void
dis_reserve(size_t bytes)
{
    while (dstream.write_ptr + bytes > dstream.capacity)
    {
        dstream.capacity *= 2;
        dstream.memory = realloc(dstream.memory, dstream.capacity);
    }
}

struct draw_instruction_t*
dis_last_pushed_instruction()
{
    if (dstream.write_ptr == 0)
    {
        return NULL;
    }
    return (struct draw_instruction_t *)(dstream.memory + dstream.last_draw_instruction_offset);
}

void
dis_push_data(size_t bytes, void *data)
{
    dis_reserve(bytes);
    memcpy(dstream.memory + dstream.write_ptr, data, bytes);
    dstream.write_ptr += bytes;
}

void
dis_push_instruction(struct draw_instruction_t instr)
{
    dstream.last_draw_instruction_offset = dstream.write_ptr;
    dis_push_data(sizeof(struct draw_instruction_t), &instr);
}

size_t
dis_fetch_data(size_t bytes, void *data)
{
    size_t read_count = bytes;
    if (dstream.read_ptr + bytes < dstream.capacity)
        read_count = dstream.capacity - dstream.read_ptr;
    memcpy(data, dstream.memory + dstream.read_ptr, bytes);
    dstream.read_ptr += bytes;
    return read_count;
}

void
dis_switch_program(int program)
{
    struct draw_instruction_t *last = dis_last_pushed_instruction();

    if (last && last->type == DI_SWITCH_PROGRAM)
    {
        last->program = program;
        return;
    }
    else
    {
        struct draw_instruction_t instr;

        instr.type = DI_SWITCH_PROGRAM;
        instr.program = program;
        dis_push_instruction(instr);
    }
}

void
dis_push_vertices(size_t count, size_t vertex_size, void *vertex_data)
{
    struct draw_instruction_t *last = dis_last_pushed_instruction();

    if (last && (last->type == DI_PUSH_VERTICES))
    {
        dis_push_data(count * vertex_size, vertex_data);
        last->count += count;
        return;
    }
    else
    {
        struct draw_instruction_t instr;

        instr.type = DI_PUSH_VERTICES;
        instr.count = count;
        dis_push_instruction(instr);
        dis_push_data(count * vertex_size, vertex_data);
    }
}

void
dis_push_indices(size_t count, GLuint *index_data)
{
    struct draw_instruction_t *last = dis_last_pushed_instruction();

    if (last && (last->type == DI_PUSH_INDICES))
    {
        dis_push_data(count * sizeof(GLuint), index_data);
        last->count += count;
        return;
    }
    else
    {
        struct draw_instruction_t instr;

        instr.type = DI_PUSH_INDICES;
        instr.count = count;
        dis_push_instruction(instr);
        dis_push_data(count * sizeof(GLuint), index_data);
    }
}

void
dis_program_switch_texture(GLuint texture)
{
    struct draw_instruction_t *last = dis_last_pushed_instruction();

    if (last && last->type == DI_PROGRAM_SWITCH_TEXTURE)
    {
        last->texture = texture;
        return;
    }
    else
    {
        struct draw_instruction_t instr;

        instr.type = DI_PROGRAM_SWITCH_TEXTURE;
        instr.texture = texture;
        dis_push_instruction(instr);
    }
}

void
dis_program_switch_font(texture_font_t *font)
{
    struct draw_instruction_t *last = dis_last_pushed_instruction();

    if (last && last->type == DI_PROGRAM_SWITCH_FONT)
    {
        last->font = font;
        return;
    }
    else
    {
        struct draw_instruction_t instr;

        instr.type = DI_PROGRAM_SWITCH_FONT;
        instr.font = font;
        dis_push_instruction(instr);
    }
}

struct draw_instruction_t terminate = { .type = DI_TERMINATE };

void
dis_finish()
{
    dis_push_instruction(terminate);
}

struct draw_instruction_t*
dis_fetch_instruction()
{
    if (dstream.read_ptr + sizeof(struct draw_instruction_t) > dstream.capacity)
        return NULL;
    struct draw_instruction_t *result;
    result = (struct draw_instruction_t *)(dstream.memory + dstream.read_ptr);
    dstream.read_ptr += sizeof(struct draw_instruction_t);
    return result;
}

struct draw_cmd*
draw_cmd_new(int program)
{
    struct draw_cmd *result = calloc(1, sizeof(struct draw_cmd));
    result->program = program;
    result->vertices = vector_new(programs[program]->vertex->vertices->item_size);
    result->indices = vector_new(programs[program]->vertex->indices->item_size);
    return result;
}

void
draw_cmd_delete(struct draw_cmd *cmd)
{
    vector_delete(cmd->vertices);
    vector_delete(cmd->indices);
    free(cmd);
}

void
ds_init()
{
    memset(&ds, 0, sizeof(struct draw_state));
    ds.commands = vector_new(sizeof(struct draw_cmd));
    vector_push_back(ds.commands, calloc(1, sizeof(struct draw_cmd)));
    ds.current_command = 0;
}

void
ds_destroy()
{
    for (int i = 0; i < ds.commands->size; ++i)
    {
        struct draw_cmd *cmd = vector_get(ds.commands, i);
        draw_cmd_delete(cmd);
    }
    vector_delete(ds.commands);
}

void
ds_alloc_next_command(int program)
{
    struct draw_cmd *result = calloc(1, sizeof(struct draw_cmd));
    result->program = program;
    vector_push_back(ds.commands, result);
    ds.current_command++;
}

void
ds_next_frame()
{
    for (int i = 0; i < ds.commands->size; ++i)
    {
        struct draw_cmd *cmd = vector_get(ds.commands, i);
        draw_cmd_delete(cmd);
    }
    vector_resize(ds.commands, 1);
    memset(vector_get(ds.commands, 0), 0, sizeof(struct draw_cmd));
    ds.current_command = 0;
}

void
ds_render()
{

}

void
ds_render()
{
    for (int i = 0; i < vector_size(state->commands); ++i)
    {
        struct draw_cmd *current = vector_get(state->commands, i);
        if (current->texture != state->texture)
        {
            glBindTexture(GL_TEXTURE_2D, current->texture);
            state->texture = current->texture;
        }
        if (current->program != state->program)
        {
            if (state->program >= 0)
            {
                programs[state->program].unload();
            }
            if (programs[current->program].shader != state->shader)
            {
                glUseProgram(programs[current->program].shader);
                state->shader = programs[current->program].shader;
            }
            programs[current->program].load();
        }
        programs[current->program].render(current);
    }
    if (state->program >= 0)
        programs[state->program].unload();
    state->program = -1;
}

void
ds_prepare_command(int program)
{
    int should_alloc_command = 0;
    struct draw_cmd *current = vector_get(ds.commands, ds.current_command);

    if (current == NULL)
        should_alloc_command = 1;
    else if (program != current->program)
        should_alloc_command = 1;
    else if (programs[program].check && !programs[program].check(current))
        should_alloc_command = 1;

    if (should_alloc_command)
    {
        ds_alloc_next_command();
    }
}

void
draw_line(vec2 xy, vec2 delta, vec4 color)
{
    ds_prepare_command(PROGRAM_TRIANGLES_PLAIN);
    vertex_buffer_delete();
}

void
draw_rect(vec2 xy, vec2 hw, vec4 color)
{
    ds_prepare_command(PROGRAM_TRIANGLES_PLAIN);

}

void
draw_rect_outline(vec2 xy, vec2 hw, vec4 color)
{
    ds_prepare_command(PROGRAM_TRIANGLES_PLAIN);
}

void
draw_rect_textured(vec2 xy, vec2 hw, vec4 color, int texture, vec2 uv, vec2 st)
{
    ds_prepare_command(PROGRAM_TRIANGLES_TEXTURED);
}

void
draw_string(vec2 xy, const char *string, texture_font_t *font, vec4 color)
{
    ds_prepare_command(PROGRAM_FREETYPE);
}

void
draw_string_outline(vec2 xy, const char *string, texture_font_t *font, vec4 color)
{
    ds_prepare_command(PROGRAM_FREETYPE);
}

void
draw_string_with_outline(vec2 xy, const char *string, texture_font_t *font, vec4 color, vec4 outline_color)
{
    ds_prepare_command(PROGRAM_FREETYPE);
}

void
get_string_size(const char *string, texture_font_t *font, vec2 *out)
{

}
