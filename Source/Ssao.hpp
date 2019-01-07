//
//  Ssao.hpp
//  103033241_AS4
//
//  Created by Softcat on 1/8/19.
//  Copyright © 2019 NTHU. All rights reserved.
//

#ifndef Ssao_hpp
#define Ssao_hpp

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

class Ssao{
public:
    Ssao();
    void loadVao(GLuint ssao_prog);
    void loadKernalUbo();
    void loadNoiseMap();
    
    GLuint ssao_vao;
    GLuint kernal_ubo;
    GLuint noise_map;
};

#endif /* Ssao_hpp */
