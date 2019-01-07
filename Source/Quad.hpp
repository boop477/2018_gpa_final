//
//  Quad.hpp
//  103033241_AS4
//
//  Created by Softcat on 12/19/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#ifndef Quad_hpp
#define Quad_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#include <unistd.h>
#include "../Include/GLM/glm/glm.hpp"
#include "../Include/GLM/glm/gtc/matrix_transform.hpp"
#include "../Include/GLM/glm/gtc/quaternion.hpp"
#include "../Include/GLM/glm/gtx/transform.hpp"
#include "UniformList.h"

class Quad{
public:
    Quad(glm::vec3,
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
private:
    void _init();
    GLuint _vao;
    glm::vec3 _position;
    glm::vec3 _scale;
    glm::quat _quaternion;
};

#endif /* Quad_hpp */
