//
//  CubeMap.hpp
//  103033241_AS4
//
//  Created by Softcat on 12/19/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#ifndef CubeMap_hpp
#define CubeMap_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#include <unistd.h>
#include "../Include/GLM/glm/glm.hpp"
#include "UniformList.h"

class CubeMap{
public:
    CubeMap();
    GLuint loadTexture();
    void draw(UniformList,
              glm::mat4,
              glm::mat4);
private:
    GLuint _vao;
    GLuint tex_envmap;
};

#endif /* CubeMap_hpp */
