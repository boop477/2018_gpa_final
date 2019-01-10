//
//  CameraChar.hpp
//  103033241_AS4
//
//  Created by Softcat on 1/10/19.
//  Copyright © 2019 NTHU. All rights reserved.
//
//    Character = fbximport*n1 + CameraChar*n2

#ifndef CameraChar_hpp
#define CameraChar_hpp

#include <stdio.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#include <unistd.h>
#include "../Include/GLM/glm/glm.hpp"
#include "../Include/GLM/glm/gtc/matrix_transform.hpp"
#include "../Include/GLM/glm/gtc/type_ptr.hpp"

class CameraChar{
public:
    CameraChar(glm::vec3 position, glm::vec3 char_position, glm::vec3 eye_front);
    void reshape(int, int);
    void moveWithChar(glm::vec3 char_position, glm::vec3 eye_front);
    void rotateEyePos(glm::vec3 char_position, float yaw);
    void moveEyePos(glm::vec3 char_position);
    void rotateEyeFront(float yaw, float pitch);
    
    glm::mat4 getView();
    glm::mat4 getProjection();
    
    glm::vec3 eye_pos; // use character._position + _position to get this variable
    glm::vec3 eye_front; // This var is shared with character
    glm::vec3 eye_up = glm::vec3(0.0f, 1.0f, 0.0f); // Fixed variable. Do not change this
    
    float pitch = 0.0f;
    
    glm::vec3 _position; // Position relative to characters
    glm::vec3 _position_base; // Position relative to characters
    
private:
    glm::mat4 view;
    glm::mat4 projection;
    
};

#endif /* CameraChar_hpp */
