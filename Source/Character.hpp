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
#include "CameraChar.hpp"

class Character{
public:
    Character(Fbximport* model);
    void mouse_update(int mouse_x, int mouse_y, int height, int width);
    void mouse_update();
    void key_update(unsigned char key);
    void selectFirst(){current_camera = third_camera;}
    void reshape(int width, int height){third_camera->reshape(width, height);}
    void trackballFlag(int mouse_x, int mouse_y, int width, int height);
    
    CameraChar* current_camera;
    
private:
    Fbximport* _model;
    glm::vec3 _eye_front = glm::vec3(1.0f, 0.0f, 0.0f); // This var is shared with cameras
    CameraChar* third_camera;
    
    bool update_yaw = false;
    float yaw = 0.0;
    float yaw_add = 0.0;
    bool update_pitch = false;
    float pitch = 0.0;
};

#endif /* Character_hpp */
