//
//  SsaoFbo.hpp
//  103033241_AS4
//
//  Created by Softcat on 1/7/19.
//  Copyright © 2019 NTHU. All rights reserved.
//

#ifndef SsaoFbo_hpp
#define SsaoFbo_hpp

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

class SsaoFbo{
public:
    GLuint fbo;
    GLuint texture_map;
    GLuint depth_stencil_map;
    
    SsaoFbo(std::string name);
    
    void beforeDraw();
    void afterDraw();
    
    void reshape(int width, int height);
    void log();
    void readme();
private:
    std::string _name;
};

#endif /* SsaoFbo_hpp */
