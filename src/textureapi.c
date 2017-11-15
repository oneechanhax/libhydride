/*
 * textureapi_internal.c
 *
 *  Created on: Nov 11, 2017
 *      Author: nullifiedcat
 */

#include "drawglx_internal.h"
#include "textureapi_internal.h"
#include "log.h"

#include <string.h>
#include <memory.h>
#include <stdio.h>

int
textureapi_load_png_rgba(const char *name, struct textureapi_texture_t *out)
{
    memset(out, 0, sizeof(struct textureapi_texture_t));

    FILE *file;
    file = fopen(name, "r");
    if (file == NULL)
    {
        perror("textureapi: could not open file: ");
        return -1;
    }
    png_byte header[8];
    fread(header, 1, 8, file);
    if (png_sig_cmp(header, 0, 8))
    {
        log_write("textureapi: not a PNG file\n");
        fclose(file);
        return -1;
    }
    png_structp pngstr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (pngstr == NULL)
    {
        log_write("textureapi: png creation error\n");
        return -1;
    }
    png_infop pnginfo = png_create_info_struct(pngstr);
    png_infop pngend = png_create_info_struct(pngstr);
    if (setjmp(png_jmpbuf(pngstr)))
    {
        png_destroy_read_struct(pngstr, pnginfo, pngend);
        return -1;
    }
    png_init_io(pngstr, file);
    png_set_sig_bytes(pngstr, 8);
    png_read_info(pngstr, pnginfo);
    int width, height, depth, colortype;
    png_get_IHDR(pngstr, pnginfo, &width, &height, &depth, &colortype, NULL, NULL, NULL);
    png_read_update_info(pngstr, pnginfo);
    int row_bytes;
    if (PNG_COLOR_TYPE_RGBA != png_get_color_type(pngstr, pnginfo))
    {
        log_write("textureapi: not RGBA PNG\n");
        png_destroy_read_struct(pngstr, pnginfo, pngend);
        fclose(file);
        return -1;
    }

    png_bytepp row_pointers;
    row_bytes = png_get_rowbytes(pngstr, pnginfo);
    out->data = malloc(row_bytes * height * sizeof(png_byte));
    if (out->data == NULL)
    {
        log_write("malloc error\n");
        png_destroy_read_struct(pngstr, pnginfo, pngend);
        fclose(file);
        return -1;
    }
    row_pointers = malloc(height * sizeof(png_bytep));
    if (row_pointers == NULL)
    {
        log_write("malloc error\n");
        png_destroy_read_struct(pngstr, pnginfo, pngend);
        free(out->data);
        fclose(file);
        return -1;
    }
    for (int i = 0; i < height; ++i)
    {
        row_pointers[height - 1 - i] = out->data + i * row_bytes;
    }
    png_read_image(pngstr, row_pointers);

    // png_destroy_read_struct(pngstr, pnginfo, pngend);
    free(row_pointers);
    fclose(file);
    out->width = width;
    out->height = height;
    out->init = 1;
    return 0;
}


void
textureapi_bind(xoverlay_texture_handle_t handle)
{
    struct textureapi_texture_t *texture = textureapi_get(handle);
    if (texture == NULL || texture->init == 0)
        return;
    if (!texture->bound)
    {
        log_write("generating texture\n");
        glGenTextures(1, &texture->texture_id);
        glBindTexture(GL_TEXTURE_2D, texture->texture_id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        texture->bound = 1;
    }
    ds_use_texture(texture->texture_id);
}

xoverlay_texture_handle_t
xoverlay_texture_load_png_rgba(const char *path)
{
    struct textureapi_texture_t result;
    result.bound = 0;
    strncpy(result.filename, path, 255);
    if (textureapi_load_png_rgba(path, &result) != 0)
    {
        log_write("textureapi: could not load texture file %s\n", path);
    }
    return textureapi_add_texture(result);
}


void
xoverlay_texture_get_size(xoverlay_texture_handle_t handle, int *width, int *height)
{
    struct textureapi_texture_t *tx = textureapi_get(handle);
    if (tx == NULL)
        return;
    if (width)
        *width = tx->width;
    if (height)
        *height = tx->height;
}

struct textureapi_texture_t *
textureapi_get(xoverlay_texture_handle_t handle)
{
    if (handle == 0 || handle >= XOVERLAY_TEXTURE_COUNT)
        return NULL;
    if (loaded_textures[handle - 1].init == 0)
        return NULL;
    return &loaded_textures[handle - 1];
}

int
textureapi_init()
{
    memset(loaded_textures, 0, sizeof(loaded_textures));
    return 0;
}

void
textureapi_destroy()
{

}

xoverlay_texture_handle_t
textureapi_add_texture(struct textureapi_texture_t texture)
{
    for (xoverlay_texture_handle_t i = 0; i < XOVERLAY_TEXTURE_COUNT; ++i)
    {
        if (loaded_textures[i].init == 0)
        {
            memcpy(&loaded_textures[i], &texture, sizeof(texture));
            loaded_textures[i].init = 1;
            return i + 1;
        }
    }
    return 0;
}

void
xoverlay_texture_unload(xoverlay_texture_handle_t handle)
{
    // TODO
}
