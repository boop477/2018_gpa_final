//
//  Character.cpp
//  103033241_AS4
//
//  Created by Softcat on 1/10/19.
//  Copyright © 2019 NTHU. All rights reserved.
//

#include "Character.hpp"

Character::Character(Fbximport* model){
    _model = model;
}
void Character::mouse_update(int mouse_x, int mouse_y, int height, int width){
    mouse_x -= width / 2;
    mouse_y -= height / 2;
    
    // Map from [-width/2, width/2] to [-180.0, 180.0]
    float yaw = float(mouse_x) / (width / 2)*180.0;
    // Map from [-height/2, height/2] to [-89.0, 89.0]
    float pitch = float(mouse_y) / (height / 2)*-89.0;
    pitch = 0.0;
    
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    _eye_front = glm::normalize(front);
    _model->addQuarternion2Base(glm::quat(glm::vec3(glm::radians(0.0),glm::radians(-yaw), glm::radians(0.0))));
    
    if (mouse_y >= height/2){
        // camera look down
    }
    else{
        // camera look up
    }
}
void Character::key_update(unsigned char key){
    if (key == 'w') {
        _model->addPosition(_eye_front / glm::vec3(5.0));
        printf ("char->w");
    }
    else if (key == 's') {
        _model->addPosition(_eye_front*glm::vec3(-1.0) / glm::vec3(5.0));
        printf ("char->s");
    }
    else if (key == 'a') {
        glm::vec3 vec = _eye_front / glm::vec3(5.0);
        
        glm::mat3 rot = glm::mat3();
        rot[0] = glm::vec3(cos(glm::radians(90.0f)), 0.0, sin(glm::radians(90.0f)));
        rot[1] = glm::vec3(0.0, 1.0, 0.0);
        rot[2] = glm::vec3(-sin(glm::radians(90.0f)), 0.0, cos(glm::radians(90.0f)));
        
        glm::vec3 move = rot * (_eye_front / glm::vec3(5.0));
        _model->addPosition(move*glm::vec3(-1.0));
        printf ("char->a");
    }
    else if (key == 'd') {
        glm::vec3 vec = _eye_front / glm::vec3(5.0);
        
        glm::mat3 rot = glm::mat3();
        rot[0] = glm::vec3(cos(glm::radians(90.0f)), 0.0, sin(glm::radians(90.0f)));
        rot[1] = glm::vec3(0.0, 1.0, 0.0);
        rot[2] = glm::vec3(-sin(glm::radians(90.0f)), 0.0, cos(glm::radians(90.0f)));
        
        glm::vec3 move = rot * (_eye_front / glm::vec3(5.0));
        _model->addPosition(move*glm::vec3(1.0));
        printf ("char->d");
    }
}
