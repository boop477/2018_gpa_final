//
//  Character.cpp
//  103033241_AS4
//
//  Created by Softcat on 1/10/19.
//  Copyright © 2019 NTHU. All rights reserved.
//

#include "Character.hpp"


Character::Character(Fbximport* idle, Fbximport* run){
    _run = run;
    _idle = idle;
    third_camera = new CameraChar(glm::vec3(-1.5, 1.5, 0.0), _run->getPosition(), _eye_front);
    first_camera = new CameraChar(glm::vec3(1.5, 1.5, 0.0), _run->getPosition(), _eye_front);
    this->selectFirst();
    current_model = _idle;
}
void Character::updateTimer(){
    _idle->updateTimer();
    _run->updateTimer();
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
        _run->addQuarternion2Base(glm::quat(glm::vec3(glm::radians(0.0),glm::radians(-yaw), glm::radians(0.0))));
        _idle->addQuarternion2Base(glm::quat(glm::vec3(glm::radians(0.0),glm::radians(-yaw), glm::radians(0.0))));
        
        third_camera->rotateEyePos(_run->getPosition(), yaw);
        third_camera->rotateEyeFront(yaw, pitch);
        first_camera->rotateEyePos(_run->getPosition(), yaw);
        first_camera->rotateEyeFront(yaw, pitch);
    }
    
    if(update_pitch){
        third_camera->rotateEyeFront(yaw, pitch);
        first_camera->rotateEyeFront(yaw, pitch);
        update_pitch = false;
    }
}
void Character::key_update_up(unsigned char key){
    if(key == 'w'){
        isMoving = false;
        selectIdle();
    }
}
void Character::key_update(unsigned char key){
    if (key == 'w') {
        _run->addPosition(_eye_front / glm::vec3(5.0));
        _idle->addPosition(_eye_front / glm::vec3(5.0));
        if (!isMoving){
            isMoving = true;
            selectRun();
        }
    }
    else if (key == 's') {
        _run->addPosition(_eye_front*glm::vec3(-1.0) / glm::vec3(5.0));
        _idle->addPosition(_eye_front*glm::vec3(-1.0) / glm::vec3(5.0));
    }
    else if (key == 'a') {
        glm::vec3 vec = _eye_front / glm::vec3(5.0);
        
        glm::mat3 rot = glm::mat3();
        rot[0] = glm::vec3(cos(glm::radians(90.0f)), 0.0, sin(glm::radians(90.0f)));
        rot[1] = glm::vec3(0.0, 1.0, 0.0);
        rot[2] = glm::vec3(-sin(glm::radians(90.0f)), 0.0, cos(glm::radians(90.0f)));
        
        glm::vec3 move = rot * (_eye_front / glm::vec3(5.0));
        _run->addPosition(move*glm::vec3(-1.0));
        _idle->addPosition(move*glm::vec3(-1.0));
    }
    else if (key == 'd') {
        glm::vec3 vec = _eye_front / glm::vec3(5.0);
        
        glm::mat3 rot = glm::mat3();
        rot[0] = glm::vec3(cos(glm::radians(90.0f)), 0.0, sin(glm::radians(90.0f)));
        rot[1] = glm::vec3(0.0, 1.0, 0.0);
        rot[2] = glm::vec3(-sin(glm::radians(90.0f)), 0.0, cos(glm::radians(90.0f)));
        
        glm::vec3 move = rot * (_eye_front / glm::vec3(5.0));
        _run->addPosition(move*glm::vec3(1.0));
        _idle->addPosition(move*glm::vec3(1.0));
    }
    //third_camera->moveWithChar(_model->getPosition(), _eye_front);
    third_camera->moveEyePos(_run->getPosition());
    first_camera->moveEyePos(_run->getPosition());
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
void Character::draw(UniformList uniform_list,
                     glm::mat4 view_matrix,
                     glm::mat4 proj_matrix,
                     glm::mat4 light_vp_matrix,
                     BfShadingEffect bfshading_effect){
    current_model->draw(uniform_list,
                        view_matrix,
                        proj_matrix,
                        light_vp_matrix,
                        bfshading_effect);
}
void Character::draw(UniformList uniform_list,
                     glm::mat4 light_vp_matrix){
    current_model->draw(uniform_list,
                        light_vp_matrix);
}
void Character::draw(UniformList uniform_list,
                     glm::mat4 view_matrix,
                     glm::mat4 proj_matrix){
    current_model->draw(uniform_list,
                        view_matrix,
                        proj_matrix);
}
