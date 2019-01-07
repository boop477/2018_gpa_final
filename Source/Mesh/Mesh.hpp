//
//  Mesh.hpp
//  103033241_AS4
//
//  Created by Softcat on 12/18/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#include <unistd.h>
#include "../Include/GLM/glm/glm.hpp"
#include "../Include/GLM/glm/gtc/matrix_transform.hpp"
#include "../Include/GLM/glm/gtc/type_ptr.hpp"
#include "../Include/TinyOBJ/tiny_obj_loader.h"

#include "UniformList.h"

class Mesh{
public:
    Mesh(std::string,
         std::string,
         glm::vec3,
         glm::vec3,
         glm::quat);
    void draw(UniformList,
              glm::mat4,
              glm::mat4,
              glm::mat4,
              bool);
    void draw(UniformList,
              glm::mat4);
    glm::mat4 getModelMatrix();
    
    void addPosition(glm::vec3 position);
    void setPosition(glm::vec3 position);
    void addQuaternion(glm::quat quaternion);
    void setQuaternion(glm::quat quaternion);
    void setScale(glm::vec3 scale);
    void addScale(glm::vec3 scale);
    void log();
private:
    void _init();
    
    std::string _name;
    std::string _file_dir;
    glm::vec3 _position;
    glm::vec3 _scale;
    glm::quat _quaternion;
    //GLuint _vao;
    //size_t _index_count;
    std::vector<GLuint> _vao_list;
    std::vector<size_t> _index_count_list;
};

#endif /* Mesh_hpp */

