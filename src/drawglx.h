
/*
 * Libhydride: A transparent drawable GL layer for your desktop! 
 * Copyright (C) 2022 Rebekah Rowe
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

typedef struct
{
    int version_major;
    int version_minor;
    GLXContext context;
} hydride_glx_state;

extern hydride_glx_state glx_state;

int hydride_glx_init();
int hydride_glx_create_window();
int hydride_glx_destroy();
