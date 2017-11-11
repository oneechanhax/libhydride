/*
 * drawglx_internal.c
 *
 *  Created on: Nov 9, 2017
 *      Author: nullifiedcat
 */

#include "drawglx_internal.h"
#include "vertex_structs.h"
#include "overlay.h"

#include <memory.h>
#include <string.h>
#include <math.h>

int
drawglx_internal_init()
{
    return 0;
}

/* STACK SAFE */
void
dis_init()
{
    dstream.capacity = 128;
    dstream.memory = malloc(dstream.capacity);
    dstream.read_ptr = 0;
    dstream.write_ptr = 0;
    dstream.next_index = 0;
}

/* STACK SAFE */
void
dis_destroy()
{
    free(dstream.memory);
}

/* STACK SAFE */
void
dis_reset()
{
    dstream.write_ptr = 0;
    dstream.read_ptr = 0;
    dstream.next_index = 0;
}

/* STACK SAFE */
void
dis_reserve(size_t bytes)
{
    while (dstream.write_ptr + bytes > dstream.capacity)
    {
        dstream.capacity *= 2;
        dstream.capacity += 4;
        dstream.memory = realloc(dstream.memory, dstream.capacity);
    }
}

/* STACK SAFE */
struct draw_instruction_t*
dis_last_pushed_instruction()
{
    if (dstream.write_ptr == 0)
    {
        return NULL;
    }
    if (dstream.last_draw_instruction_offset + sizeof(struct draw_instruction_t) > dstream.capacity)
    {
        return NULL;
    }
    return (struct draw_instruction_t *)(dstream.memory + dstream.last_draw_instruction_offset);
}

/* STACK SAFE */
void
dis_push_data(size_t bytes, void *data)
{
    dis_reserve(bytes);
    memcpy(dstream.memory + dstream.write_ptr, data, bytes);
    dstream.write_ptr += bytes;
}

/* STACK SAFE */
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

void*
dis_read_data(size_t bytes)
{
    void *result = dstream.memory + dstream.read_ptr;
    dstream.read_ptr += bytes;
    return result;
}

void
dis_switch_program(int program)
{
    struct draw_instruction_t *last = dis_last_pushed_instruction();

    if (last && last->type == DI_SWITCH_PROGRAM)
    {
        if (last->program != program)
        {
            last->program = program;
            dstream.next_index = 0;
        }
        return;
    }
    else
    {
        struct draw_instruction_t instr;

        instr.type = DI_SWITCH_PROGRAM;
        instr.program = program;
        dstream.next_index = 0;
        dis_push_instruction(instr);
    }
}

void
dis_push_vertices(size_t count, size_t vertex_size, void *vertex_data)
{
    struct draw_instruction_t *last = dis_last_pushed_instruction();
    dstream.next_index += count;

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


void
ds_init()
{
    memset(&ds, 0, sizeof(struct draw_state));
    dis_init();
    ds.program = -1;
    mat4_set_identity(&ds.projection);
    mat4_set_identity(&ds.view);
    mat4_set_identity(&ds.model);
    mat4_set_orthographic(&ds.projection, 0, xoverlay_library.width, 0, xoverlay_library.height, -1, 1);
}

void
ds_destroy()
{
    dis_destroy();
}

void
ds_mark_dirty()
{
    ds.dirty = 1;
}

void
ds_render_if_needed()
{
    if (ds.dirty && ds.program >= 0)
    {
        printf("calling renderer...\n");
        programs[ds.program].render();
    }
    ds.dirty = 0;
}

void
ds_pre_render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_STENCIL_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_EDGE_FLAG_ARRAY);
    glDisableClientState(GL_FOG_COORD_ARRAY);
    glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);
}

void
ds_post_render()
{
    printf("GL error: %s\n", glewGetErrorString(glGetError()));
    glPopClientAttrib();
    glPopAttrib();
    glFlush();
    glXSwapBuffers(xoverlay_library.display, xoverlay_library.window);
}

void
ds_render_next_frame()
{
    ds_pre_render();
    dis_finish();
    ds.program = -1;
    struct draw_instruction_t *instr;

    instr = dis_fetch_instruction();

    while (instr)
    {
        printf("instruction: %d data: %d\n", instr->type, instr->program);
        switch (instr->type)
        {
            case DI_SWITCH_PROGRAM:
                if (ds.program != instr->program)
                {
                    ds_render_if_needed();
                    if (ds.program >= 0 && programs[ds.program].unload)
                        programs[ds.program].unload();
                    ds.program = instr->program;
                    if (programs[ds.program].load)
                        programs[ds.program].load();
                }
                break;
            case DI_PUSH_VERTICES:
                ds_mark_dirty();
                printf("pushing vertices:\n");
                float *vert = dis_read_data(instr->count * programs[ds.program].vertex_size);
                vertex_buffer_push_back_vertices(programs[ds.program].vertex, vert, instr->count);
                break;
            case DI_PUSH_INDICES:
                ds_mark_dirty();
                GLuint *indices = dis_read_data(instr->count * sizeof(GLuint));
                vertex_buffer_push_back_indices(programs[ds.program].vertex, indices, instr->count);
                break;
            case DI_PROGRAM_SWITCH_TEXTURE:
                ds_bind_texture(instr->texture);
                break;
            case DI_PROGRAM_SWITCH_FONT:
                program_freetype_switch_font(instr->font);
                break;
            case DI_INVALID_INSTRUCTION:
            case DI_TERMINATE:
            default:
                break;

        }
        instr = dis_fetch_instruction();
    }
    ds_render_if_needed();
    if (ds.program >= 0 && programs[ds.program].unload)
    {
        programs[ds.program].unload();
        ds.program = -1;
    }
    dis_reset();
    ds_post_render();
}

void
ds_bind_texture(GLuint texture)
{
    if (ds.texture != texture)
    {
        ds.texture = texture;
        glBindTexture(GL_TEXTURE_2D, texture);
    }
}

void
ds_use_shader(GLuint shader)
{
    if (ds.shader != shader)
    {
        ds.shader = shader;
        printf("glUseProgram(%u)\n", shader);
        glUseProgram(shader);
    }
}

void
ds_prepare_program(int program)
{
    printf("current program: %d, new: %d\n", ds.program, program);
    if (program != ds.program)
    {
        printf("switching to program %d from %d\n", program, ds.program);
        dis_switch_program(program);
        ds.program = program;
    }
}

void
draw_line(vec2 xy, vec2 delta, vec4 color, float thickness)
{
    ds_prepare_program(PROGRAM_TRIANGLES_PLAIN);
    GLuint idx = dstream.next_index;

    struct vertex_v2fc4f vertices[0];
    /* A C => ABC, CDB
     * B D
     */
    GLuint indices[6] = { idx, idx + 1, idx + 2, idx + 2, idx + 3, idx + 1 };

    vec2 normal = { .x = -delta.y, .y = delta.x };
    vec2 end = (vec2){ xy.x + delta.x, xy.y + delta.y };
    float length = sqrtf(normal.x * normal.x + normal.y * normal.y);
    if (length == 0)
        return;
    length /= thickness;
    normal.x /= length;
    normal.y /= length;

    vertices[0].pos.x = xy.x - normal.x;
    vertices[0].pos.y = xy.y - normal.y;
    vertices[0].color = color;

    vertices[1].pos.x = xy.x + normal.x;
    vertices[1].pos.y = xy.y + normal.y;
    vertices[1].color = color;

    vertices[2].pos.x = end.x - normal.x;
    vertices[2].pos.y = end.y - normal.y;
    vertices[2].color = color;

    vertices[3].pos.x = end.x + normal.x;
    vertices[3].pos.y = end.y + normal.y;
    vertices[3].color = color;

    dis_push_vertices(4, sizeof(struct vertex_v2fc4f), vertices);
    dis_push_indices(6, indices);
}

void
draw_rect(vec2 xy, vec2 hw, vec4 color)
{
    ds_prepare_program(PROGRAM_TRIANGLES_PLAIN);
    GLuint idx = dstream.next_index;

    struct vertex_v2fc4f vertices[4];
    /* A C => ABC, CDB
     * B D
     */
    GLuint indices[6] = { idx, idx + 1, idx + 2, idx + 2, idx + 3, idx + 1 };

    vertices[0].pos.x = xy.x;
    vertices[0].pos.y = xy.y;
    vertices[0].color = color;

    vertices[1].pos.x = xy.x;
    vertices[1].pos.y = xy.y + hw.y;
    vertices[1].color = color;

    vertices[2].pos.x = xy.x + hw.x;
    vertices[2].pos.y = xy.y + hw.y;
    vertices[2].color = color;

    vertices[3].pos.x = xy.x + hw.x;
    vertices[3].pos.y = xy.y;
    vertices[3].color = color;

    dis_push_vertices(4, sizeof(struct vertex_v2fc4f), vertices);
    dis_push_indices(6, indices);
}

void
draw_rect_outline(vec2 xy, vec2 hw, vec4 color, float thickness)
{
    vec2 point = xy;
    vec2 delta = { hw.x, 0 };
    draw_line(point, delta, color, thickness);
    point.x += delta.x;
    delta.x = 0;
    delta.y = hw.y;
    draw_line(point, delta, color, thickness);
    point.y += delta.y;
    delta.x = -hw.x;
    delta.y = 0;
    draw_line(point, delta, color, thickness);
    point.x += delta.x;
    delta.x = 0;
    delta.y = -hw.y;
    draw_line(point, delta, color, thickness);
}

void
draw_rect_textured(vec2 xy, vec2 hw, vec4 color, int texture, vec2 uv, vec2 st)
{
    ds_prepare_program(PROGRAM_TRIANGLES_TEXTURED);
    GLuint idx = dstream.next_index;

    struct vertex_v2ft2fc4f vertices[4];
    /* A C => ABC, CDB
     * B D
     */
    GLuint indices[6] = { idx, idx + 1, idx + 2, idx + 2, idx + 3, idx + 1 };

    vertices[0].pos.x = xy.x;
    vertices[0].pos.y = xy.y;
    vertices[0].uv = uv;
    vertices[0].color = color;

    vertices[1].pos.x = xy.x;
    vertices[1].pos.y = xy.y + hw.y;
    vertices[1].uv = (vec2){ uv.x, uv.y + st.y };
    vertices[1].color = color;

    vertices[2].pos.x = xy.x + hw.x;
    vertices[2].pos.y = xy.y + hw.y;
    vertices[1].uv = (vec2){ uv.x + st.x, uv.y + st.y };
    vertices[2].color = color;

    vertices[3].pos.x = xy.x + hw.x;
    vertices[3].pos.y = xy.y;
    vertices[1].uv = (vec2){ uv.x + st.x, uv.y };
    vertices[3].color = color;

    dis_push_vertices(4, sizeof(struct vertex_v2ft2fc4f), vertices);
    dis_push_indices(6, indices);
}

void
draw_string(vec2 xy, const char *string, texture_font_t *font, vec4 color)
{
    ds_prepare_program(PROGRAM_FREETYPE);
}

void
draw_string_outline(vec2 xy, const char *string, texture_font_t *font, vec4 color)
{
    ds_prepare_program(PROGRAM_FREETYPE);
}

void
draw_string_with_outline(vec2 xy, const char *string, texture_font_t *font, vec4 color, vec4 outline_color)
{
    ds_prepare_program(PROGRAM_FREETYPE);
}

void
get_string_size(const char *string, texture_font_t *font, vec2 *out)
{

}
