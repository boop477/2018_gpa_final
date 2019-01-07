//
//  Sb.hpp
//  103033241_AS4
//
//  Created by Softcat on 12/26/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#ifndef Sb_hpp
#define Sb_hpp

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

class Sb{
public:
    GLuint fbo;
    GLuint texture_map;
    GLuint depth_stencil_map;
    
    Sb(std::string name);
    
    void beforeDrawSkyboxModel();
    void afterDrawSkyboxModel();
    
    void reshape(int width, int height);
    void log();
    void readme();
private:
    std::string _name;
};

#endif /* Sb_hpp */
