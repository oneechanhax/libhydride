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
    float x;
    float y;

    float r;
    float g;
    float b;
    float a;
};

/* Used for drawing textured primitives and freetype-gl fonts */
struct vertex_v2ft2fc4f
{
    float x;
    float y;

    float s;
    float t;

    float r;
    float g;
    float b;
    float a;
};


