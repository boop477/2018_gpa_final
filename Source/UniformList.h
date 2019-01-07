//
//  UniformList.h
//  103033241_AS4
//
//  Created by Softcat on 12/18/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#ifndef UniformList_h
#define UniformList_h

#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#include <unistd.h>

struct UniformList
{
    struct{
        GLint mvp;
    } depth;
    struct
    {
        GLint mv_matrix;
        GLint model_matrix;
        GLint view_matrix;
        GLint proj_matrix;
        GLint light_mvp_matrix;
        GLint is_quad;
        GLint is_shadow;
        
        GLint tex_cubemap;
        GLint tex_shadow;
    } render;
    struct
    {
        GLint view_matrix;
        GLint inv_vp_matrix;
        GLint tex_cubemap;
    } skybox;
    struct
    {
        GLint tex_sobj;
        GLint tex_snoobj;
        GLint tex_sb;
        GLint tex;
        GLint is_using_df;
    } fbo2screen;
};

#endif /* UniformList_h */
