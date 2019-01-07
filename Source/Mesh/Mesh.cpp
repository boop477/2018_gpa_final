//
//  Mesh.cpp
//  103033241_AS4
//
//  Created by Softcat on 12/18/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#include "Mesh.hpp"
#include "UniformList.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include "NotImplementedError.h"
#include "../Include/GLM/glm/gtx/transform.hpp"
#include "../Include/GLM/glm/gtx/quaternion.hpp"

Mesh::Mesh(std::string name,
           std::string file_dir,
           glm::vec3 position,
           glm::vec3 scale,
           glm::quat quaternion){
    this->_file_dir = file_dir;
    this->_name = name;
    this->_position = position;
    this->_quaternion = quaternion;
    this->_scale = scale;
    _init();
    log();
}
glm::mat4 Mesh::getModelMatrix(){
    glm::mat4 translation_matrix = glm::translate(glm::mat4(), this->_position);
    glm::mat4 rotation_matrix = glm::toMat4(this->_quaternion);
    glm::mat4 model = translation_matrix * rotation_matrix;
    model = glm::scale(model, this->_scale);
    return model;
}
void Mesh::_init(){
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    tinyobj::LoadObj(shapes, materials, err, _file_dir.c_str());
    // Remember to load all the available shapes
    for(int i = 0 ;i < shapes.size(); i++){
        GLuint vao;
        size_t index_count;
        
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        GLuint position_buffer;
        GLuint normal_buffer;
        GLuint index_buffer;
        
        glGenBuffers(1, &position_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
        glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.positions.size() * sizeof(float), shapes[i].mesh.positions.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        
        glGenBuffers(1, &normal_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
        glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.normals.size() * sizeof(float), shapes[i].mesh.normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
        
        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[i].mesh.indices.size() * sizeof(unsigned int), shapes[i].mesh.indices.data(), GL_STATIC_DRAW);
        index_count = shapes[i].mesh.indices.size();
        
        _vao_list.push_back(vao);
        _index_count_list.push_back(index_count);
    }
}
void Mesh::addPosition(glm::vec3 position){
    this->_position += position;
}
void Mesh::setPosition(glm::vec3 position){
    this->_position = position;
}
void Mesh::addQuaternion(glm::quat quaternion){
    this->_quaternion = this->_quaternion * quaternion;
}
void Mesh::setQuaternion(glm::quat quaternion){
    this->_quaternion = quaternion;
}
void Mesh::setScale(glm::vec3 scale){
    this->_scale = scale;
}
void Mesh::addScale(glm::vec3 scale){
    throw NotImplemented();
}
void Mesh::draw(UniformList uniform_list,
                glm::mat4 view_matrix,
                glm::mat4 proj_matrix,
                glm::mat4 light_vp_matrix,
                bool is_shadow){
    /*
     * Should invoke glUseProgram before calling this function.
     * Use this function to draw a model with environment mapping, shadow and BF lighting.
     */
    for(int i = 0; i < _vao_list.size(); i++){
        GLuint vao = _vao_list[i];
        size_t index_count = _index_count_list[i];
        glBindVertexArray(vao);  //Bind vao
        
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
        
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
        
        glUniform1i(uniform_list.render.is_quad, 0);
    }
}
void Mesh::draw(UniformList uniform_list,
                glm::mat4 light_vp_matrix){
    /*
     * Should invoke glUseProgram before calling this function.
     * Invoke this function to get a depth map.
     */
    for(int i = 0; i < _vao_list.size(); i++){
        GLuint vao = _vao_list[i];
        size_t index_count = _index_count_list[i];
        glBindVertexArray(vao);
        glm::mat4 model = this->getModelMatrix();   // Assign uniforms
        glUniformMatrix4fv(uniform_list.depth.mvp, 1, GL_FALSE, value_ptr(light_vp_matrix * model));
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
void Mesh::log(){
    std::cout << "= = = = = =" << std::endl;
    std::cout << " name:" << this->_name << std::endl;
    for(int i = 0; i < _vao_list.size(); i++){
        printf("(%d)vao:%d\n", i, _vao_list[i]);
        printf("index count:%d\n", _index_count_list[i]);
    }
    std::cout << "= = = = = =" << std::endl;
}
