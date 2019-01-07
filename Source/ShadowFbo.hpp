//
//  ShadowFbo.hpp
//  103033241_AS4
//
//  Created by Softcat on 12/22/18.
//  Copyright © 2018 NTHU. All rights reserved.
//

#ifndef ShadowFbo_hpp
#define ShadowFbo_hpp

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

#define SHADOW_MAP_SIZE 4096

class ShadowFbo{
public:
    GLuint fbo;
    GLuint depth_map;
    GLuint texture_map;
    GLuint window_vao;
    
    ShadowFbo(std::string name);
    void reshape();
    void log();
    
    void beforeDraw();
    void afterDraw();
private:
    
    std::string _name;
    
};


#endif /* ShadowFbo_hpp */
