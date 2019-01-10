//
//  Character.hpp
//  103033241_AS4
//
//  Created by Softcat on 1/10/19.
//  Copyright © 2019 NTHU. All rights reserved.
//

#ifndef Character_hpp
#define Character_hpp

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
#include "FBXLoader/fbximport.h"

class Character{
public:
    Character(Fbximport* model);
    void mouse_update(int mouse_x, int mouse_y, int height, int width);
    void key_update(unsigned char key);
    
private:
    Fbximport* _model;
    glm::vec3 _eye_front = glm::vec3(1.0f, 0.0f, 0.0f);
};

#endif /* Character_hpp */
