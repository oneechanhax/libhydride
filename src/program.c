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
    glUniform1i(glGetUniformLocation(shader, "texture"), 0);
    glUseProgram(0);
}

const char *shader_ultimate_vert =
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "attribute vec2 vertex;\n"
        "attribute vec2 tex_coord;\n"
        "attribute vec4 color;\n"
        "attribute uint draw_mode;\n"
        "flat out uint  frag_DrawMode;\n"
        "void main()\n"
        "{\n"
        "    gl_TexCoord[0].xy = tex_coord.xy;\n"
        "    gl_FrontColor     = color;\n"
        "    gl_Position       = projection*(view*(model*vec4(vertex,0.0,1.0)));\n"
        "    frag_DrawMode     = draw_mode;\n"     
        "}";
const char *shader_ultimate_frag =
        "uniform sampler2D texture;\n"
        "flat in uint frag_DrawMode\n"
        "void main()\n"
        "{\n"
        "   if (frag_DrawMode == 1)\n"
        "       gl_FragColor = gl_Color;\n"
        "   else\n"
        "   {\n"
        "       vec4 tex = texture2D(texture, gl_TexCoord[0].xy);\n"
        "       if (frag_DrawMode == 2)\n"
        "           gl_FragColor = gl_Color * tex\n"
        "       else if (frag_DrawMode == 3)\n"       
        "           gl_FragColor = vec4(gl_Color.rgb, gl_Color.a * texture2D(texture, gl_TexCoord[0].xy).r);\n"
        "       else\n"
        "           gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}";
        
void
program_init()
{
    program.buffer = vertex_buffer_new("vertex:2f,tex_coord:2f,color:4f,draw_mode:1I");
    program.shader = glCreateProgram();
    GLint status;
    GLuint sh_frag = compile_shader(shader_ultimate_frag, GL_FRAGMENT_SHADER);
    glAttachShader(program->shader, sh_frag);
    glDeleteShader(sh_frag);

    GLuint sh_vert = compile_shader(shader_ultimate_vert, GL_VERTEX_SHADER);
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
}