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
    if(mouse_x >= width /2){
        // model rotate right
        _model->addQuaternion(glm::quat(glm::vec3(glm::radians(0.0), glm::radians(5.0), glm::radians(0.0))));
    }
    else{
        // model rotate left
        _model->addQuaternion(glm::quat(glm::vec3(glm::radians(0.0), glm::radians(-5.0), glm::radians(0.0))));
    }
    
    if (mouse_y >= height/2){
        // camera look down
    }
    else{
        // camera look up
    }
}
