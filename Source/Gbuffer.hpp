//
//  Gbuffer.hpp
//  103033241_AS4
//
//  Created by Softcat on 1/7/19.
//  Copyright © 2019 NTHU. All rights reserved.
//

#ifndef Gbuffer_hpp
#define Gbuffer_hpp

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

class Gbuffer{
public:
    GLuint fbo;
    GLuint normal_map;
    GLuint depth_map;
    
    Gbuffer(std::string name);
    void reshape(int width, int height);
    void log();
    
    void beforeDraw();
    void afterDraw();
private:
    std::string _name;
};


#endif /* Gbuffer_hpp */
