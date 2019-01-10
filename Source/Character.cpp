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
    third_camera = new CameraChar(glm::vec3(-1.5, 2.0, 0.0), _model->getPosition(), _eye_front);
}
void Character::mouse_update(int mouse_x, int mouse_y, int height, int width){
    mouse_x -= width / 2;
    mouse_y -= height / 2;
    
    // Map from [-width/2, width/2] to [-180.0, 180.0]
    //float yaw = float(mouse_x) / (width / 2)*180.0;
    // Map from [-height/2, height/2] to [-89.0, 89.0]
    float pitch = float(mouse_y) / (height / 2)*-89.0;
    //pitch = 0.0;
    
    if (mouse_x >= 0){
        yaw += 5.0;
    }else{
        yaw += -5.0;
    }
    
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(0.0));
    front.y = sin(glm::radians(0.0));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(0.0));
    _eye_front = glm::normalize(front);
    _model->addQuarternion2Base(glm::quat(glm::vec3(glm::radians(0.0),glm::radians(-yaw), glm::radians(0.0))));
    //_model->setQuaternion(glm::quat(glm::vec3(glm::radians(0.0),glm::radians(-yaw), glm::radians(0.0))));
    
    /*yaw = -yaw;
    glm::vec3 new_position;
    new_position.x = cos(glm::radians(yaw))*(third_camera->_position_base.x) +
                     sin(glm::radians(yaw))*(third_camera->_position_base.z);
    new_position.y = third_camera->_position.y;
    new_position.z = -sin(glm::radians(yaw))*(third_camera->_position_base.x) +
                     cos(glm::radians(yaw))*(third_camera->_position_base.z);
    third_camera->_position = new_position;*/
    
    if (mouse_y >= height/2){
        // camera look down
    }
    else{
        // camera look up
    }
    //third_camera->moveWithChar(_model->getPosition(), _eye_front);
    third_camera->rotateEyePos(_model->getPosition(), yaw);
    third_camera->rotateEyeFront(yaw, pitch);
}
void Character::mouse_update(){
    // In MyDraw()
    if (update_yaw){
        yaw += yaw_add;
        
        // _eye_front of the character can't pitch
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(0.0));
        front.y = sin(glm::radians(0.0));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(0.0));
        _eye_front = glm::normalize(front);
        _model->addQuarternion2Base(glm::quat(glm::vec3(glm::radians(0.0),glm::radians(-yaw), glm::radians(0.0))));
        
        third_camera->rotateEyePos(_model->getPosition(), yaw);
        third_camera->rotateEyeFront(yaw, pitch);
    }
    
    if(update_pitch){
        third_camera->rotateEyeFront(yaw, pitch);
        update_pitch = false;
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
    //third_camera->moveWithChar(_model->getPosition(), _eye_front);
    third_camera->moveEyePos(_model->getPosition());
}
void Character::trackballFlag(int mouse_x, int mouse_y, int width, int height){
    update_pitch = true;
    
    //printf("%d %d\n", mouse_x, mouse_y);
    
    mouse_x -= width / 2;
    mouse_y -= height / 2;
    
    pitch = float(mouse_y) / (height / 2)*-89.0;
    float threshold = float(width)/4.0;
    if (mouse_x >= -threshold/2 && mouse_x <= threshold/2){
        update_yaw = false;
    }else if (mouse_x > threshold){
        update_yaw = true;
        yaw_add = 3.0;
    }else if (mouse_x < -threshold){
        update_yaw = true;
        yaw_add = -3.0;
    }else if (mouse_x >= 0){
        update_yaw = true;
        yaw_add = abs(float((mouse_x-threshold/2)/(threshold/2)*3.0));
    }else{
        update_yaw = true;
        yaw_add = -1* abs(float((mouse_x+threshold/2)/(threshold/2)*3.0));
    }
}
