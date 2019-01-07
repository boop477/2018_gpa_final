//
//  Helper.h
//  103033241_AS4
//
//  Created by Softcat on 12/18/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#ifndef Helper_h
#define Helper_h
#include "../Include/Common.h"
#include "UniformList.h"
#include <iostream>
#include <string>

// For shader and program debugging usage
GLint status = GL_TRUE;
GLsizei read_num;
char error_msg[1024];
struct ViewportSize{
    int width;
    int height;
};

using namespace glm;
// Print debug message
void programDebug(GLuint program, std::string p_error_msg ){
    glGetShaderiv(program, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(program, 1024, &read_num, error_msg);
        std::cout << p_error_msg << std::endl << error_msg << std::endl;
    }
}
// Load shader
char** loadShaderSource(const char* file)
{
    FILE* fp = fopen(file, "rb");
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *src = new char[sz + 1];
    fread(src, sizeof(char), sz, fp);
    src[sz] = '\0';
    char **srcp = new char*[1];
    srcp[0] = src;
    return srcp;
}
// free shader
void freeShaderSource(char** srcp)
{
    delete srcp[0];
    delete srcp;
}
// Create a program and load both vertex and fragment shader
GLuint createProgram(std::string vertex_shader,
                     std::string fragment_shader,
                     std::string program_name){
    GLuint program = glCreateProgram();
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    char** fragmentShaderSource = loadShaderSource(fragment_shader.c_str());
    glShaderSource(fs, 1, fragmentShaderSource, NULL);
    glCompileShader(fs);
    programDebug(fs, program_name+":fs:");
    freeShaderSource(fragmentShaderSource);
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    char** vertexShaderSource = loadShaderSource(vertex_shader.c_str());
    glShaderSource(vs, 1, vertexShaderSource, NULL);
    glCompileShader(vs);
    programDebug(vs, program_name+":vs:");
    freeShaderSource(vertexShaderSource);
    
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    
    glLinkProgram(program);
    return program;
}
void printUniformList(UniformList u_list){
    printf("= = = =\n");
    printf("[uniform list]");
    printf("depth.mvp:%d\n", u_list.depth.mvp);
    printf("render.tex_cubemap:%d\n", u_list.render.tex_cubemap);
    printf("render.tex_shadow:%d\n", u_list.render.tex_shadow);
    printf("skybox.tex_cubemap:%d\n", u_list.skybox.tex_cubemap);
    printf("fbo2screen.tex_sb:%d\n", u_list.fbo2screen.tex_sb);
    printf("fbo2screen.tex_sobj:%d\n", u_list.fbo2screen.tex_sobj);
    printf("fbo2screen.tex_snoobj:%d\n", u_list.fbo2screen.tex_snoobj);
    printf("= = = =\n");
}

/*
 // Mesh.cpp _init() in single shape
 void Mesh::_init(){
 std::vector<tinyobj::shape_t> shapes;
 std::vector<tinyobj::material_t> materials;
 std::string err;
 tinyobj::LoadObj(shapes, materials, err, _file_dir.c_str());
 
 glGenVertexArrays(1, &_vao);
 glBindVertexArray(_vao);
 
 GLuint position_buffer;
 GLuint normal_buffer;
 GLuint index_buffer;
 
 glGenBuffers(1, &position_buffer);
 glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
 glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.positions.size() * sizeof(float), shapes[0].mesh.positions.data(), GL_STATIC_DRAW);
 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
 glEnableVertexAttribArray(0);
 
 glGenBuffers(1, &normal_buffer);
 glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
 glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.normals.size() * sizeof(float), shapes[0].mesh.normals.data(), GL_STATIC_DRAW);
 glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
 glEnableVertexAttribArray(1);
 
 glGenBuffers(1, &index_buffer);
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
 glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size() * sizeof(unsigned int), shapes[0].mesh.indices.data(), GL_STATIC_DRAW);
 _index_count = shapes[0].mesh.indices.size();
 }
 */

/*
 void Mesh::draw(UniformList uniform_list,
                 glm::mat4 view_matrix,
                 glm::mat4 proj_matrix,
                 glm::mat4 light_vp_matrix,
                 bool is_shadow){
    glBindVertexArray(_vao);  //Bind vao
    
    glm::mat4 model = this->getModelMatrix();   // Assign uniforms
    glm::mat4 light_mvp_matrix = light_vp_matrix * model;
    glUniformMatrix4fv(uniform_list.render.model_matrix, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(uniform_list.render.view_matrix, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(uniform_list.render.proj_matrix, 1, GL_FALSE, &proj_matrix[0][0]);
    glUniformMatrix4fv(uniform_list.render.light_mvp_matrix, 1, GL_FALSE, &light_mvp_matrix[0][0]);
    glUniform1i(uniform_list.render.is_quad, 0);
    if(is_shadow){
        glUniform1i(uniform_list.render.is_shadow, 1);
    }
    else{
        glUniform1i(uniform_list.render.is_shadow, 0);
    }
    
    glDrawElements(GL_TRIANGLES, _index_count, GL_UNSIGNED_INT, 0);
    
    glUniform1i(uniform_list.render.is_quad, 0);
}
 */

/*// == Draw the skybox == //
 // Clear Buffer and Bind texture
 glBindFramebuffer(GL_FRAMEBUFFER, 0); // Draw to default buffer
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 glViewport(0, 0, viewport_size.width, viewport_size.height);
 glClearBufferfv(GL_COLOR, 0, pink);
 glClearBufferfv(GL_DEPTH, 0, ones);
 
 // == Draw the models == //
 // - Draw skybox - //
 glUseProgram(skybox_prog);
 
 glActiveTexture(GL_TEXTURE2);
 glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
 glUniform1i(uniforms.skybox.tex_cubemap, 2);
 
 cube_map->draw(uniforms, view_matrix, inv_vp_matrix);
 
 // - Draw the ladybug and the quad - //
 glUseProgram(render_prog);
 
 // Bind textures
 glActiveTexture(GL_TEXTURE1);
 glBindTexture(GL_TEXTURE_2D, shadow_fbo->depth_map);
 glUniform1i(uniforms.render.tex_shadow, 1);
 
 glActiveTexture(GL_TEXTURE2);
 glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
 glUniform1i(uniforms.render.tex_cubemap, 2);
 // draw mesh
 mesh->draw(uniforms, view_matrix, proj_matrix, shadow_sbpv_matrix, false);*/

#endif /* Helper_h */
