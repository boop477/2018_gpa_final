//
//  Sobj.hpp
//  103033241_AS4
//
//  Created by Softcat on 12/22/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#ifndef Sobj_hpp
#define Sobj_hpp

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

class Sobj{
public:
    GLuint fbo;
    GLuint depth_map;
    GLuint texture_map;
    GLuint stencil_map;
    GLuint depth_stencil_map;
    
    Sobj(std::string name);
    
    
    void beforeMakingStencilMap(int width, int height);
    void afterMakingStencilMap();
    void beforeDrawQuadShadow();
    void afterDrawQuadShadow();
    
    void reshape(int width, int height);
    void log();
    void readme();
private:
    
    std::string _name;
};

#endif /* Sobj_hpp */
