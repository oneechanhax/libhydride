/*
 * vertex_structs.h
 *
 *  Created on: Nov 10, 2017
 *      Author: nullifiedcat
 */

#pragma once

/* Used for drawing primitives */
struct vertex_v2fc4f
{
    vec2 pos;
    vec4 color;
};

/* Used for drawing textured primitives and freetype-gl fonts */
struct vertex_v2ft2fc4f
{
    vec2 pos;
    vec2 uv;
    vec4 color;
};


