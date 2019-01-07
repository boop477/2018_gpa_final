//
//  Shape.h
//  Quiz Framework
//
//  Created by Softcat on 10/27/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#ifndef Shape_h
#define Shape_h

#include "Common.h"
#include "Shape.h"

typedef struct
{
    GLuint vao;            // vertex array object
    GLuint vbo;            // vertex buffer object
    GLuint vboTex;        // vertex buffer object of texture
    GLuint ebo;
    
    GLuint p_normal;
    int materialId;
    int indexCount;
    GLuint m_texture;
} Shape;

#endif /* Shape_h */
