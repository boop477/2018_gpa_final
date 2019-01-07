//
//  Quad.cpp
//  103033241_AS4
//
//  Created by Softcat on 12/19/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

//#include "Mesh.hpp"
#include "UniformList.h"
#include "Quad.hpp"
#include <iostream>
#include <stdio.h>
#include <string>
#include "NotImplementedError.h"
#include "../Include/GLM/glm/gtx/transform.hpp"
#include "../Include/GLM/glm/gtx/quaternion.hpp"

Quad::Quad(glm::vec3 position,
           glm::vec3 scale,
           glm::quat quaternion){
    _vao = 999;
    this->_position = position;
    this->_quaternion = quaternion;
    this->_scale = scale;
    _init();
}
void Quad::draw(UniformList uniform_list,
                glm::mat4 view_matrix,
                glm::mat4 proj_matrix,
                glm::mat4 light_vp_matrix,
                bool is_shadow){
    /*
     * Should invoke glUseProgram before calling this function.
     * The purpose of this function is to draw a quad with or without shading.
     */
    glBindVertexArray(_vao);
    
    glm::mat4 model = getModelMatrix();   // Assign uniforms
    glm::mat4 light_mvp_matrix = light_vp_matrix * model;
    glUniformMatrix4fv(uniform_list.render.model_matrix, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(uniform_list.render.view_matrix, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(uniform_list.render.proj_matrix, 1, GL_FALSE, &proj_matrix[0][0]);
    glUniformMatrix4fv(uniform_list.render.light_mvp_matrix, 1, GL_FALSE, &light_mvp_matrix[0][0]);
    glUniform1i(uniform_list.render.is_quad, 1);
    if(is_shadow){
        glUniform1i(uniform_list.render.is_shadow, 1);
    }
    else{
        glUniform1i(uniform_list.render.is_shadow, 0);
    }
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glUniform1i(uniform_list.render.is_quad, 0);
    glBindVertexArray(0);
}
void Quad::_init(){
    float plane_vertices[]={
        25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
        
        25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
        25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    
    // plane VAO
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindVertexArray(vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
}
glm::mat4 Quad::getModelMatrix(){
    glm::mat4 translation_matrix = glm::translate(glm::mat4(), this->_position);
    glm::mat4 rotation_matrix = glm::toMat4(this->_quaternion);
    glm::mat4 model = translation_matrix * rotation_matrix;
    model = glm::scale(model, this->_scale);
    return model;
}
void Quad::draw(UniformList uniform_list,
                glm::mat4 light_vp_matrix){
    /*
     * Should invoke glUseProgram before calling this function.
     * The purpose of this function is getting a depth map.
     */
    glBindVertexArray(_vao);
    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(uniform_list.depth.mvp, 1, GL_FALSE, &(light_vp_matrix*model)[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
