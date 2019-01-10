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
    Character(Fbximport* idle, Fbximport* run);
    //void mouse_update(int mouse_x, int mouse_y, int height, int width);
    void mouse_update();
    void key_update(unsigned char key);
    void key_update_up(unsigned char key);
    
    void selectThird(){current_camera = third_camera;}
    void selectFirst(){current_camera = first_camera;}
    void selectRun(){current_model = _run;}
    void selectIdle(){current_model = _idle;}
    
    void reshape(int width, int height){third_camera->reshape(width, height);
                                        first_camera->reshape(width, height);}
    void updateTimer();
    void trackballFlag(int mouse_x, int mouse_y, int width, int height);
    
    void draw(UniformList uniform_list,
              glm::mat4 view_matrix,
              glm::mat4 proj_matrix,
              glm::mat4 light_vp_matrix,
              BfShadingEffect bfshading_effect);
    void draw(UniformList uniform_list,
              glm::mat4 light_vp_matrix);
    void draw(UniformList uniform_list,
              glm::mat4 view_matrix,
              glm::mat4 proj_matrix);
    
    CameraChar* current_camera;
    Fbximport* current_model;
    bool isMoving = false;
private:
    Fbximport* _idle;
    Fbximport* _run;
    
    glm::vec3 _eye_front = glm::vec3(1.0f, 0.0f, 0.0f);
    CameraChar* third_camera;
    CameraChar* first_camera;
    
    bool update_yaw = false;
    float yaw = 0.0;
    float yaw_add = 0.0;
    bool update_pitch = false;
    float pitch = 0.0;
};

#endif /* Character_hpp */
