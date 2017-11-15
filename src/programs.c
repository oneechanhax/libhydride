/*
 * draw_commands.c
 *
 *  Created on: Nov 10, 2017
 *      Author: nullifiedcat
 */

#include "drawglx_internal.h"
#include "log.h"

#include <stdio.h>

GLuint compile_shader(const char *source, GLenum type)
{
    GLint status;
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        char error_log[256];
        glGetShaderInfoLog(shader, sizeof(error_log), NULL, error_log);
        log_write("GL Shader compilation error:\n%s\n", error_log);
        exit(1);
    }
    return shader;
}

void
setup_matrices(GLuint shader, int textures)
{
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, 0, ds.model.data);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, 0, ds.view.data);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, 0, ds.projection.data);
    if (textures)
        glUniform1i(glGetUniformLocation(shader, "texture"), 0);
    glUseProgram(0);
}

void
program_init_inplace(struct program_t *program, const char *vertex_format, const char *shader_frag, const char *shader_vert, program_callback_empty_fn init, program_callback_empty_fn load, program_callback_empty_fn render, program_callback_empty_fn unload, program_callback_empty_fn destroy)
{
    program->init = init;
    program->load = load;
    program->render = render;
    program->unload = unload;
    program->destroy = destroy;
    program->vertex = vertex_buffer_new(vertex_format);

    program->shader = glCreateProgram();
    GLint status;

    GLuint sh_frag = compile_shader(shader_frag, GL_FRAGMENT_SHADER);
    glAttachShader(program->shader, sh_frag);
    glDeleteShader(sh_frag);

    GLuint sh_vert = compile_shader(shader_vert, GL_VERTEX_SHADER);
    glAttachShader(program->shader, sh_vert);
    glDeleteShader(sh_vert);

    glLinkProgram(program->shader);
    glGetProgramiv(program->shader, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        char error_log[256];
        glGetShaderInfoLog(program->shader, sizeof(error_log), NULL, error_log);
        log_write("GL Shader linking error:\n%s\n", error_log);
        exit(1);
    }

    program->vertex_size = program->vertex->vertices->item_size;
}

#define PROGRAM_INIT_INPLACE(location, format, fragment, vertex, name) \
    program_init_inplace(location, format, fragment, vertex, PROGRAM_INIT(name), PROGRAM_LOAD(name), PROGRAM_RENDER(name), PROGRAM_UNLOAD(name), PROGRAM_DESTROY(name))

const char *shader_v2fc4f_vert =
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "attribute vec2 vertex;\n"
        "attribute vec4 color;\n"
        "void main()\n"
        "{\n"
        "    gl_FrontColor = color;\n"
        "    gl_Position = projection*(view*(model*vec4(vertex,0.0,1.0)));\n"
        "}";
const char *shader_v2fc4f_frag =
        "void main() {\n"
        "    gl_FragColor = gl_Color;\n"
        "}";
const char *shader_v2ft2fc4f_vert =
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"

        


void
program_init_everything()
{
    log_write("Initializing programs\n");
    PROGRAM_INIT_INPLACE(&programs[PROGRAM_TRIANGLES_PLAIN], "vertex:2f,color:4f", shader_v2fc4f_frag, shader_v2fc4f_vert, triangles_plain);
    setup_matrices(programs[PROGRAM_TRIANGLES_PLAIN].shader, 0);
    PROGRAM_INIT_INPLACE(&programs[PROGRAM_TRIANGLES_TEXTURED], "vertex:2f,tex_coord:2f,color:4f", shader_v2ft2fc4f_frag, shader_v2ft2fc4f_vert, triangles_textured);
    setup_matrices(programs[PROGRAM_TRIANGLES_TEXTURED].shader, 1);
    PROGRAM_INIT_INPLACE(&programs[PROGRAM_FREETYPE], "vertex:2f,tex_coord:2f,color:4f", shader_v2ft2fc4f_freetype_frag, shader_v2ft2fc4f_vert, freetype);
    setup_matrices(programs[PROGRAM_FREETYPE].shader, 1);
}
