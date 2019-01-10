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
        GLint is_normal_map;
        GLint is_ssao;
        
        GLint texture_diffuse1;
        GLint tex_cubemap;
        GLint tex_shadow;
        GLint tex;
        GLint tex_ssao;
        GLint tex_normal_map;
        
        GLuint program_id;
    } render;
    struct
    {
        /*
        GLint mv_matrix;
        GLint model_matrix;
        GLint view_matrix;
        GLint proj_matrix;
        GLint light_mvp_matrix;
        
        GLint is_quad;
        GLint is_shadow;
        GLint tex;
        GLint tex_cubemap;
        GLint tex_shadow;*/
        GLint model_matrix;
        GLint view_matrix;
        GLint proj_matrix;
        GLint tex;
    } tex_render;
    struct
    {
        GLint mv_matrix;
        GLint proj_matrix;
    } depth_normal;
    struct
    {
        GLint normal_map;
        GLint depth_map;
        GLint noise_map;
        GLint noise_scale;
        GLint proj;
    } ssao;
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
